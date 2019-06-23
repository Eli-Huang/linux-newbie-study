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
#include <linux/usb.h>

MODULE_AUTHOR("HJH");
MODULE_DESCRIPTION("This is a usb driver test module\n");
MODULE_LICENSE("GPL");

/*usb mouse0 device info*/
static struct usb_device_id test_usb_table [] = {
	 { USB_DEVICE_INFO(0x00,0x00,0x00) },
	 { }
};
MODULE_DEVICE_TABLE(usb,test_usb_table);
static struct usb_driver test_usb_driver = {
	.name = "helloworld",
	.id_table = test_usb_table,
};

static int __init test_usb_init(void)
{
	int result;
	result = usb_register(&test_usb_driver);
	if (result) {
		printk(KERN_ERR"usb_register failed,errno is %d",result);
	}else{
		printk(KERN_DEBUG"usb_register succeeded\n");
	}
	return result;
}

static void __exit test_usb_exit(void)
{
	usb_deregister(&test_usb_driver);
	printk(KERN_DEBUG"usb_deregister succeeded\n");
}


module_init(test_usb_init);
module_exit(test_usb_exit);
