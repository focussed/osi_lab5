/*
 * memory driver for lab5
 * 
 * Adopted from 'Writing Device Drivers in Linux'
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include "uaccess.h" /* copy_from/to_user */
#include <linux/cdev.h>
#include <linux/device.h>


MODULE_LICENSE("Dual BSD/GPL");

/* Necessary structures */
dev_t dev = 0;
static struct class *dev_class;
static struct cdev memory_cdev;

/* Declaration of memory.c functions */
int memory_open(struct inode *inode, struct file *filp);
int memory_release(struct inode *inode, struct file *filp);
ssize_t memory_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t memory_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void memory_exit(void);
int memory_init(void);

/* access functions */
struct file_operations fops = {
  read: memory_read,
  write: memory_write,
  open: memory_open,
  release: memory_release
};

/* Declaration of the init and exit functions */
module_init(memory_init);
module_exit(memory_exit);


/* Global variables of the driver */
/* Fixed Major number */
int memory_major = 160;

/* Buffer to store data */
char *memory_buffer;

/*
 * Code to run on insert of driver to kernel 
 * This should allocate free major device number 
 */
int memory_init(void) {

	int result;

	/* Registering device */
  	result = register_chrdev(memory_major, "memory", &fops);
  	if (result < 0) {
		printk("<1>memory: cannot obtain major number %d\n", memory_major);
    	return result;
  	}

	  /* Now initialise the buffer for the memory */

	  /* Allocating memory for the buffer */
	  memory_buffer = kmalloc(1, GFP_KERNEL); 
	  if (!memory_buffer) { 
	    result = -ENOMEM;
	    goto fail; 
	  } 

	  memset(memory_buffer, 0, 1);

        printk(KERN_INFO "memory driver: Device Driver Insert...Done!!!\n");
        return 0;

fail: 
    printk(KERN_INFO "memory driver: Device Driver insert FAILED no memory\n");
    memory_exit(); 
    return result;
}

/*
 * exit module called on removing the module from the kernel
 */
void memory_exit(void) {

  /* Freeing the major number */
  unregister_chrdev(memory_major, "memory");

  /* Freeing buffer memory */
  if (memory_buffer) {
    kfree(memory_buffer);
  }

  printk(KERN_INFO "memory driver: Device Driver Remove...Done!!!\n");

}

/*
 * Called on the open of the driver
 */
int memory_open(struct inode *inode, struct file *filp) {
  /* Success */
  printk(KERN_INFO "memory driver: Device Driver open() called\n");
  return 0;
}

/*
 * Called on the close of the driver
 */
int memory_release(struct inode *inode, struct file *filp) {
  /* Success */
  printk(KERN_INFO "memory driver: Device Driver release() called\n");
  return 0;
}

/*
 * Called on the read of the driver
 */
ssize_t memory_read(struct file *filp, char __user *buf, 
                    size_t count, loff_t *f_pos) { 

  printk(KERN_INFO "memory driver: Device Driver read() called\n");
  printk(KERN_INFO "memory driver: buffer = [%x]\n", memory_buffer[0]);
  /* Transfering data to user space */ 
  copy_to_user((char *)buf,memory_buffer,1);

  /* Changing reading position as best suits */ 
  if (*f_pos == 0) { 
    *f_pos+=1; 
    return 1; 
  } else { 
    return 0; 
  }
}

/*
 * Called on the write to the driver
 */
ssize_t memory_write( struct file *filp, const char *buf,
                      size_t count, loff_t *f_pos) {

  char *tmp;

  printk(KERN_INFO "memory driver: Device Driver write() called\n");

  tmp=(char *)buf+count-1;
  copy_from_user(memory_buffer,tmp,1);
  return 1;
}

/* End of driver */
