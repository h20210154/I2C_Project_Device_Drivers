# I2C_Project_Device_Drivers
I2C_Project_Device_Drivers

SUMMARY

The project is interfacing the SSD1306 OLED display to the RaspberryPi 4 board using I2C Protocol.I2C slave devices have 7 bits to represent the address so total 127 devices can be connected using the I2C bus.Here SSD1306 is connected to the board via one I2c line with address 0x3C. The Project allows the user to turn on/off the OLED and to set the contrast value by entering the Mode values.
************************************************************
Hardware Used
> RaspberryPi 4 board
> SSD1306 OLED Display
> Jumper wires for connection
************************************************************
I2C Interface
The device supports I2C protocol for communication. As the i2c protocol is based on the Platform Drivers subsystem in Linux, it follows the style of defining the driver and device structure keeping the name of device and driver same. Both the driver and device must be registered for the kernel to know they exist. Once the matching driver structure for the given device is found the probe function is called.Apart from these there a=is adapter structure in I2C that tells the busnumber the device is connected to and the algorithm being used by the network.

oled_i2c_adapter is the i2c adapter structure that defines i2c bus 1 as the bus number. I2C_Write function is used to actually write to the client using the I2C bus.I2C_Read function is used to actually get the data from the client using the I2C bus but since OLED is a client that wont send any data to the master , this read function is not being used in this project.

**************************************************************
Kernel Driver
The Kernel Space code consists of the following functions to establish the proper communication between the client(OLED) and Master ( RaspberryPi 4) . Also the kernel allows user to give the mode the OLED should work in. In this project OLED acts as a character device.
Following functions are present in Kernel related to I2C (Bus Protocol)
>Function to write into the I2C Client
>Function to read one byte data from the I2C Client 
>I2C driver Structure that has to be added to linux
>I2C Board Info strucutre

Following functions are present in Kernel related to OLED (CLient)
>Function to send the command/ data to the LED
>OLED_Write function
>Initializing the SSD1306 OLED
>Fills the complete OLED with this data byte

Following functions are present in Kernel that allows the user to talk and make system calls based on which some functionality can be done on the hardware
>ioctl function definition( 3 functions are implemented : OLED_OFF, OLED_ON, CONTRAST_SET) 

>Module Init function
>Module Exit function
***************************************************************

Userspace code
The Userspace code asks the client about which mode he wants the oled to be in. Depending on the input given by the user the mode of OLED changes.
Mode = 1 : Turns the OLED Display OFF
Mode = 0 : Turns the OLED Display ON
As user enters the Mode as 1 , the ioctl function for turning off the OLED is called( OLED_OFF)
As user enters the Mode as 0 , the ioctl function for turning on the OLED is called( OLED_ON)
As user enters the Y as input , the ioctl function for adjusting the contrast the OLED is called( CONTRAST_SET)
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


FOLLOW THE FOLLOWING STEPS FOR COMPILATION AND SEEING WORKING OF PROJECT
1) Enter "make" command in the terminal. This will build the kernel space code as per our Makefile
2) Enter "gcc -o userspace user.c" command in the terminal. This will compile the userspace code
3)On successfull compilation and generatiion of oled.ko file of kernel space code, enter command "sudo insmod oled.ko" to insert the code into kernel.You'll see the OLED backlight turns on as soon as insert the module in the Kernel Space. 
4)Enter command "ls /dev" to check if the device is registered in the kernel or not. On successful registering device "oled-dev" can be seen in the list of devices in kernel.
5)Enter "sudo ./userspace" command to run the userspace code.You'll be asked to enter the Mode which you want your OLED to work on. 
	Enter Mode = 1  to turn off the OLED display
	Enter Mode = 0  to turn on the OLED display
	Enter Mode = Y to set the Contrast of OLED display
Any other Mode value entered except these will show invalid input.
6)Enter "sudo rmmod oled" to remove the code from kernel space. You'll see the OLED backlight turns off after you enter this command. 




 



