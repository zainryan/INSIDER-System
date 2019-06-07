#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>

#define BUF_SIZE_LOG2 (22)

#ifndef VM_RESERVED
#define VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)
#endif

static int major = 88;
struct dentry *file;

struct mmap_info {
  char *data;
  int reference;
};

void mmap_open(struct vm_area_struct *vma) {
  struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
  info->reference++;
}

void mmap_close(struct vm_area_struct *vma) {
  struct mmap_info *info = (struct mmap_info *)vma->vm_private_data;
  info->reference--;
}

static int mmap_fault(struct vm_fault *vmf) {
  struct page *page;
  struct mmap_info *info;
  unsigned long offset;

  info = (struct mmap_info *)vmf->vma->vm_private_data;
  offset = ((unsigned long)vmf->address) - ((unsigned long)vmf->vma->vm_start);
  if (offset > (1 << BUF_SIZE_LOG2)) {
    printk(KERN_ERR "FPGA_DMA: Invalid address deference, offset = %lu",
           offset);
    return 0;
  }
  page = virt_to_page((info->data) + offset);
  get_page(page);
  vmf->page = page;

  return 0;
}

struct vm_operations_struct mmap_vm_ops = {
    .open = mmap_open,
    .close = mmap_close,
    .fault = mmap_fault,
};

int op_mmap(struct file *filp, struct vm_area_struct *vma) {
  vma->vm_ops = &mmap_vm_ops;
  vma->vm_flags |= VM_RESERVED;
  vma->vm_private_data = filp->private_data;
  mmap_open(vma);
  return 0;
}

int mmapfop_close(struct inode *inode, struct file *filp) {
  struct mmap_info *info = filp->private_data;

  free_page((unsigned long)info->data);
  kfree(info);
  filp->private_data = NULL;
  return 0;
}

int mmapfop_open(struct inode *inode, struct file *filp) {
  struct mmap_info *info = kmalloc(sizeof(struct mmap_info), GFP_KERNEL);
  unsigned long long *phy_addr;
  info->data = (char *)__get_free_pages(GFP_KERNEL, BUF_SIZE_LOG2 - 12);
  memset(info->data, 0, 1 << BUF_SIZE_LOG2);
  phy_addr = (unsigned long long *)info->data;
  *phy_addr = virt_to_phys(info->data);
  filp->private_data = info;
  return 0;
}

static const struct file_operations mmap_fops = {
    .open = mmapfop_open,
    .release = mmapfop_close,
    .mmap = op_mmap,
};

static int __init fpga_dma_module_init(void) {
  register_chrdev(major, "fpga_dma", &mmap_fops);
  return 0;
}

static void __exit fpga_dma_module_exit(void) {
  unregister_chrdev(major, "fpga dma");
}

module_init(fpga_dma_module_init);
module_exit(fpga_dma_module_exit);
MODULE_LICENSE("GPL");
