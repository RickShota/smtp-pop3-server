/*
 * EEliod V3 -  step motor
 * 2006-08-01   <figo.zhang@emdoor.com>
 * 
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/rtc.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <asm/system.h>

#include <asm/hardware.h>
#include <asm/delay.h>
#include <asm/arch/pxa-regs.h>
#include <asm/irq.h>
#include <asm/mach/time.h>
#include <asm/io.h>

#define ST_MOTOR_MAJOR       	63
#define ST_MOTOR_NAME  		"st_motor"

#define GPIO_84_PullHigh()	GPSR2|=GPIO_bit(84)	//用于控制步进电机方向
#define GPIO_84_PullLow()	GPCR2|=GPIO_bit(84)
#define GPIO_83_PullHigh()	GPSR2|=GPIO_bit(83)	//用于产生步进电机脉冲
#define GPIO_83_PullLow()	GPCR2|=GPIO_bit(83)
#define GPIO_82_PullHigh()	GPSR2|=GPIO_bit(82)	//用于直流电机
#define GPIO_82_PullLow()	GPCR2|=GPIO_bit(82)
#define GPIO_81_PullHigh()	GPSR2|=GPIO_bit(81)	//用于直流电机
#define GPIO_81_PullLow()	GPCR2|=GPIO_bit(81)
#define GPIO_53_PullHigh()	GPSR1|=GPIO_bit(53)
#define GPIO_53_PullLow()	GPCR1|=GPIO_bit(53)

#define Forward			1 
#define Reverse			0

#define MOTOR_Forward    	0x1
#define MOTOR_Reverse    	0x2
#define MOTOR_SetSpeed   	0x3
#define MOTOR_GetSpeed   	0x4
#define MOTOR_STOP       	0x5
#define DEC_Forward            0x6
#define DEC_Reverse            0x7

static int g_uPWMRunTimes=0;	//步进机的运行脉冲数
static int g_HighTime=1250;	//以微秒为单位，未校正前的理想值
static int g_LowTime=1250;
//static int g_High=1;	//占空比
//static int g_Low=1;

static int g_HighTimeA=1125;	//校正后实际要使用的值
static int  g_LowTimeA=1000;

static int g_DCERunTime=1000;		//以毫秒为单位
static long g_Positive=0;	//直流电机转向，FFFFFFFF为正向，即GPIO81为高，GPIO82为低
static int g_ZLDJPositive=1;	//步进电机方向



struct step 
{
int speed;
char direction;
};

struct dec
{
int speed;
char direction;
};


struct motor
{
struct step st_moto;
struct dec dec_moto;
};



void CorrectData(void)
{
	g_HighTimeA=g_HighTime*47;
	g_HighTimeA/=60;
	g_LowTimeA=g_LowTime*83;
	g_LowTimeA/=100;	
}

void st_handler(struct motor *mo)	//步进电机驱动
{
			int i;
	
			GPIO_53_PullLow();//power on
			udelay(10);

			if (mo->st_moto.direction)
			//if(g_ZLDJPositive)
				GPIO_84_PullLow();
			else	
				GPIO_84_PullHigh();
			
		
			for( i=0;i<10;i++) {	
					
				GPIO_83_PullHigh();
				udelay(mo->st_moto.speed);
				GPIO_83_PullLow();
				udelay(mo->st_moto.speed);
				
			}
			GPIO_84_PullLow();
	

		GPIO_53_PullHigh();	//power off
		udelay(10);
}

static void DecHandler(struct motor * mo) 
{
	int i;
	GPIO_82_PullHigh();
	GPIO_81_PullHigh();
	

	if (mo->dec_moto.direction)
	{	
		GPIO_81_PullHigh();
		udelay(10);			
		GPIO_82_PullLow();
		mdelay(150);
	
		GPIO_82_PullHigh();	
	}
	else{
		GPIO_82_PullHigh();
		udelay(10);
		GPIO_81_PullLow();
		mdelay(150);	
		GPIO_81_PullHigh();	
	}
	
	GPIO_82_PullHigh();
	GPIO_81_PullHigh();

	udelay(40);	
}


static int st_motor_open(struct inode *inode, struct file *filp)
{

	struct motor * motor_edr;

	motor_edr =(struct motor *)kmalloc(sizeof(struct motor), GFP_KERNEL);
	filp->private_data = motor_edr;
	return 0;
}

static int st_motor_release(struct inode *inode, struct file *filp)
{

	kfree(filp->private_data);
	return 0;        
}                         

static ssize_t st_motor_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

static ssize_t st_motor_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos )
{
	return 0;
}

static int st_motor_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{

	struct motor * mo;

	mo=(struct motor *)filp->private_data;

	
	switch (cmd) { 
		case MOTOR_Forward : {
				mo->st_moto.direction=1;
				if (arg!=NULL)
					mo->st_moto.speed=arg;
				//g_ZLDJPositive=1;
				st_handler(mo);
			break;
				 }
				 
		case MOTOR_Reverse : {
				mo->st_moto.direction=0;
				if (arg != NULL)	
					mo->st_moto.speed=arg;
				//g_ZLDJPositive=0;
				st_handler(mo);
					 break;
	 			 }
	 			 
		case MOTOR_SetSpeed : {
		 		
					 break;
	 			 }
		case MOTOR_GetSpeed : {
			
					 
					 break;
	 			 }
		case MOTOR_STOP : {
				GPIO_53_PullHigh();	
					 break;
	 			 }
		case DEC_Forward : {
				mo->dec_moto.direction=1;
				if (arg != NULL)
					mo->dec_moto.speed = arg;
			//	g_Positive = 1;
				DecHandler(mo);
					break;
				
				}
		case DEC_Reverse : {
				mo->dec_moto.direction=0;
				if (arg != NULL)
					mo->dec_moto.speed = arg;
			//	g_Positive = 0;
				DecHandler(mo);
					break;
				}
		default:
				 return 0;
	}
	return 0;
}

static struct file_operations st_motor_fops = {
	read	:	st_motor_read,
	write	:	st_motor_write,
	ioctl	:	st_motor_ioctl,
	open	:	st_motor_open,
	release	:	st_motor_release,
};

static int st_motor_init(void)
{
	int result;
	//unsigned long mem_addr, mem_len;

	result = register_chrdev(ST_MOTOR_MAJOR, ST_MOTOR_NAME, &st_motor_fops);
	if (result < 0) {
		printk(KERN_WARNING " can't get major \n");
		return result;
	}

    	g_uPWMRunTimes=0;
    	g_HighTime=1250;	
    	g_LowTime=1250;
	g_DCERunTime=1000;
	g_Positive=0xFFFFFFFF;
	CorrectData();//校正占空比和频率误差
	
	GPDR2 |= GPIO_bit(84);
	GPDR2 |= GPIO_bit(83);
	GPDR2 |= GPIO_bit(82);
	GPDR2 |= GPIO_bit(81);
	GPDR1 |= GPIO_bit(53);

	//pxa_gpio_mode(84| GPIO_OUT);
	//pxa_gpio_mode(83| GPIO_OUT);
	//pxa_gpio_mode(82| GPIO_OUT);
	//pxa_gpio_mode(81| GPIO_OUT);
	//pxa_gpio_mode(53| GPIO_OUT);

	GPIO_84_PullLow();
	GPIO_83_PullLow();
	GPIO_82_PullHigh();
	GPIO_81_PullHigh();
	GPIO_53_PullHigh();
	
	return 0;
}

static void st_motor_exit(void)
{
	//free_irq(OSTIMER_IRQ, NULL);
	unregister_chrdev(ST_MOTOR_MAJOR, ST_MOTOR_NAME);
	//iounmap(mem_base);
}

module_init(st_motor_init);
module_exit(st_motor_exit);

/* EOF */
