/************************************************
*************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <linux/kernel.h>
#include <unistd.h>

typedef unsigned char u8;
#define I2C_DEV		"/dev/xsb_edr_8seg"

#define NUM_0  0x3f
#define NUM_1  0x06
#define NUM_2  0x5b
#define NUM_3  ~0x30
#define NUM_4  ~0x19
#define NUM_5  ~0x12
#define NUM_6  ~0x2
#define NUM_7  ~0x78
#define NUM_8  ~0x0
#define NUM_9  ~0x10

char number[]={NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9};


void clear_led(int fd)
{
  int i;
  char val=0;                // 传0,使驱动ioctl中arg=0     ???
  for(i=1;i<=6;i++)		  // if(!arg) return -EINVAL;   ???
	{
		ioctl(fd, i, &val);
	}
  sleep(1);
}


void appear_same(int fd)
{
  char i,j,base=0;
 	
  for (j=0, base=0 ;j<=9; j++, base++)
	{ for(i=1; i<=6; i++)
		ioctl(fd, i, number+base);

	  sleep(1);
	  
	}
	
}


void appear_roll(int fd)
{
  char i, j, base=0;
	
  for (j=0, base=0; j<=9; j++, base++)
	{
        for(i=1; i<=6; i++)
		ioctl(fd, i, number+(base+i-1)%10);

	sleep(1);
	}


}

int main(int argc, char **argv)
{
	int fd;
	int i=1,j=0,base=0;
	int	k=0x3F;
	unsigned char value,led[6]={0x01,0x2,0x4,0x8,0x10,0x20};
	fd = open(I2C_DEV, O_RDWR);
	if(fd < 0)
	{
		printf("####i2c test device open fail####%d \n",fd);
		return (-1);
	}
	


//	ioctl(fd, 0, 0);

	while(1)
	{
	 
	appear_same(fd);
	clear_led(fd);
	appear_roll(fd);
	clear_led(fd);

	
	}
/*
	k=0x3F;
	ioctl(fd,1,&k);
	k=0x06;
	ioctl(fd,2,&k);
	k=0x5B;
	ioctl(fd,3,&k);
	k=0x4f;
	ioctl(fd,4,&k);
	k=0x6d;
	ioctl(fd,5,&k);
	k=0x	
//	write(fd,led,6);
*/
	close(fd);
	printf("Close.......................................");
	return(0);
}
