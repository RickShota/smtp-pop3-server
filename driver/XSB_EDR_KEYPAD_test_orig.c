/************************************************
 *  led for zlg7290 demo
 *  by Zou jian guo <ah_zou@163.com>   
 *  2004-07-07
 *
*************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <linux/kernel.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

typedef unsigned char u8;
#define DEV_NAME		"/dev/xsb_edr_kbd"


int isready(int fd)
     {
         int rc;
         fd_set fds;
         struct timeval tv;
    
         FD_ZERO(&fds);
         FD_SET(fd,&fds);
         tv.tv_sec = tv.tv_usec = 0;
    
         rc = select(fd+1, &fds, NULL, NULL, &tv);
         if (rc < 0)
           return -1;
   
         return FD_ISSET(fd,&fds) ? 1 : 0;
     }

int main(int argc, char **argv)
{

	int i,k,fd;
	unsigned char value=0;
	fd = open(DEV_NAME, O_RDWR);
	if(fd < 0)
	{
		printf("####i2c test device open fail####%d \n",fd);
		return (-1);
	}
		printf("success!!!!!!!!!!!!!!!!!!!!\n");

	while(1)
	{
		i=isready(fd);
		if(i)
		read(fd,&value,1);
		printf("%x\n",(int)value);
	}

/*
	while(1)
	{
	printf("Go To Read KeyPad.........\n");

	read(fd,&value,1);
	printf("%x\n",(int)value);

}

*/
	close(fd);
	printf("Close.......................................");
	return(0);
}


 
