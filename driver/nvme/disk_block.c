#include "const.h"
#include <asm/io.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/version.h>

#define GET_GLOBAL_SLOT_ID(part_id, local_slot_id)                             \
  (BITMAP_CHUNK_SIZE * part_id + local_slot_id)
#define WRITE_CMD_TAG(global_slot_id) (WRITE_CMD_TAG_OFFSET + global_slot_id)
#define READ_CMD_TAG(global_slot_id) (READ_CMD_TAG_OFFSET + global_slot_id)

static int disk_major = 0;
static volatile u32 __iomem *kram;
static u8 *kbufs[DEVICE_REQ_QUEUE_DEPTH];
static atomic_t req_slot_bitmaps_locks[BITMAP_CHUNK_NUM];
static unsigned long long
    req_slot_bitmaps[BITMAP_CHUNK_NUM] ____cacheline_aligned_in_smp;
static int bitmap_partition_cnts[BITMAP_CHUNK_NUM];
static spinlock_t bitmap_partition_cnts_lock;

static int get_part_id(void) {
  int min_val = DEVICE_REQ_QUEUE_DEPTH, min_id = 0, i;
  spin_lock(&bitmap_partition_cnts_lock);
  for (i = 0; i < BITMAP_CHUNK_NUM; i++) {
    if (bitmap_partition_cnts[i] < min_val) {
      min_val = bitmap_partition_cnts[i];
      min_id = i;
    }
  }
  bitmap_partition_cnts[min_id]++;
  spin_unlock(&bitmap_partition_cnts_lock);
  return min_id;
}

static void put_part_id(int part_id) {
  spin_lock(&bitmap_partition_cnts_lock);
  bitmap_partition_cnts[part_id]--;
  spin_unlock(&bitmap_partition_cnts_lock);
}

static void my_lock(int part_id) {
  u16 delay_time;
  int mask = MASK_MIN;
  while (1) {
    if (!atomic_cmpxchg(&req_slot_bitmaps_locks[part_id], 0, 1)) {
      break;
    }
    get_random_bytes(&delay_time, sizeof(delay_time));
    ndelay(delay_time & ((1 << mask) - 1));
    mask++;
  }
}

static void my_unlock(int part_id) {
  atomic_set(&req_slot_bitmaps_locks[part_id], 0);
  smp_mb__after_atomic();
}

int diskdevice_init(void) {
  int kbuf_fail = 0;
  int i, j;

  spin_lock_init(&bitmap_partition_cnts_lock);

  for (i = 0; i < BITMAP_CHUNK_NUM; i++) {
    atomic_set(&req_slot_bitmaps_locks[i], 0);
    smp_mb__after_atomic();
    for (j = 0; j < BITMAP_CHUNK_SIZE; j++) {
      req_slot_bitmaps[i] |= 1LL << j;
    }
  }

  for (i = 0; i < DEVICE_REQ_QUEUE_DEPTH; i++) {
    kbufs[i] =
        (u8 *)kmalloc(PHYSICAL_SECTOR_SIZE + DEVICE_PCIE_WIDTH, GFP_KERNEL);
    if (kbufs[i] == NULL) {
      kbuf_fail = 1;
    } else {
      memset(kbufs[i], 0, PHYSICAL_SECTOR_SIZE + DEVICE_PCIE_WIDTH);
    }
  }

  kram = (u32 *)ioremap(PHY_MEM_BASE_ADDR, PHY_MEM_SIZE);
  if (kbuf_fail || kram == NULL) {
    return -ENOMEM;
  }

  for (i = 0; i < DEVICE_REQ_QUEUE_DEPTH; i++) {
    u64 addr = (u64)virt_to_phys(kbufs[i]);
    *(kram + KBUF_ADDR_TAG) = addr >> 32;
    *(kram + KBUF_ADDR_TAG) = addr & 0xFFFFFFFF;
  }

  *(kram + HOST_THROTTLE_PARAM_TAG) = HOST_READ_THROTTLE_PARAM;
  *(kram + HOST_THROTTLE_PARAM_TAG) = HOST_WRITE_THROTTLE_PARAM;
  *(kram + DEVICE_DELAY_CYCLE_TAG) = DEVICE_DELAY_CYCLE_CNT;
  *(kram + DEVICE_THROTTLE_PARAM_TAG) = DEVICE_READ_THROTTLE_PARAM;
  *(kram + DEVICE_THROTTLE_PARAM_TAG) = DEVICE_WRITE_THROTTLE_PARAM;
  return DEVICE_SIZE;
}

void diskdevice_cleanup(void) {
  int i;
  if (kram != NULL) {
    iounmap(kram);
  }
  for (i = 0; i < DEVICE_REQ_QUEUE_DEPTH; i++) {
    if (kbufs[i] != NULL) {
      kfree(kbufs[i]);
    }
  }
}

/*
 * The internal structure representation of our Device
 */
