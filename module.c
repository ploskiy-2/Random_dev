#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <asm/uaccess.h> 

#include "ff.h"         

#define DEVICE_NAME "Random_Numbers_Module"

#define SUCCESS 0
#define MAX_LENGTH 32

/* To forbidde multi-using */
enum {
        CDEV_NOT_USED = 0,
        CDEV_EXCLUSIVE_OPEN = 1,
};
static atomic_t cdev_status = ATOMIC_INIT(CDEV_NOT_USED);

static int major;
static struct class *cls;

static size_t k_length = 0;
static uint8_t crs_coefficients[MAX_LENGTH];
static uint8_t crs_elements[MAX_LENGTH];
static uint8_t crs_c = 0;

static ff_elem_t *ff_crs_c = NULL;
static ff_elem_t *ff_crs_coefficients[MAX_LENGTH];
static ff_elem_t *ff_crs_elements[MAX_LENGTH];

static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_write(struct file *filp, const char __user *buffer, size_t length, loff_t *offset);
static ssize_t my_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset);

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = my_open,
        .release = my_release,
        .write = my_write,
        .read = my_read,
};

module_param(k_length, ulong, 0);
MODULE_PARM_DESC(k_length, "Length of CRS");

module_param_array(crs_coefficients, byte, NULL, 0);
MODULE_PARM_DESC(crs_coefficients, "CRS coefficients: a0, a1, ..., a_{k-1}");

module_param_array(crs_elements, byte, NULL, 0);
MODULE_PARM_DESC(crs_elements, "Initial CRS elements: x0, x1, ..., x_{k-1}");

module_param(crs_c, byte, 0);
MODULE_PARM_DESC(crs_c, "CRS constant");




MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vadim Ploskarev");
MODULE_DESCRIPTION("Generator of random numbers.");