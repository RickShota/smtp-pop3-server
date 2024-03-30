
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/arch-pxa/pxa-regs.h>
#include <linux/fs.h>
#include <linux/mm.h>

/* debug macros */

#define MAX_KBD_BUF	16	/* how many do we want to buffer */
#define IRQ_KEYPAD      4

#define DEVICE_NAME	"emdoor_kbd"   /*设备的名字叫做emdoor_kbd，可以在/dev下面看到*/ 
#define KEYVALUE_HAVE  1
#define KEYVALUE_NO    0

#define KPC_DIR		1<<5
#define KPC_MAT		1<<22
static int Emdoor_kbd_fasync(int, struct file* ,int);
typedef unsigned char KBD_RET;

struct fasync_struct * fasync;

typedef struct {
	KBD_RET kbd_buff;		/* protect against overrun */
	unsigned int kbd_status;
	wait_queue_head_t wq;
	spinlock_t lock;

//struct fasync_struct * fasync;
} KBD_DEV;


static KBD_DEV kbddev;


static int kbdMajor=0;

static ssize_t Emdoor_kbd_read(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{		

		KBD_DEV * kbd=filp->private_data;

		KBD_RET kbd_ret;
		while(1)
		{
			if(kbd->kbd_status==KEYVALUE_HAVE)
			{
			 kbd_ret = kbd->kbd_buff;
			 copy_to_user(buffer, &kbd_ret, sizeof(KBD_RET));
			 kbd->kbd_status=KEYVALUE_NO;
			 return sizeof(KBD_RET);
			}
			else{

			if (filp->f_flags & O_NONBLOCK)
			  return -EAGAIN;
			interruptible_sleep_on(&(kbd->wq));
			if (signal_pending(current))
			return -ERESTARTSYS;
			}
		}
		return sizeof(KBD_RET);
		
		
}


static void Emdoor_isr_kbd(int irq, void *dev_id, struct pt_regs *reg)
{

		int kpc_value;
		KBD_DEV * kbd = (KBD_DEV *) dev_id;
		spin_lock_irq(&(kbd->lock));
//		printk(KERN_EMERG"INTERRUPT OCCOUR\n");
		kpc_value=KPC;
		if(kpc_value&KPC_MAT)
		{
			kbd->kbd_buff=KPAS&0xFF;
			if(kbd->kbd_buff!=0xFF){
			    switch(kbd->kbd_buff){
				case 0x0:  kbd->kbd_buff=5;  break;
				case 0x1:  kbd->kbd_buff=6;  break;
				case 0x2:  kbd->kbd_buff=7;  break;
				case 0x5:  kbd->kbd_buff=8;  break;
				case 0x10: kbd->kbd_buff=9;  break;
				case 0x11: kbd->kbd_buff=10; break;
				case 0x12: kbd->kbd_buff=11; break;
				case 0x15: kbd->kbd_buff=12; break;
				case 0x20: kbd->kbd_buff=13; break;
				case 0x21: kbd->kbd_buff=14; break;
				case 0x22: kbd->kbd_buff=15; break;
				case 0x25: kbd->kbd_buff=16; break;
				default:    break;
				}
				kbd->kbd_status=KEYVALUE_HAVE;
//		wake_up_interruptible(&(kbd->wq));		
			}

		}
		else if(kpc_value&KPC_DIR){
			kbd->kbd_buff=KPDK&0xFF;
			if(kbd->kbd_buff!=0x0){
			    switch(kbd->kbd_buff){
				case 0x40:  kbd->kbd_buff=1;  break;
				case 0x2:   kbd->kbd_buff=2;  break;
				case 0x4:   kbd->kbd_buff=3;  break;
				case 0x20:  kbd->kbd_buff=4;  break;
				default:	   break;
				}
			kbd->kbd_status=KEYVALUE_HAVE;
//	wake_up_interruptible(&(kbd->wq));		
			}					

			}
		if ( fasync )
			kill_fasync( &(fasync), SIGIO, POLL_IN );
		wake_up_interruptible(&(kbd->wq)); /*唤醒等待队列*/

	spin_unlock_irq(&(kbd->lock));

}

static int Emdoor_kbd_open(struct inode *inode, struct file *filp)
{
	int ret;
	KBD_DEV * kbd;
	MOD_INC_USE_COUNT;



	kbd =(KBD_DEV *) kmalloc(sizeof(KBD_DEV ), GFP_KERNEL);
//	KPC=0x2FAFF9C3;               //閿洏涓柇鍒濆鍖?
	KPC=KPC_ASACT | (3<<26) | (7<<23 ) | KPC_IMKP | KPC_MS6 | KPC_MS5 | KPC_MS4 | KPC_MS3 | KPC_MS2 | KPC_MS1 | KPC_MS0 | KPC_ME | KPC_MIE | (7<<6) | KPC_DE | KPC_DIE;	

	init_waitqueue_head(&(kbd->wq));
	set_GPIO_mode(94  | GPIO_ALT_FN_1_IN);   /*include <asm/arch-pxa/pxa-regs.h>*/
	set_GPIO_mode(95  | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(98  | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(99  | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(100 | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(101 | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(102 | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(103 | GPIO_ALT_FN_2_OUT);
	set_GPIO_mode(104 | GPIO_ALT_FN_2_OUT);
	set_GPIO_mode(105 | GPIO_ALT_FN_2_OUT);
	set_GPIO_mode(108 | GPIO_ALT_FN_2_OUT);

	kbd->kbd_status=KEYVALUE_NO;
//	kbddev.kbd_status=KEYVALUE_NO;

	filp->private_data=kbd;
	// Enable interrupt
	ret = request_irq(IRQ_KEYPAD, Emdoor_isr_kbd, SA_INTERRUPT, DEVICE_NAME, kbd);
	
	if (ret)
	{
	printk(KERN_EMERG " Interrupt init=%x!!!!\n",ret);
	return ret;
	}
	
	return 0;
}


static unsigned int Emdoor_kbd_poll(struct file *filp,struct poll_table_struct *wait)  /*与应用程序中Select对应*/
{
	KBD_DEV * kbd=filp->private_data;
	poll_wait(filp,&(kbd->wq),wait);             /*将select中的应用加入等待队列*/
	return (kbd->kbd_status==KEYVALUE_HAVE) ?  (POLLIN|POLLRDNORM): 0  ;

}

static int Emdoor_kbd_release(struct inode *inode, struct file *filp)
{	
	KBD_DEV * kbd = filp->private_data;
	KPC=0x0;

	Emdoor_kbd_fasync(-1, filp, 0);	 /*本文件的操作从上述的 设备异步事件等待链表中剥离。*/
	kfree(kbd );
	free_irq(IRQ_KEYPAD, kbd);
	MOD_DEC_USE_COUNT;
	return 0;
}


static int Emdoor_kbd_fasync(int fd, struct file * file, int mode)   /*与应用程序中fcntl设置的文件描述符状态FASYNC关联*/
{
	KBD_DEV *kbd = file->private_data;

	return  fasync_helper(fd, file, mode, &(fasync) );
	

}



static struct file_operations Emdoor_fops = {
	open:		Emdoor_kbd_open,
	read:		Emdoor_kbd_read,	
	release:	Emdoor_kbd_release,
	poll:		Emdoor_kbd_poll,
	fasync:		Emdoor_kbd_fasync,
	owner:		THIS_MODULE,
	
};


static int __init Emdoor_kbd_init(void)
{
	int ret;

	ret = register_chrdev(62, DEVICE_NAME, &Emdoor_fops);
	if (ret < 0) {
		printk(DEVICE_NAME " can't get major number\n");
		return ret;
	}
	
	free_irq(IRQ_KEYPAD,NULL);                     //释放分配给已定中断的内存,造成使用该中断的其他驱动错误,不妥
	udelay(10);                                    //此处无意义 ???

	//printk(KERN_INFO " Emdoor_kpd initialized\n");

	return 0;
}


static void __exit Emdoor_kbd_exit(void)
{	
	unregister_chrdev(62, DEVICE_NAME);
}

module_init(Emdoor_kbd_init);
module_exit(Emdoor_kbd_exit);

