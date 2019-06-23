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


#include <linux/slab.h>

#include <linux/printk.h>
#include <linux/list.h>

#include <linux/delay.h>
#include <linux/spinlock.h>

#define MISCDEVICE "eudyptula"
#define LENGTH_NAME 19
static int wake_condtion_is_true = 0;
//static char my_assigned_ID[] = "7c1caf2f50d1";
DECLARE_WAIT_QUEUE_HEAD(write_queue_head);
static struct task_struct *kthread_task = NULL;
DECLARE_WAITQUEUE(wee_wait,NULL);
static LIST_HEAD(identity_head);
//static struct list_head *tail = &identity_head;
//static atomic_t count = ATOMIC_INIT(0);
static int times = 0;
static DEFINE_SPINLOCK(identity_lock);
unsigned long flag;

struct identity {
	char name[20];
	int id;
	bool busy;
	struct list_head list;
};

static struct identity *identity_find(int id)
{
	struct identity *pos = NULL;
	struct identity *end = NULL;
	/*struct identity *tmp = NULL; */

	list_for_each_entry_safe(pos, end, &identity_head, list) {
		if (pos->id == id)
			return pos;
	}
	/*list_for_each_safe(pos, end, &identity_head){
	   tmp = container_of(pos, struct identity,list);
	   if(pos->id == id)
	   return tmp;
	   } */
	return NULL;
}

static int identity_create(char *name, int id)
{
	int ret = 0;
	int len = 0;
	struct identity *tmp = NULL;
	if (name == NULL) {
		ret = -1;
		goto OUT;
	}
	tmp = (struct identity *)kmalloc(sizeof(struct identity), GFP_KERNEL);
	if (tmp == NULL) {
		ret = -1;
		pr_err("kmalloc has failed \n");
		goto OUT;
	}
	memset(tmp, 0, sizeof(struct identity));
	len = strlen(name);
	if (len > 0 && len <= 20) {
		memcpy(tmp->name, name, strlen(name));
		//snprintf(tmp->name, strlen(name), name); 输出会少一个字符Dave -> Dav
	} else {
		if (len > 20) {
			memcpy(tmp->name, name, 19);
		} else {
			pr_err("The name is null\n");
			kfree(tmp);
			ret = -1;
			goto OUT;
		}
	}
	if (!identity_find(id)) {
		tmp->id = id;
		printk("The id will be added. id is %d\n",id);
	} else {
		ret = -2;
		kfree(tmp);
		pr_err("The id had been added. id is %d\n",id);
		goto OUT;
	}
	tmp->busy = false;
	spin_lock_irqsave(&identity_lock,flag);
	list_add(&tmp->list, identity_head.prev);
	//tail = &tmp->list;
	spin_unlock_irqrestore(&identity_lock,flag);
OUT:
	return ret;
}

static void identity_destroy(int id)
{
	struct identity *tmp = NULL;
	tmp = identity_find(id);
	if (!tmp) {
		return;
	}
	list_del(&tmp->list);
	kfree(tmp);
}
struct identity *identity_get(void)
{
	struct identity *result = NULL;
	struct identity *tmp =NULL;
	tmp = list_first_entry_or_null(&identity_head, struct identity, list);
	if (tmp) {
		result = tmp;
		spin_lock_irqsave(&identity_lock,flag);
		list_del(&tmp->list);
		spin_unlock_irqrestore(&identity_lock,flag);
	}
	return result;
}


static ssize_t write_function(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
	char *kbuf = NULL;
	int retval = 0;

	kbuf = (char *) kmalloc(sizeof(char)*(LENGTH_NAME), GFP_KERNEL);
	memset(kbuf, '\0', sizeof(char)*(LENGTH_NAME));
	if (snprintf(kbuf, LENGTH_NAME+1, buf) <=0 ){
		pr_err("cp is error\n");
		retval = 1;
		goto out;
	}
	//atomic_inc(&count);
	times = times + 1;
	//if ((retval = (identity_create(kbuf, atomic_read(&count)))) != 0){
	if ((retval = (identity_create(kbuf, times))) != 0){
		//atomic_dec(&count);
		times = times - 1;
		pr_err("identity_create error,count is %d\n",times);
		goto out;
	}
	wake_condtion_is_true = 1;
	wake_up_interruptible(&write_queue_head);
	printk("writefunction: succeeded times = %d\n",times);
	
out:
	kfree(kbuf);
	return retval;	


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
	struct identity *tmp = NULL;
	
	while (!kthread_should_stop()) {
		err = wait_event_interruptible(write_queue_head,wake_condtion_is_true);
		if (!err) {
			printk("kthread has been wake\n");
			tmp = identity_get();
			wake_condtion_is_true = 0;
			if (tmp) {
				printk("identity_get result name is %s, id is %d\n",tmp->name, tmp->id);
				kfree(tmp);
			}
		}
		msleep(5*1000);
	}
	return err;
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
	init_wait_entry(&wee_wait, 1);
	add_wait_queue(&write_queue_head, &wee_wait);
	wake_up_process(kthread_task);
	return result;
}

static void __exit test_misc_device_exit(void)
{
	struct identity *tmp = NULL;	
	kthread_stop(kthread_task);
	while (!list_empty(&identity_head)) {
		tmp = list_last_entry(&identity_head, struct identity, list);
		list_del(&tmp->list);
		kfree(tmp);
	}
	pr_info("The linked-list test is end\n");
	
	misc_deregister(&mymiscdevice);
	printk(KERN_DEBUG"miscdevice_deregister succeeded\n");
	
	
}



module_init(test_misc_device_init);
module_exit(test_misc_device_exit);



MODULE_AUTHOR("HJH");
MODULE_DESCRIPTION("This is a miscdevice driver test module\n");
MODULE_LICENSE("GPL");
