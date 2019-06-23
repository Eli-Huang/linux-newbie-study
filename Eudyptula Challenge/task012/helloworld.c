/*
 *
 *  THIS IS A KERNEL LIST TEST MODULE
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
#include <linux/slab.h>
#include <linux/string.h>

#include <linux/printk.h>
#include <linux/list.h>

MODULE_AUTHOR("HJH");
MODULE_DESCRIPTION("This is a kernel linked-list test module\n");
MODULE_LICENSE("GPL");

#define MISCDEVICE "eudyptula"

struct identity {
	char name[20];
	int id;
	bool busy;
	struct list_head list;
};
static LIST_HEAD(identity_head);
static struct list_head *tail = &identity_head;

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
	} else {
		ret = -2;
		kfree(tmp);
		pr_err("The id had been added.\n");
		goto OUT;
	}
	tmp->busy = false;
	list_add(&tmp->list, tail);
	tail = &tmp->list;
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

static int __init list_test_init(void)
{
	struct identity *temp;
	identity_create(MISCDEVICE, 5);
	identity_create(NULL, 12);
	identity_create("Alice", 1);
	identity_create("Bob", 2);
	identity_create("Bob", 2);
	identity_create("Dave", 3);
	identity_create("Gena", 10);

	temp = identity_find(3);
	pr_info("id 3 = %s\n", temp->name);

	temp = identity_find(42);
	if (temp == NULL)
		pr_err("id 42 not found\n");

	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);
	return 0;
}

static void __exit list_test_exit(void)
{
	struct identity *tmp = NULL;
	while (!list_empty(&identity_head)) {
		tmp = list_last_entry(&identity_head, struct identity, list);
		list_del(&tmp->list);
		kfree(tmp);
	}
	pr_info("The linked-list test is end\n");
}

module_init(list_test_init);
module_exit(list_test_exit);
MODULE_LICENSE("GPL");
