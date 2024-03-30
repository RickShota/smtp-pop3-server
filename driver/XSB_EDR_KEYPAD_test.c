#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

struct input_event
{
	struct timeval time;
	unsigned short type;
	unsigned short code;
	unsigned int value;
};



int fd_key, fd_sw, fd_led, fd_max;
fd_set readfds;
void handle();

int main(void)
{
 
   char buf;
   int dac_value;
   long oflags; 


   
   fd_key= open("/dev/xsb_edr_kbd",O_RDWR);
 

       fd_max = fd_key;
  
   signal(SIGIO, &handle);
   fcntl(fd_key, F_SETOWN, getpid());
   oflags = fcntl(fd_key, F_GETFL);          //获得原句柄读写flag
   fcntl(fd_key, F_SETFL, oflags | FASYNC);  //设置对应驱动中 fasync() 非同步事件
 

   while(1)
    {

    }


}

void handle()
{  char buf=0;
    
 
   {
     
      FD_ZERO(&readfds);
      FD_SET(fd_key, &readfds);
 //     FD_SET(fd_sw, &readfds); 
       
      select(fd_max + 1, &readfds, NULL, NULL, NULL);
      if ( FD_ISSET(fd_key, &readfds))
         { 
           read(fd_key, &buf, sizeof(char));
    	   printf("Scancode is 0x%08x\n", buf);  
         }
     
    }
}
