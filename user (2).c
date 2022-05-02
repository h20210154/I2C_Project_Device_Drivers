
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>             
#include <unistd.h>             
#include <sys/ioctl.h>        

#define OLED_OFF _IOW('a','b',int32_t*)              // SSD1306 OLED Slave Address
#define OLED_ON _IOW('a','d',int32_t*) 
#define CONTRAST_SET _IOW('a','c',char*)
#define DEVICE_FILE_NAME "/dev/oled-dev"


int main()
{
    int fd, ret_val;
	int mode;
	
	fd = open(DEVICE_FILE_NAME, O_RDWR);
	
	   if (fd < 0) 
	{
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
        }
	
	printf("Please enter the mode (1 to OFF, 0 to ON): \n");
	scanf(" %d", &mode);
	
	printf("press 'y' if you want to chnage contrast to '128' and 'n' if don't want to change: \n");
	scanf(" %c", &contrast);
	
	int fflush( FILE *stream );
	int fflush( FILE *stream );
	


    fd = open(DEVICE_FILE_NAME, O_RDWR);
	
	if(mode==1) 
	{
	ioctl(fd, OLED_OFF, (int32_t*)&mode);
	}
	else if(mode==0 && contrast == 'y')
	
	{
	ioctl(fd, OLED_ON, (int32_t*)&mode);
	ioctl(fd, CONTRAST_SET, (int32_t*)&contrast);
	}
	
	else if(mode==0 && contrast == 'n')
	
	{
	ioctl(fd, OLED_ON, (int32_t*)&mode);
	ioctl(fd, CONTRAST_SET, (int32_t*)&contrast);
	}
	
	else printf("Wrong Input\n");
	
    if (fd < 0) 
	{
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }
    

    close(fd);
    return 0;
}
