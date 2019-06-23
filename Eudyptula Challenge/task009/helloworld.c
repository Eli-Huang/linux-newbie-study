/*
 *
 *  THIS IS A SYSFS TEST MODULE
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
#include <linux/uaccess.h> //copy_to_user
#include <linux/slab.h> //kmalloc
#include <linux/spinlock.h>

#include <linux/sysfs.h>
#include <linux/kobject.h>

MODULE_AUTHOR("HJH");
MODULE_DESCRIPTION("This is a sysfs test module\n");
MODULE_LICENSE("GPL");

#define MISCDEVICE "eudyptula"
static char default_content[32] = "defaults";
static const char my_assigned_id[] = "7c1caf2f50d1";
static struct kobject *eudyptula_kobj;
static void *test_foo;
static DEFINE_SPINLOCK(foo_write_read_lock);
static ssize_t show_id(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	char *tmp = &default_content[0];
	int len = sprintf(buf, "%s\n", tmp);
	printk(KERN_DEBUG"show_id succeeded,%s,%d,%s\n",buf,len,tmp);
	return len;
	
}
static ssize_t show_jiffies(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{	
	int len = sprintf(buf, "%ld\n", jiffies);
	printk(KERN_DEBUG"show_jiffies succeeded\n");
	return len;
	
}
static ssize_t show_foo(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	char *tmp = (char *) test_foo;
	int len = 0;
	
	spin_lock(&foo_write_read_lock);
	len = sprintf(buf, "%s\n", tmp);
	spin_unlock(&foo_write_read_lock);
	printk(KERN_DEBUG"show_foo succeeded\n");
	return len;
	
}
static ssize_t store_id(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int retval = 0;
	char *tmp = default_content;
	char *buf_copy = kmalloc(sizeof(char)*32, GFP_KERNEL);
	char *my_assigned_id_ptr = my_assigned_id;
	
	if (snprintf(buf_copy, strlen(my_assigned_id_ptr)+1, buf) <= 0) {
		printk(KERN_DEBUG"store_id: failed when copy\n");
		retval = -2;
		goto out;
	}
	if (strcmp(buf_copy, my_assigned_id_ptr)) {
		printk(KERN_DEBUG"store_id: id is error,write failed:%s vs %s\n", buf_copy, my_assigned_id_ptr);
		retval = -1;
		goto out;
	}
	if ((retval = snprintf(tmp, strlen(my_assigned_id_ptr)+1, buf)) <= 0) {
		printk(KERN_DEBUG"store_id: failed\n");
		goto out;
	} else {
		printk(KERN_DEBUG"store_id: succeeded\n");
	}
out:
	kfree(buf_copy);
	return retval;
}
static ssize_t store_foo(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int retval = 0;
	char *tmp = (char *) test_foo;
	
	spin_lock(&foo_write_read_lock);
	if ((retval = sprintf(tmp, "%s", buf)) <= 0) {
		spin_unlock(&foo_write_read_lock);
		printk(KERN_DEBUG"store_foo: failed,retval = %d\n",retval);
		retval = -3;
		goto out;
	} else {
		spin_unlock(&foo_write_read_lock);
		printk(KERN_DEBUG"store_foo: succeeded\n");
	}
out:
	return retval;
}

static struct kobj_attribute foo_attribute = __ATTR(foo, 0644, show_foo, store_foo);
static struct kobj_attribute jiffies_attribute = __ATTR(jiffies, 0444, show_jiffies, NULL);
static struct kobj_attribute id_attribute = __ATTR(id, 0774, show_id, store_id);

static struct attribute *attrs_array[] = {
	&foo_attribute.attr,
	&id_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};
static struct attribute_group attrs_group = {
	.attrs = attrs_array,
};
static int __init sysfs_test_init(void)
{
	test_foo = kmalloc(4096, GFP_KERNEL);
	
	eudyptula_kobj = kobject_create_and_add(MISCDEVICE, kernel_kobj);
	sysfs_create_file(eudyptula_kobj, &jiffies_attribute.attr);
	sysfs_create_group(eudyptula_kobj, &attrs_group);
	
	printk(KERN_DEBUG"sysfs:Hello,My Modules!\n");	
	return 0;
}
static void  __exit sysfs_test_exit(void)
{
	
	kobject_put(eudyptula_kobj);
	kfree(test_foo);
	printk(KERN_DEBUG"<1>Goodbye,World!leaving the kernel space");
}
module_init(sysfs_test_init);
module_exit(sysfs_test_exit);
MODULE_LICENSE("GPL");
