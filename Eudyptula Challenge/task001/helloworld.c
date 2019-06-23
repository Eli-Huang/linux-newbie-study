#include <linux/module.h>

#include <linux/kernel.h>

#include <linux/init.h>

static int __init moduleInit(void)
{
	printk(KERN_DEBUG"debug:Hello,My Modules!\n");
	printk(KERN_ERR"error:Hello,My Modules!\n");
	return 0;
}
static void  __exit moduleExit(void)
{
	printk("<1>Goodbye,World!leaving the kernel space");


}
module_init(moduleInit);
module_exit(moduleExit);
MODULE_LICENSE("GPL");	
