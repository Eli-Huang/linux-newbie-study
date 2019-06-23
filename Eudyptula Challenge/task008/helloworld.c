/*
 *
 *  THIS IS A DEBUGFS TEST MODULE
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
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h> //copy_to_user
#include <linux/slab.h> //kmalloc
#include <linux/spinlock.h>

MODULE_AUTHOR("HJH");
MODULE_DESCRIPTION("This is a debugfs test module\n");
MODULE_LICENSE("GPL");

#define MISCDEVICE "eudyptula"
static char default_content[] = "defaults";
static const char my_assigned_id[] = "7c1caf2f50d1";
static struct dentry *my_debugfs_subdirectory;
static void *test_foo;
static DEFINE_SPINLOCK(foo_write_read_lock);
static long ioctl_to_debugfs(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk(KERN_DEBUG"ioctl_to_debugfs succeeded\n");
	return 0;
}
static ssize_t read_from_debugfs_file_id(struct file *filp, char __user *buf, size_t count, loff_t *f_ops)
{
	char *tmp = (char *) filp->private_data;
	int len = strlen(tmp);
	int result = 0;
	
	printk(KERN_DEBUG"read_from_debugfs_file_id succeeded\n");
	if (count < 0)
		return -1;
	if (*f_ops > len - 1)
		return 0;
	if ((*f_ops+count) <= len) {
		copy_to_user(buf, tmp + (*f_ops), count);
		result = count;
	} else {
		copy_to_user(buf, tmp + (*f_ops), len - (*f_ops));
		result = len - (*f_ops);
	}
	return result;
}
static ssize_t read_from_debugfs_file_foo(struct file *filp, char __user *buf, size_t count, loff_t *f_ops)
{
	char *tmp = (char *) filp->private_data;
	int len = 0;
	int result = 0;
	
	printk(KERN_DEBUG"read_from_debugfs_file_id succeeded\n");
	if (count < 0)
		return -1;
	spin_lock(&foo_write_read_lock);
	len = strlen(tmp);
	if (*f_ops > len - 1)
		return 0;
	if ((*f_ops+count) <= len) {
		copy_to_user(buf, tmp + (*f_ops), count);
		spin_unlock(&foo_write_read_lock);
		result = count;
	} else {
		copy_to_user(buf, tmp + (*f_ops), len - (*f_ops));
		spin_unlock(&foo_write_read_lock);
		result = len - (*f_ops);
	}
	return result;
}
static ssize_t read_from_debugfs_file_jiffies(struct file *filp, char __user *buf, size_t count, loff_t *f_ops)
{
	int len = sizeof(unsigned long);
	unsigned long *tmp = &jiffies;
	int result = 0;
	
	printk(KERN_DEBUG"read_from_debugfs_file_jiffies succeeded\n");
	if (count < 0)
		return -1;
	if (*f_ops > len - 1)
		return 0;
	if ((*f_ops+count) <= len) {
		copy_to_user(buf, tmp + (*f_ops), count);
		result = count;
	} else {
		copy_to_user(buf, tmp + (*f_ops), len - (*f_ops));
		result = len - (*f_ops);
	}
	return result;
}
static ssize_t write_to_debugfs_file_id(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
	int retval = 0;
	char *tmp = filp->private_data;
	char *buf_copy = kmalloc(sizeof(char)*32, GFP_KERNEL);
	char *my_assigned_id_ptr = my_assigned_id;
	
	if (copy_from_user(buf_copy, buf, count)) {
		printk(KERN_DEBUG"debugfs_file_id: failed when copy\n");
		retval = -2;
		goto out;
	}
	if (strcmp(buf_copy, my_assigned_id_ptr)) {
		printk(KERN_DEBUG"write_to_debugfs_file_id: id is error,write failed:%s vs %s\n", buf_copy, my_assigned_id_ptr);
		retval = -1;
		goto out;
	}
	if (copy_from_user(tmp, buf, count)) {
		printk(KERN_DEBUG"write_to_debugfs_file_id: failed\n");
		retval = -3;
		goto out;
	} else {
		printk(KERN_DEBUG"write_to_debugfs_file_id: succeeded\n");
	}
	printk(KERN_DEBUG"3: my_assigned_id is %s,0x%x\n", default_content, &default_content[11]);
out:
	kfree(buf_copy);
	return retval;
}
static ssize_t write_to_debugfs_file_foo(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
	int retval = 0;
	char *tmp = filp->private_data;
	
	spin_lock(&foo_write_read_lock);
	if (copy_from_user(tmp, buf, count)) {
		printk(KERN_DEBUG"write_to_debugfs_file_id: failed\n");
		retval = -3;
		spin_unlock(&foo_write_read_lock);
		goto out;
	} else {
		spin_unlock(&foo_write_read_lock);
		printk(KERN_DEBUG"write_to_debugfs_file_id: succeeded\n");
	}
	printk(KERN_DEBUG"3: my_assigned_id is %s,0x%x\n", tmp, tmp);
out:
	return retval;
}
static int open_debugfs_file(struct inode *inodestruct, struct file *filp)
{
	filp->private_data = inodestruct->i_private;
	printk(KERN_DEBUG"open_debugfs_file succeeded\n");
	return 0;
}
static int release_debugfs_file(struct inode *inodestruct, struct file *filp)
{
	printk(KERN_DEBUG"release_debugfs_file succeeded\n");
	return 0;
}
static struct file_operations the_id_file_ops = {
	.open = open_debugfs_file,
	.write = write_to_debugfs_file_id,
	.read = read_from_debugfs_file_id,
	.release = release_debugfs_file,
	.unlocked_ioctl = ioctl_to_debugfs,
};
static struct file_operations the_jiffies_file_ops = {
	.open = open_debugfs_file,
	.read = read_from_debugfs_file_jiffies,
	.release = release_debugfs_file,
};
static struct file_operations the_foo_file_ops = {
	.open = open_debugfs_file,
	.write = write_to_debugfs_file_foo,
	.read = read_from_debugfs_file_foo,
	.release = release_debugfs_file,
	.unlocked_ioctl = ioctl_to_debugfs,
};
static int __init debugfs_test_init(void)
{
	test_foo = kmalloc(4096, GFP_KERNEL);
	my_debugfs_subdirectory = debugfs_create_dir(MISCDEVICE, NULL);
	debugfs_create_file("id", 0766, my_debugfs_subdirectory, default_content, &the_id_file_ops);
	debugfs_create_file("jiffies", 0444, my_debugfs_subdirectory, NULL, &the_jiffies_file_ops);
	debugfs_create_file("foo", 0744, my_debugfs_subdirectory, test_foo, &the_foo_file_ops);
	debugfs_create_u32("test-jiffies", 0444, my_debugfs_subdirectory, (u32 *) &jiffies);
	printk(KERN_DEBUG"debug:Hello,My Modules!\n");
	return 0;
}
static void  __exit debugfs_test_exit(void)
{
	debugfs_remove_recursive(my_debugfs_subdirectory);
	kfree(test_foo);
	printk(KERN_DEBUG"<1>Goodbye,World!leaving the kernel space");
}
module_init(debugfs_test_init);
module_exit(debugfs_test_exit);
MODULE_LICENSE("GPL");
