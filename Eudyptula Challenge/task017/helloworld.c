/*
 *
 *  THIS IS A MOUSE USB DRIVER TEST MODULE
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/string.h>

#include <linux/wait.h>
#include <linux/kthread.h>

MODULE_AUTHOR("HJH");
MODULE_DESCRIPTION("This is a miscdevice driver test module\n");
MODULE_LICENSE("GPL");

#define MISCDEVICE "eudyptula"
static char my_assigned_ID[] = "7c1caf2f50d1";
DECLARE_WAIT_QUEUE_HEAD(write_queue_head);
static struct task_struct *kthread_task = NULL;
DECLARE_WAITQUEUE(wee_wait,NULL);
/*
static long mytestmiscdeviceioctlfunction(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk(KERN_DEBUG"mytestmiscdeviceioctlfunction succeeded\n");
	return 0;
}

static ssize_t readfunction(struct file *filp, char __user *buf, size_t count, loff_t * f_ops)
{
	int len = sizeof(myassignedID);
	printk(KERN_DEBUG"readfunction succeeded\n");
	if(count < 0){
		return -1;
	}
	if(count < len){
		strncpy(buf,myassignedID,count);
		return count;
	}else{
		strncpy(buf,myassignedID,len);
		return len;
	}
	
}
* */
static ssize_t write_function(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
	
	if( strcmp(buf,my_assigned_ID) ){
		printk(KERN_DEBUG"writefunction: id is error,write failed\n");
		return -1;
	}
	printk(KERN_DEBUG"writefunction: succeeded\n");
}
static int open_function(struct inode *inodestruct, struct file *filp)
{
	printk(KERN_DEBUG"openfunction succeeded\n");
	return 0;
}
static int release_function(struct inode *inodestruct, struct file *filp)
{
	printk(KERN_DEBUG"releasefunction succeeded\n");
	return 0;
}

static struct file_operations mydeviceops = {
	.open = open_function,
	.write = write_function,
	.release = release_function,
};

static struct miscdevice mymiscdevice = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = MISCDEVICE,
	.fops = &mydeviceops,
	.mode = 0222,
};
int my_kthread_func(void *data)
{
	int err = 0;
	printk(KERN_DEBUG"This is a test kthread_func starting\n");
	err = wait_event_interruptible(write_queue_head,kthread_should_stop());
	if (err) 
		printk(KERN_DEBUG"Test kthread_func is ending\n");
	return 0;
}
static int __init test_misc_device_init(void)
{
	int result;
	
	result = misc_register(&mymiscdevice);
	if (result) {
		printk(KERN_ERR"miscdevice_register failed,errno is %d",result);
	}else{
		printk(KERN_DEBUG"miscdevice_register succeeded\n");
	}
	kthread_task = kthread_create(&my_kthread_func, NULL, MISCDEVICE, NULL);
	init_waitqueue_entry(&wee_wait, kthread_task);
	add_wait_queue(&write_queue_head, &wee_wait);
	wake_up_process(kthread_task);
	return result;
}

static void __exit test_misc_device_exit(void)
{
	kthread_stop(kthread_task);
	wake_up_interruptible(&write_queue_head);
	misc_deregister(&mymiscdevice);
	printk(KERN_DEBUG"miscdevice_deregister succeeded\n");
}



module_init(test_misc_device_init);
module_exit(test_misc_device_exit);
