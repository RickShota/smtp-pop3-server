#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/hardware.h>
#include <linux/ioport.h>
#include <asm/io.h>

#include <asm/delay.h>
#include <asm/uaccess.h>


#define DEVICE_NAME	"emdoor_8led"
#define SEG_LED  	0x10500000             ;虚拟地址--物理地址为0x00500000
static char Led_Buff=0x0;

unsigned long * Led_Address;

static void Updateled(void)
{
	writew(Led_Buff,Led_Address);
	return;
	
}

static ssize_t Emdoor_8Led_write(struct file *file, const char *buffer, size_t count, loff_t * ppos)
{
	if(count!=1)
	{
	printk(KERN_EMERG "the count of input is not one!!");
	return 0;
	}

	copy_from_user(&Led_Buff, buffer, 1);
	Updateled();
	return 1;
}

static int Emdoor_8Led_open(struct inode *inode, struct file *filp)
{
	MOD_INC_USE_COUNT;
	return 0;
}

static int Emdoor_8Led_release(struct inode *inode, struct file *filp)
{
	MOD_DEC_USE_COUNT;
	return 0;
}

static struct file_operations Emdoor_fops = {
	open:	Emdoor_8Led_open,
	write:	Emdoor_8Led_write,
	release:	Emdoor_8Led_release,
	owner: THIS_MODULE,
};


static int __init Emdoor_8Led_init(void)
{
	int ret;
	Led_Address=ioremap(SEG_LED, 4);

	ret = register_chrdev(60, DEVICE_NAME, &Emdoor_fops);
	if (ret < 0) {
		printk(DEVICE_NAME " can't get major number\n");
		return ret;
	}

	//printk(DEVICE_NAME " initialized\n");

	return 0;
}

static void __exit Emdoor_8Led_exit(void)
{	
	iounmap(Led_Address);
	unregister_chrdev(60, DEVICE_NAME);
}

module_init(Emdoor_8Led_init);
module_exit(Emdoor_8Led_exit);

