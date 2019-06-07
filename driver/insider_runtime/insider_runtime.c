#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/path.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

struct ioctl_req {
  char *real_file_path_ptr;
  uint32_t real_file_path_len;
  int8_t is_lock;
};

#define IOCTL_CMD _IOW('a', 'a', struct ioctl_req *)

dev_t dev = 0;
static struct class *dev_class;
static struct cdev insider_runtime_cdev;

static int __init insider_runtime_driver_init(void);
static void __exit insider_runtime_driver_exit(void);
static int insider_runtime_open(struct inode *inode, struct file *file);
static int insider_runtime_release(struct inode *inode, struct file *file);
static long insider_runtime_ioctl(struct file *file, unsigned int cmd,
                                  unsigned long arg);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = insider_runtime_open,
    .unlocked_ioctl = insider_runtime_ioctl,
    .release = insider_runtime_release,
};

static int insider_runtime_open(struct inode *inode, struct file *file) {
  return 0;
}

static int insider_runtime_release(struct inode *inode, struct file *file) {
  return 0;
}

static long insider_runtime_ioctl(struct file *file, unsigned int cmd,
                                  unsigned long arg) {
  struct ioctl_req req;
  char *real_file_path;
  struct inode *inode;
  struct path path;

  switch (cmd) {
  case IOCTL_CMD:
    if (copy_from_user(&req, (struct ioctl_req *)arg, sizeof(struct ioctl_req)) != 0) {
      printk(KERN_ERR "INSIDER runtime deiver: Failed on copy_from_user!");
      break;
    }
    real_file_path = (char *)vmalloc(req.real_file_path_len + 1);
    if (copy_from_user(real_file_path, req.real_file_path_ptr,
                       req.real_file_path_len + 1) != 0) {
      printk(KERN_ERR "INSIDER runtime deiver: Failed on copy_from_user!");
      break;
    }
    kern_path(real_file_path, LOOKUP_FOLLOW, &path);
    inode = path.dentry->d_inode;
    if (req.is_lock) {
      inode->i_flags |= S_APPEND;
    } else {
      inode->i_flags &= ~S_APPEND;
    }
    path_put(&path);
    if (real_file_path) {
      vfree(real_file_path);
    }
    break;
  default:
    printk(KERN_ERR "INSIDER runtime driver: Unsupported ioctl operation!");
    break;
  }
  return 0;
}

static int __init insider_runtime_driver_init(void) {
  /*Allocating Major number*/
  if ((alloc_chrdev_region(&dev, 0, 1, "insider_runtime_Dev")) < 0) {
    printk(KERN_INFO "INSIDER runtime driver: Cannot allocate major number\n");
    return -1;
  }

  /*Creating cdev structure*/
  cdev_init(&insider_runtime_cdev, &fops);

  /*Adding character device to the system*/
  if ((cdev_add(&insider_runtime_cdev, dev, 1)) < 0) {
    printk(KERN_INFO
           "INSIDER runtime driver: Cannot add the device to the system\n");
    goto r_class;
  }

  /*Creating struct class*/
  if ((dev_class = class_create(THIS_MODULE, "insider_runtime_class")) ==
      NULL) {
    printk(KERN_INFO
           "INSIDER runtime driver: Cannot create the struct class\n");
    goto r_class;
  }

  /*Creating device*/
  if ((device_create(dev_class, NULL, dev, NULL, "insider_runtime")) == NULL) {
    printk(KERN_INFO "INSIDER runtime driver: Cannot create the Device 1\n");
    goto r_device;
  }
  return 0;

r_device:
  class_destroy(dev_class);
r_class:
  unregister_chrdev_region(dev, 1);
  return -1;
}

void __exit insider_runtime_driver_exit(void) {
  device_destroy(dev_class, dev);
  class_destroy(dev_class);
  cdev_del(&insider_runtime_cdev);
  unregister_chrdev_region(dev, 1);
}

module_init(insider_runtime_driver_init);
module_exit(insider_runtime_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Insider runtime driver.");
