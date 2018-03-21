#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>



struct list_head g_list;
struct class *wuyu_class = NULL;
struct device *wuyu_dev = NULL;
typedef struct test_struct {
	int id;
	char name[20];
	struct list_head list;
}test_struct_t;

struct test_struct1
{
	int id;
	char name[20];
	dev_t devno;
	int val;
	struct device *dev;
	struct cdev *cdev;
};

struct test_struct1 *test_struct1_p = NULL;
//ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
//int (*release) (struct inode *, struct file *);


void my_dbg(const char *str)
{
	printk(KERN_ALERT"%s\n", str);
}

void my_printk(const char *fmt, ...)
{
	va_list vlist;
	static char buf[100];
	int ret = 0;
	memset(buf, 0, sizeof(buf));
	va_start(vlist, fmt);
	ret = vsnprintf (buf, sizeof(buf), fmt, vlist);
	va_end (vlist);

	printk(KERN_ALERT"%s\n", buf);
	
}
#define my_vdbg(fmt, ...) \
	my_printk("[%s][%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

int test1_open(struct inode * node, struct file *filp)
{
	my_dbg("wuyu test1_open is called\n");
	return 0;
}

struct file_operations fops = {
	.open = test1_open,
};
void create_test_struct(const char *name, int id) {
	test_struct_t *test = kzalloc(sizeof(test_struct_t),GFP_KERNEL);
	strcpy(test->name, name);
	test->id = id;
	list_add_tail(&test->list, &g_list);
}

ssize_t hello_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	int ret = 0;
	my_dbg("hello show is called\n");
	my_vdbg("%s, %d\n", "wuyu",123455);
	my_printk("wuyu", "%d\n", 123);
	ret = snprintf(buf, 4096, "%d\n",test_struct1_p->val);
	printk(KERN_ALERT"hello_show ret=%d\n", ret);
	return ret;
	//return strcpy(buf, "hello_show_called");
}
ssize_t hello_store(struct device *dev, struct device_attribute *attr,
		 const char *buf, size_t count)
{
	
	int val = 0;
	test_struct1_p->val = simple_strtol (buf, NULL, 10);
	printk(KERN_ALERT"hello store is called, count = %d, val=%d\n", count, test_struct1_p->val);
	return count;
}
		 
static DEVICE_ATTR (hello, S_IWUSR | S_IRUGO, hello_show, hello_store);

static int hello_init(void)
{
	int i =0;
	test_struct_t *p = NULL;
	int ret = 0;
	printk(KERN_ALERT"hello init called\n");
	INIT_LIST_HEAD(&g_list);
	test_struct1_p = kzalloc(sizeof(struct test_struct1), GFP_KERNEL);
	if(NULL == test_struct1_p) {
		my_dbg("test_struct1_p is NULL\n");
		goto exit;
	}
	test_struct1_p->cdev = cdev_alloc();
	ret = alloc_chrdev_region(&test_struct1_p->devno, 0, 1, "wuyu_dev_test1");
	if(ret < 0) {
		my_dbg("alloc_chrdev_region failed\n");
		goto exit;
	}
	printk(KERN_ALERT"major is:%x, minor is %x\n", MAJOR(test_struct1_p->devno),MINOR(test_struct1_p->devno));

	cdev_init(test_struct1_p->cdev, &fops);
	ret = cdev_add(test_struct1_p->cdev, test_struct1_p->devno, 1);
	if(ret < 0) {
		my_dbg("cdev_add failed\n");
		goto exit;
	}

	create_test_struct("test0", 0);
	create_test_struct("test1", 1);
	create_test_struct("test2", 2);
	create_test_struct("test3", 3);
	create_test_struct("test4", 4);

	list_for_each_entry(p, &g_list, list) {
		printk(KERN_ALERT"%s,%d\n", p->name, p->id);
		
	}

	wuyu_class = class_create(THIS_MODULE, "wuyu_class");
	if(NULL == wuyu_class) {
		my_dbg("create wuyu_class failed\n");
		goto exit;
	}
	test_struct1_p->dev = device_create(wuyu_class, NULL, test_struct1_p->devno,  test_struct1_p, "wuyu_device");
	if(NULL == test_struct1_p->dev) {
		my_dbg("device_create  failed\n");
		goto exit;
	}

	ret = device_create_file (test_struct1_p->dev , &dev_attr_hello);
	if(ret != 0) {
		my_dbg("device_create_file  failed\n");
		goto exit;
	}

exit:
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT"hello exit called\n");
	test_struct_t *p = NULL;
	test_struct_t *q = NULL;
	list_for_each_entry_safe(p, q, &g_list, list) {
		if(NULL != p) {
			printk(KERN_ALERT"kfree %s,%d\n", p->name, p->id);
			kfree(p);
		}
	}
	cdev_del(test_struct1_p->cdev);
	unregister_chrdev_region (test_struct1_p->devno, 1);
	kfree(test_struct1_p);

	device_del (test_struct1_p->dev);

	class_destroy (wuyu_class);
	return;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");