static struct disk_device {
  /* Size is the size of the device (in sectors) */
  u32 size;
  /* For exclusive access to our request queue */
  spinlock_t lock;
  /* Our request queue */
  struct request_queue *disk_queue;
  /* This is kernel's representation of an individual disk device */
  struct gendisk *disk;
} disk_dev;

static int disk_open(struct block_device *bdev, fmode_t mode) {
  unsigned unit = iminor(bdev->bd_inode);

  if (unit > MINOR_CNT)
    return -ENODEV;

  return 0;
}

static void disk_close(struct gendisk *disk, fmode_t mode) {}

static int disk_getgeo(struct block_device *bdev, struct hd_geometry *geo) {
  geo->heads = HEAD_CNT;
  geo->cylinders = CYLINDER_CNT;
  geo->sectors = SECTOR_CNT;
  geo->start = 0;
  return 0;
}

__always_inline static int get_req_slot(unsigned long long old_bitmap,
                                        unsigned long long new_bitmap) {
  unsigned long long i = 0;
  for (i = 0; i < BITMAP_CHUNK_SIZE; i++) {
    if ((1LL << i) == (old_bitmap ^ new_bitmap)) {
      return i;
    }
  }
  return 0;
}

__always_inline static void
disk_complete_req(int part_id, int *used_slots_count, int *used_slots,
                  bool *req_types, struct page **req_bv_pages,
                  u8 **trans_buf_addrs, u32 *trans_buf_sectors) {
  int i;
  int local_req_slot, global_req_slot;
  volatile u8 *fin_flag;
  unsigned long long clear_bitmap = 0;

  for (i = 0; i < *used_slots_count; i++) {
    local_req_slot = used_slots[i];
    global_req_slot = GET_GLOBAL_SLOT_ID(part_id, local_req_slot);
    fin_flag = &kbufs[global_req_slot][PHYSICAL_SECTOR_SIZE];
    while (!(*fin_flag))
      ;
    *fin_flag = 0;
    if (req_types[local_req_slot] == READ_TYPE) {
      memcpy(trans_buf_addrs[local_req_slot], kbufs[global_req_slot],
             trans_buf_sectors[local_req_slot] * LOGICAL_SECTOR_SIZE);
      kunmap(req_bv_pages[local_req_slot]);
    }
    clear_bitmap |= 1LL << local_req_slot;
  }
  if (clear_bitmap) {
    my_lock(part_id);
    req_slot_bitmaps[part_id] ^= clear_bitmap;
    my_unlock(part_id);
  }
  *used_slots_count = 0;
}

/*
 * Represents a block I/O request for us to execute
 */
static blk_qc_t disk_make_request(struct request_queue *q, struct bio *bio) {
  struct bvec_iter iter;
  struct bio_vec bvec;
  u8 *buffer;
  sector_t sector_off = bio->bi_iter.bi_sector;
  sector_t sectors;
  bool req_types[BITMAP_CHUNK_SIZE];
  struct page *req_bv_pages[BITMAP_CHUNK_SIZE];
  u8 *trans_buf_addrs[BITMAP_CHUNK_SIZE];
  u32 trans_buf_sectors[BITMAP_CHUNK_SIZE];
  int used_slots[BITMAP_CHUNK_SIZE];
  int used_slots_count = 0;
  unsigned long long old_bitmap;
  unsigned long long new_bitmap;
  bool no_free_slot;
  int local_req_slot, global_req_slot;
  u16 delay_time;
  int mask;
  int part_id;

  get_cpu();
  part_id = get_part_id();

  bio_for_each_segment(bvec, bio, iter) {
    sectors = (bvec.bv_len) >> LOGICAL_SECTOR_SIZE_LOG2;
    buffer = kmap(bvec.bv_page) + bvec.bv_offset;

    if (unlikely(bio_op(bio) == REQ_OP_DISCARD)) {
      printk(KERN_NOTICE "Disk: Skip non-fs request\n");
    } else {
    find_free_slot:
      no_free_slot = false;
      mask = MASK_MIN;
      if (!req_slot_bitmaps[part_id]) {
        if (used_slots_count) {
          disk_complete_req(part_id, &used_slots_count, used_slots, req_types,
                            req_bv_pages, trans_buf_addrs, trans_buf_sectors);
        }
        get_random_bytes(&delay_time, sizeof(delay_time));
        ndelay(delay_time & ((1 << mask) - 1));
        mask++;
        goto find_free_slot;
      }
      my_lock(part_id);
      if (!req_slot_bitmaps[part_id]) {
        no_free_slot = true;
      } else {
        old_bitmap = req_slot_bitmaps[part_id];
        new_bitmap = req_slot_bitmaps[part_id] =
            req_slot_bitmaps[part_id] & (req_slot_bitmaps[part_id] - 1);
      }
      my_unlock(part_id);
      if (no_free_slot) {
        disk_complete_req(part_id, &used_slots_count, used_slots, req_types,
                          req_bv_pages, trans_buf_addrs, trans_buf_sectors);
        goto find_free_slot;
      }
      local_req_slot = get_req_slot(old_bitmap, new_bitmap);
      global_req_slot = GET_GLOBAL_SLOT_ID(part_id, local_req_slot);
      used_slots[used_slots_count++] = local_req_slot;
      // issue req
      if (op_is_write(bio_op(bio))) { // disk write op
        memcpy(kbufs[global_req_slot], buffer,
               sectors << LOGICAL_SECTOR_SIZE_LOG2);
        *(kram + WRITE_CMD_TAG(global_req_slot)) =
            (sector_off << 3) | (sectors - 1);
        kunmap(bvec.bv_page);
        req_types[local_req_slot] = WRITE_TYPE;
      } else { // disk read op
        trans_buf_addrs[local_req_slot] = buffer;
        trans_buf_sectors[local_req_slot] = sectors;
        *(kram + READ_CMD_TAG(global_req_slot)) =
            (sector_off << 3) | (sectors - 1);
        req_bv_pages[local_req_slot] = bvec.bv_page;
        req_types[local_req_slot] = READ_TYPE;
      }

      sector_off += sectors;
    }
  }

  disk_complete_req(part_id, &used_slots_count, used_slots, req_types,
                    req_bv_pages, trans_buf_addrs, trans_buf_sectors);

  put_part_id(part_id);
  put_cpu();

  bio_endio(bio);

  return BLK_QC_T_NONE;
#undef REQ_QUEUE_SIZE
}

