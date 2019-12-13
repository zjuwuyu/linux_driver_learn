#include"mpu6050bsp.h"


int HELLO_MAJOR = 0;
int HELLO_MINOR = 0;
int NUMBER_OF_DEVICES = 2;
 
struct class *my_class;
struct cdev cdev;
dev_t devno;
/*************************************************************************************/ 


#define DRIVER_NAME "mpu6050"

int nrf_open(struct inode *inode,struct file *filp)
{
	u8 reg;
	reg=InitMPU6050();

	printk("mpu6050:%d\n",reg);
	return nonseekable_open(inode,filp);
}

long nrf_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		
		default:
			return -2;
	}
	return 0;
}
int nrf_read(struct file *filp, char *buffer,size_t count, loff_t *ppos)
{
	mpu_get_date();
	return copy_to_user(buffer, mpu_Date, 14);
}
int nrf_write(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{	
	return 0;
}

struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.read = nrf_read,
	.write = nrf_write,
	.open 	= nrf_open,
	.unlocked_ioctl = nrf_ioctl,
};


/**************************************************************************************/
static int __init hello_init (void)
{
    int result;
    devno = MKDEV(HELLO_MAJOR, HELLO_MINOR);
    if (HELLO_MAJOR)
        result = register_chrdev_region(devno, 2, "mpu6050");
    else
    {
        result = alloc_chrdev_region(&devno, 0, 2, "mpu6050");
        HELLO_MAJOR = MAJOR(devno);
    }  
    printk("MAJOR IS %d\n",HELLO_MAJOR);
    my_class = class_create(THIS_MODULE,"mpu6050_class");  //类名为
    if(IS_ERR(my_class)) 
    {
        printk("Err: failed in creating class.\n");
        return -1; 
    }
    device_create(my_class,NULL,devno,NULL,"mpu6050");      //设备名为mpu6050
    if (result<0) 
    {
        printk (KERN_WARNING "hello: can't get major number %d\n", HELLO_MAJOR);
        return result;
    }
 
    cdev_init(&cdev, &hello_fops);
    cdev.owner = THIS_MODULE;
    cdev_add(&cdev, devno, NUMBER_OF_DEVICES);
    printk (KERN_INFO "mpu6050 driver Registered\n");
    return 0;
}
 
static void __exit hello_exit (void)
{
    cdev_del (&cdev);
    device_destroy(my_class, devno);         //delete device node under /dev//必须先删除设备，再删除class类
    class_destroy(my_class);                 //delete class created by us
    unregister_chrdev_region (devno,NUMBER_OF_DEVICES);
    printk (KERN_INFO "char driver cleaned up\n");
}
 
module_init (hello_init);
module_exit (hello_exit);
 
MODULE_LICENSE ("GPL");
