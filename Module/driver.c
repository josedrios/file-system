/**************************************************************
* Class::  CSC-415-01 Spring 2024
* Name:: Jose Rios
* Student ID:: 921160471
* GitHub-Name:: Colorbomb1
* Project:: Assignment 6 – Device Driver
*
* File:: driver.c
*
* Description:: This file contains functions that make the
* device driver functional, it includes opening, closing,
* writing, reading and my ioctl function
*
**************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define MY_MAJOR 415
#define MY_MINOR 0
#define DEVICE_NAME "driver"

int major, minor;
char *kernel_buffer;

struct cdev my_cdev;
int actual_rx_size = 0;

MODULE_AUTHOR("Jose Rios");
MODULE_DESCRIPTION("A writing program");
MODULE_LICENSE("GPL");

struct myds{
    int count;
    char* buffer;
} myds;

static int myOpen(struct inode * inode, struct file * fs){
    struct myds * ds;
    ds = vmalloc(sizeof(struct myds));


    if(ds == 0){
        printk(KERN_ERR "Can not vmalloc.\n");
        return -1;
    }

    ds->count = 0;
    fs->private_data = ds;
    ds->buffer = vmalloc(512);
    printk(KERN_INFO "The N Queens Driver has been opened!");
    return 0;
}

static int myClose(struct inode * inode, struct file * fs){
    struct myds * ds;
    ds = (struct myds *) fs->private_data;

    vfree(ds->buffer);
    vfree(ds);
    
    return 0;
}

static ssize_t myWrite (struct file *fs, const char __user * buf, size_t hsize, loff_t * off){
    char* input = NULL;
    
    size_t newlen = 512;
    
    struct myds* ds;
    ds = (struct myds *) fs->private_data;

    if (hsize > newlen) {
        hsize = newlen;
    }

    input = vmalloc(hsize);
    if(!input){
        return -1;
    }

    if(copy_from_user(input, buf, hsize)){
        vfree(input);
        return hsize;
    }

    int i = 0;
    int num = 0;

    // converting inputted grade percentage string to a int
    while (input[i] != '\0') {
        num = num * 10 + (input[i] - '0');
        i++;
    }

    // determining the grade recieved
    if(num > 97){
        input = "You received an A+";
    }else if( num >= 93){
        input = "You received an A";
    }else if( num >= 90){
        input = "You received an A-";
    }else if( num >= 87){
        input = "You received an B+";
    }else if( num >= 83){
        input = "You received an B";
    }else if( num >= 80){
        input = "You received an B-";
    }else if( num >= 77){
        input = "You received an C+";
    }else if( num >= 73){
        input = "You received an C";
    }else if( num >= 70){
        input = "You received an C-";
    }else if( num >= 67){
        input = "You received an D+";
    }else if( num >= 63){
        input = "You received an D";
    }else if( num >= 60){
        input = "You received an D-";
    }else{
        input = "You received an F";
    }

    // copying it to the buffer in device
    strncpy(ds->buffer, input, hsize);

    // freeing all unneeded space
    vfree(input);

    ds->count = ds->count + 1;

    return hsize;
}

static ssize_t myRead (struct file *fs, char __user * buf, size_t hsize, loff_t * off){    
    size_t datalen;
    struct myds* ds;
    ds = (struct myds *) fs->private_data;

    datalen = strlen(ds->buffer);
    if (hsize > datalen) {
        hsize = datalen;
    }
    if(copy_to_user(buf, ds->buffer, hsize)){
        return -EFAULT;
    }

    *off += hsize;

    return hsize;
}

static long myIoCtl(struct file *fs, unsigned int command, unsigned long data){
    int * count;
    struct myds * ds;
    ds = (struct myds *) fs->private_data;

    if(command != 3){
        printk(KERN_ERR "Failed in myioctl\n");
        return -1;
    }
    count = (int *) data;
    int bytesNotCopied = copy_to_user(count, &(ds->count), sizeof(int));

    return bytesNotCopied;
}

struct file_operations fops = {
    .open = myOpen,
    .release = myClose,
    .write = myWrite,
    .read = myRead,
    .unlocked_ioctl = myIoCtl,
    .owner = THIS_MODULE,
};

int init_module(void){

    int result, registers;
    dev_t devno;
    devno = MKDEV(MY_MAJOR, MY_MINOR);
    registers = register_chrdev_region(devno, 1, DEVICE_NAME);
    printk(KERN_INFO "Register chardev suceeded 1: %d\n", registers);
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    result = cdev_add(&my_cdev, devno, 1);
    printk(KERN_INFO "Dev add chardev suceeded 2: %d\n", result);

    printk(KERN_INFO "Welcome - N Queens Driver is loaded.\n");

    if(result < 0){
        printk(KERN_ERR "Register chardev failed: %d\n", result);
    }

    return result;
}

void cleanup_module(void){
    dev_t devno;
    devno = MKDEV(MY_MAJOR, MY_MINOR);
    unregister_chrdev_region(devno, 1);
    cdev_del(&my_cdev);

    printk(KERN_INFO "The N Queen Driver is now closing\n");
}



