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

MODULE_AUTHOR("HJH");
MODULE_DESCRIPTION("This is a miscdevice driver test module\n");
MODULE_LICENSE("GPL");

#define MISCDEVICE "eudyptula"
static char myassignedID[] = "7c1caf2f50d1";
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
static ssize_t writefunction(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
	
	if( strcmp(buf,myassignedID) ){
		printk(KERN_DEBUG"writefunction: id is error,write failed\n");
		return -1;
	}
	printk(KERN_DEBUG"writefunction: succeeded\n");
}
static int openfunction(struct inode *inodestruct, struct file *filp)
{
	printk(KERN_DEBUG"openfunction succeeded\n");
	return 0;
}
static int releasefunction(struct inode *inodestruct, struct file *filp)
{
	printk(KERN_DEBUG"releasefunction succeeded\n");
	return 0;
}

static struct file_operations mydeviceops = {
	.open = openfunction,
	.write = writefunction,
	.read = readfunction,
	.release = releasefunction,
	.unlocked_ioctl = mytestmiscdeviceioctlfunction,
};

static struct miscdevice mymiscdevice = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = MISCDEVICE,
	.fops = &mydeviceops,
};

static int __init test_miscdevice_init(void)
{
	int result;
	result = misc_register(&mymiscdevice);
	if (result) {
		printk(KERN_ERR"miscdevice_register failed,errno is %d",result);
	}else{
		printk(KERN_DEBUG"miscdevice_register succeeded\n");
	}
	return result;
}

static void __exit test_miscdevice_exit(void)
{
	misc_deregister(&mymiscdevice);
	printk(KERN_DEBUG"miscdevice_deregister succeeded\n");
}



module_init(test_miscdevice_init);
module_exit(test_miscdevice_exit);
