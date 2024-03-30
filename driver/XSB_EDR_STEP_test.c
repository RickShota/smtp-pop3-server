/*
 * EEliod
 * step motor test.c
 * 
 * 
 */

#include    <stdio.h>
#include    <fcntl.h>

#define MOTOR_Forward       0x1
#define MOTOR_Reverse       0x2
#define MOTOR_SetSpeed       0x3
#define MOTOR_GetSpeed     0x4
#define MOTOR_STOP          0x5
#define DEC_Forward            0x6
#define DEC_Reverse            0x7

#define stmotor_dev	"/dev/xsb_edr_step"

int main_menu(void){
	int key;
	printf("\n\n");
	printf("********** MOTOR test menu **********\n");
	printf("* 1.     MOTOR Forward         *\n");
	printf("* 2.     MOTOR Reverse         *\n");
	printf("* 3.     MOTOR Set Speed       *\n");
	printf("* 4.     MOTOR Get Speed      *\n");
	printf("* 5.     MOTOR STOP            *\n");
	printf("* 6.     DEC Forward           *\n");
	printf("* 7.     DEC Reverse            *\n");
	//printf("* 8.                           *\n");
	//printf("* 9.                           *\n");
	printf("* 0.     Exit Program          *\n");
	printf("********************************\n");
	printf("\n\n");

	printf("select the command number : ");
	scanf("%d",&key);
	return key;
}


void wait_for(int count)
{
	int i,j;

	for(i=0; i<count; i++)
		for(j=0; j<1000; j++);


}

main(int ac, char *av[]){
	int key, dev;

	int speed=600;
	int i=0;
	int count;
	int j=5;

	dev = open(stmotor_dev, O_RDWR );
    	if ( dev < 0) {
        fprintf(stderr, "cannot open ST_MOTOR (%d)", dev);
        exit(2);
    }
/*
	while((key = main_menu()) != 0){
		switch(key){
			case 1: printf("\t MOTOR Forward \n");
				   ioctl(dev, MOTOR_Forward, 0);
				   break;
			case 2: printf("\t MOTOR Reverse \n");
				   ioctl(dev, MOTOR_Reverse, 0);
				   break;
			case 3: printf("\t MOTOR Speed Up \n");
				   ioctl(dev, MOTOR_SetSpeed, 0);
				   break;
			case 4: printf("\t MOTOR Speed Down \n");
				   ioctl(dev, MOTOR_GetSpeed, 0);
				   break;
			case 5: printf("\t MOTOR Stop \n");
				   ioctl(dev, MOTOR_STOP, 0);
					  break;
			case 6: printf("\t DEC Forward \n");
				   ioctl(dev, DEC_Forward, 0);
					  break;
			case 7: printf("\t DEC Reverse \n");
				   ioctl(dev, DEC_Reverse, 0);
					  break;
			default:  printf("\t Unknow command... \n");
				  //close(dev);
				  printf("Close program\n");
				  exit(2);
					  break;
        } 
  	}   

*/



	while(1)
	{  
		count=100;
	 
		for(; count>0; count--)
		ioctl(dev, MOTOR_Forward, speed);

		wait_for(100);

		count=100;
		for(; count>0; count--)
		ioctl(dev, MOTOR_Reverse, speed);
		ioctl(dev, DEC_Reverse, speed);

		wait_for(150);
		count=100;
		for(; count>0; count--)
		ioctl(dev, MOTOR_Reverse, speed);

		wait_for(100);

		count=100;
		for(; count>0; count--);
		ioctl(dev, DEC_Forward, speed);

		wait_for(150);
		i++;
		speed += 10*i;

		if (speed>1600)
			{speed=800; i=0;}

	}




	ioctl(dev,5,0);  
}      

/* EOF */
