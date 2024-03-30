/************************************************
 Be sure the Major Num 60 not be used
 Implement: mknod "/dev/xsb_edr_8led" c 60 1



*************************************************/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <termio.h>
#include <fcntl.h>
//nclude <linux/kernel.h>

#define LED_DEV		"/dev/xsb_edr_8led"
int fd;
unsigned char led=0xff;


void led_delay( unsigned long time)
{
   unsigned long i,j;
   for (i=0; i<time; i++)
	for(j=0; j<2000; j++);


}

void led_operation(int sig_num)
{
	static char led_change=1;
	if (led_change == 0)
	  	led = 0xff;
	else
		led = 0x0;

	led_change ++;
	led_change %=2;
	write(fd, &led, 1);
	

  	alarm(1);		
}

int main(int argc, char **argv)
{
	int i,k;
	int input;
	unsigned char led=0xff;
	fd = open(LED_DEV, O_RDWR);
	if(fd < 0)
	{
		printf("####XSB_EDR_8LED test device open fail####%d \n",fd);
		return (-1);
	}

	signal(SIGALRM, led_operation );
	alarm(1);
	getchar();
/*	
	while(1)
	{
	  write(fd,&led,1);

	  led_delay(5000);
	  if (led <= 0 )
		led = 0xff;	
	  led --;		
	}

*/	close(fd);
	printf("Close fd");
	return(0);
}