/*
 * These are the file operations that performed on the block device
 */
static struct block_device_operations disk_fops = {
    .owner = THIS_MODULE,
    .open = disk_open,
    .release = disk_close,
    .getgeo = disk_getgeo,
};

/*
 * This is the registration and initialization section of the block device
 * driver
 */
static int __init disk_init(void) {
  int ret;

  /* Set up our Device */
  if ((ret = diskdevice_init()) < 0) {
    return ret;
  }
  disk_dev.size = ret;

  /* Get Registered */
  disk_major = register_blkdev(disk_major, DEVICE_NAME);
  if (disk_major <= 0) {
    printk(KERN_ERR "Disk: Unable to get Major Number\n");
    diskdevice_cleanup();
    return -EBUSY;
  }

  /* Get a request queue (here queue is created) */
  spin_lock_init(&disk_dev.lock);
  disk_dev.disk_queue = blk_alloc_queue(GFP_KERNEL);
  if (!disk_dev.disk_queue) {
    printk(KERN_ERR "Disk: Unable to allocate the block queue\n");
    diskdevice_cleanup();
    return -ENOMEM;
  }
  blk_queue_make_request(disk_dev.disk_queue, disk_make_request);
  blk_queue_logical_block_size(disk_dev.disk_queue, LOGICAL_SECTOR_SIZE);
  blk_queue_physical_block_size(disk_dev.disk_queue, PHYSICAL_SECTOR_SIZE);

  if (disk_dev.disk_queue == NULL) {
    printk(KERN_ERR "Disk: blk_init_queue failure\n");
    unregister_blkdev(disk_major, DEVICE_NAME);
    diskdevice_cleanup();
    return -ENOMEM;
  }

  /*
   * Add the gendisk structure
   * By using this memory allocation is involved,
   * the minor number we need to pass bcz the device
   * will support this much partitions
   */
  disk_dev.disk = alloc_disk(MINOR_CNT);
  if (!disk_dev.disk) {
    printk(KERN_ERR "Disk: alloc_disk failure\n");
    blk_cleanup_queue(disk_dev.disk_queue);
    unregister_blkdev(disk_major, DEVICE_NAME);
    diskdevice_cleanup();
    return -ENOMEM;
  }

  /* Setting the major number */
  disk_dev.disk->major = disk_major;
  /* Setting the first mior number */
  disk_dev.disk->first_minor = FIRST_MINOR;
  /* Initializing the device operations */
  disk_dev.disk->fops = &disk_fops;
  /* Driver-specific own internal data */
  disk_dev.disk->private_data = &disk_dev;
  disk_dev.disk->queue = disk_dev.disk_queue;
  sprintf(disk_dev.disk->disk_name, DEVICE_NAME);
  /* Setting the capacity of the device in its gendisk structure */
  set_capacity(disk_dev.disk, disk_dev.size);

  /* Adding the disk to the system */
  add_disk(disk_dev.disk);
  /* Now the disk is "live" */
  printk(KERN_ERR "Disk: Block driver initialised (%d sectors; %lld bytes)\n",
         disk_dev.size, (u64)disk_dev.size * LOGICAL_SECTOR_SIZE);

  return 0;
}
/*
 * This is the unregistration and uninitialization section of the block
 * device driver
 */
static void __exit disk_cleanup(void) {
  del_gendisk(disk_dev.disk);
  put_disk(disk_dev.disk);
  blk_cleanup_queue(disk_dev.disk_queue);
  unregister_blkdev(disk_major, DEVICE_NAME);
  diskdevice_cleanup();
}

module_init(disk_init);
module_exit(disk_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Disk Block Driver for FPGA");
MODULE_ALIAS_BLOCKDEV_MAJOR(disk_major);
