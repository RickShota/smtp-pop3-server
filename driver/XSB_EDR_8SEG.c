#include <linux/module.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/hardware.h>
#include <linux/ioport.h>
#include <linux/malloc.h>
#include <asm/io.h>

#include <asm/delay.h>
#include <asm/uaccess.h>

char LED_MODULE=0;
MODULE_PARM ( LED_MODULE, "b" );


#define DEVICE_NAME	"emdoor_8seg"
#define SEG_CS1  	0x10200000
#define SEG_CS2  	0x10300000
#define SEG_CS3  	0x10400000
static char LED[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

static int SegMajor = 0;

unsigned long *cs1_address, *cs2_address, *cs3_address;


struct seg
{
char led1_val;
char led2_val;
char led3_val;
char led4_val;
char led5_val;
char led6_val;

char negative;
};



static void Updateled(struct seg *seg_7)
{
	unsigned short buff=0x00;
	buff=seg_7->led1_val;
	buff=buff|( seg_7->led2_val <<8);
	writew(buff,cs1_address);

	buff=0x00;
	buff=seg_7->led3_val;
	buff=buff|( seg_7->led4_val<<8);
	writew(buff,cs2_address);

	buff=0x00;
	buff=seg_7->led5_val;
	buff=buff|( seg_7->led6_val<<8);
	writew(buff,cs3_address);

	return;
	
}




void value_seting(struct seg *seg_7, char position, char value)
{	
	if (seg_7->negative==0)
		value=~value & ~(0x1<<7);
	else 	
	  	value=(0x1<<7)|value;
	
	if (position==1)
		seg_7->led1_val=value;
	else if(position==2)
		seg_7->led2_val=value;
	else if(position==3)
		seg_7->led3_val=value;
	else if(position==4)
		seg_7->led4_val=value;
	else if(position==5)
		seg_7->led5_val=value;
	else if(position==6)
		seg_7->led6_val=value;		
}




static ssize_t Emdoor_8Seg_write(struct file *file, const char *buffer, size_t count, loff_t * ppos)
{
	int i;
	struct seg *seg_7=file->private_data;
	char led_forall[6];	

	if(count!=6)
	{
	printk(KERN_EMERG "the count of input is not six!!");
	return 0;
	}
	copy_from_user(led_forall, buffer, 6);

	for(i=1;i<=6;i++)
	{
		value_seting(seg_7, i, LED[i-1]);        //LED[]均为0xFF; ??
	}                                              //应改为led_forall ??


	Updateled( seg_7 );
	
	return 6;
}


static int Emdoor_8Seg_ioctl(struct inode *ip, struct file *fp, 
				unsigned int cmd, unsigned long arg)
{

	char val=0x00;
	struct seg *seg_7=fp->private_data;

	if (!arg) 
	return -EINVAL;
	if (copy_from_user(&val, (int *)arg, sizeof(char))) 
	  	return -EFAULT;



	switch(cmd){
		case 1:
			value_seting(seg_7, 1, val);	
			break;

		case 2:	
			value_seting(seg_7, 2, val);
			break;

		case 3:	
			value_seting(seg_7, 3, val);
		 	 break;

		case 4:	
			value_seting(seg_7, 4, val);
			break;

		case 5:	
			value_seting(seg_7, 5, val);
			break;

		case 6:	
			value_seting(seg_7, 6, val);
			break;
		
		case 0:
			seg_7->negative = LED_MODULE;
			break;

		default:	
			printk(KERN_EMERG"ioctl parameter input error,please input number 0-6");
			break; 
		}

	Updateled( seg_7 );        //结构问题,ioctl 不应该 改写结果
		
	return 0;

    
}


static int Emdoor_8Seg_open(struct inode *inode, struct file *filp)
{
	struct seg *seg_7;

	seg_7=kmalloc(sizeof(struct seg), GFP_KERNEL);
	seg_7->negative=LED_MODULE;
	
	filp->private_data=seg_7;                  //存放 8段管的显示数据指针,在
	MOD_INC_USE_COUNT;                        //驱动各部分传递
	             
	return 0;
}


static int Emdoor_8Seg_release(struct inode *inode, struct file *filp)
{
	kfree(filp->private_data);
	MOD_DEC_USE_COUNT;
	return 0;
}


static struct file_operations Emdoor_fops = {
	open:	Emdoor_8Seg_open,
	write:	Emdoor_8Seg_write,
	release:	Emdoor_8Seg_release,
	ioctl:	Emdoor_8Seg_ioctl,
	owner: THIS_MODULE,
};



static int __init Emdoor_8Seg_init(void)
{
	int ret;
	cs1_address=ioremap(SEG_CS1, 32);
	cs2_address=ioremap(SEG_CS2, 4);
	cs3_address=ioremap(SEG_CS3, 4);


	ret = register_chrdev(61, DEVICE_NAME, &Emdoor_fops);
	if (ret < 0) {
		printk(DEVICE_NAME " can't get major number\n");
		return ret;
	}

	return 0;
}


static void __exit Emdoor_8Seg_exit(void)
{
	iounmap(cs1_address);
	iounmap(cs2_address);
	iounmap(cs3_address);
	unregister_chrdev(61, DEVICE_NAME);
}


module_init(Emdoor_8Seg_init);
module_exit(Emdoor_8Seg_exit);


