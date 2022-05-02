
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#define I2C_BUS_AVAILABLE   (          1 )              /* I2C Bus available in our Raspberry Pi */
#define SLAVE_DEVICE_NAME   ( "MY_OLED" )              /* Device and Driver Name*/
#define SSD1306_SLAVE_ADDR  (       0x3C )              /* SSD1306 OLED Slave Address*/


#define OLED_OFF _IOW('a','b',int32_t*)              /* IOCTL Function for turning the OLED off*/
#define OLED_ON _IOW('a','d',int32_t*)              /*IOCTL Function for turning the OLED on*/
#define CONTRAST_SET _IOW('a','c',char*)


static struct i2c_adapter *oled_i2c_adapter     = NULL;  /* I2C Adapter Structure*/
static struct i2c_client  *oled_i2c_client = NULL;   /*I2C Cient Structure (In our case it is OLED)*/

static dev_t value; 					/*variable for the device number*/
static struct cdev c_dev; 				/*variable for the character device structure */
static struct class *cls; 				/*variable for the device class*/


/*********************************Function to write into the I2C Client *********************************************************/

//  Arguments:
//      buff -> buffer to be sent
//      len  -> Length of the data
   
/*******************************************************************************************************************************/

int32_t mode;
char contrast;
static int I2C_Write(unsigned char *buf, unsigned int len)
{
    int ret = i2c_master_send(oled_i2c_client, buf, len);
    
    return ret;
}

/***********************************Function to read one byte data from the I2C Client ****************************************/

//  Arguments:
//      out_buff -> buffer wherer the data to be copied
//      len      -> Length of the data to be read
 
/********************************************************************************************************************************/

static int I2C_Read(unsigned char *out_buf, unsigned int len)
{
    int ret = i2c_master_recv(oled_i2c_client, out_buf, len);
    return ret;
}

/**********************************Function to send the command/ data to the LED*************************************************/
//   This function is specific to the SSD_1306 OLED. Ours is 128x32 SSD1306 OLED
//  Arguments:
//      is_cmd -> true = command, flase = data
//      data   -> data to be written
 
/********************************************************************************************************************************/
static void OLED_Write(bool is_cmd, unsigned char data)
{
    unsigned char buf[2] = {0};
    int ret;
    
    /******************************OLED_Write function***************************************************************************/
    // First byte is always control byte. Data is followed after that.
    //There are two types of data in SSD_1306 OLED i.e. Command and Data
    

    // 6th and 7th bits of Control byte decides that the next byte is, command or data.
    // for command : 6th bit of Control byte = 0 and 7th bit of Control byte = 0
    // for data : 6th bit of Control byte = 1 and 7th bit of Control byte = 0    
   /****************************************************************************************************************************/ 
    if( is_cmd == true )
    {
        buf[0] = 0x00;
    }
    else
    {
        buf[0] = 0x40;
    }
    
    buf[1] = data;
    
    ret = I2C_Write(buf, 2);
}


/************************************Initializing the SSD1306 OLED***************************************************************/

static int OLED_DisplayInit(void)
{
    msleep(100);               // delay

    /*
    //Commands to initialize the SSD_1306 OLED Display
    */
    OLED_Write(true, 0xAE); // Entire Display OFF
    OLED_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
    OLED_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
    OLED_Write(true, 0xA8); // Set Multiplex Ratio
    OLED_Write(true, 0x3F); // 64 COM lines
    OLED_Write(true, 0xD3); // Set display offset
    OLED_Write(true, 0x00); // 0 offset
    OLED_Write(true, 0x40); // Set first line as the start line of the display
    OLED_Write(true, 0x8D); // Charge pump
    OLED_Write(true, 0x14); // Enable charge dump during display on
    OLED_Write(true, 0x20); // Set memory addressing mode
    OLED_Write(true, 0x00); // Horizontal addressing mode
    OLED_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
    OLED_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
    OLED_Write(true, 0xDA); // Set com pins hardware configuration
    OLED_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
    OLED_Write(true, 0x81); // Set contrast control
    OLED_Write(true, 0x80); // Set Contrast to 128
    OLED_Write(true, 0xD9); // Set pre-charge period
    OLED_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
    OLED_Write(true, 0xDB); // Set Vcomh deselect level
    OLED_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
    OLED_Write(true, 0xA4); // Entire display ON, resume to RAM content display
    OLED_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
    OLED_Write(true, 0x2E); // Deactivate scroll
    OLED_Write(true, 0xAF); // Display ON in normal mode
    
    return 0;
}


/******************** This function Fills the complete OLED with this data byte***************************************/
static void OLED_Fill(unsigned char data)
{
    unsigned int total  = 128 * 8;  // 8 pages x 128 segments x 8 bits of data
    unsigned int i      = 0;
    
    //Fill the Display
    for(i = 0; i < total; i++)
    {
        OLED_Write(false, data);
    }
}

static int oled_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    OLED_DisplayInit();
    
    //fill the OLED with this data
    OLED_Fill(0xFF);

    pr_info("OLED Probed!!!\n");
    
    return 0;
}


static int oled_remove(struct i2c_client *client)
{   
    //fill the OLED with this data
    OLED_Fill(0x00);
    
    pr_info("OLED Removed!!!\n");
    return 0;
}

/*
// Structure that has slave device id
*/
static const struct i2c_device_id oled_id[] = {
        { SLAVE_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, oled_id);

/******************************************************* I2C driver Structure that has to be added to linux*****************************/
static struct i2c_driver oled_driver = {
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = oled_probe,
        .remove         = oled_remove,
        .id_table       = oled_id,

};



/********************************************* ioctl functionality*******************************************************************/
static long my_oled(struct file *file, unsigned int req_code, unsigned long user_data)
{
         switch(req_code) {
                		case OLED_OFF : 									
					{
						copy_from_user(&mode, (int32_t*)user_data, sizeof(mode));
						
						if(mode==1)
						{
						 OLED_Write(true, 0xAE);
						}
						
						 break;
							
					}
				case OLED_ON :
					
					{
						copy_from_user(&mode, (int32_t*)user_data, sizeof(mode));
						if(mode==0)
						{
						 OLED_Write(true, 0xAF);
						}
						break;
					}
                       
                case CONTRAST_SET :
					{
						
						copy_from_user(&contrast, (int32_t*)user_data, sizeof(contrast));
						if(contrast=='y' && mode == 0)
						{
						 OLED_Write(true, 0x81);
                         
						 OLED_Write(true, 0x01);
						}
                        
                        else if (contrast == 'n' && mode == 0)
                        {
                            OLED_Write(true, 0xAF);
                            
                        }
						break;
					
					
					}       
                	
                      
                		default: break;
                        
                        
        }
       
      
	
	 return 0;
}





/******************************************************************* I2C Board Info strucutre*******************************************/
static struct i2c_board_info oled_i2c_board_info = {
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SSD1306_SLAVE_ADDR)
    };

static struct file_operations fops =
{
	.owner = THIS_MODULE,
    //.open = ADC_file_open,
	//.release = ADC_file_close,
	//.read = ADC_file_read,
	.unlocked_ioctl = my_oled,
};


/******************************************************************** Module Init function*******************************************/
static int __init oled_driver_init(void)
{
    int ret = -1;
    oled_i2c_adapter     = i2c_get_adapter(I2C_BUS_AVAILABLE);
    
    	if((alloc_chrdev_region(&value, 177, 1, "BITS-PILANI"))<0)			/*Creation of MAJOR and MINOR numbers*/
	{
		return -1;
	}
	
	if((cls=class_create(THIS_MODULE, "oled-dev")) == NULL)			/*Class Creation*/
	{
		unregister_chrdev_region(value,1);
		return -1;
	}
	
	if((device_create(cls, NULL, value, NULL, "oled-dev")) == NULL)    		/*Device file creation with the name "adc-dev"*/
	{
		class_destroy(cls);
		unregister_chrdev_region(value,1);
		return -1;
	}

	cdev_init(&c_dev, &fops);
	if(cdev_add(&c_dev, value, 1) < 0)
	{
		
		unregister_chrdev_region(value,1);
		return -1;
	}
    
    if( oled_i2c_adapter != NULL )
    {
        oled_i2c_client = i2c_new_client_device(oled_i2c_adapter, &oled_i2c_board_info);
        
        if( oled_i2c_client != NULL )
        {
            i2c_add_driver(&oled_driver);
            ret = 0;
        }
        
        i2c_put_adapter(oled_i2c_adapter);
    }
    
    pr_info("Driver Added!!!\n");
    return ret;
}

/******************************************************************* Module Exit function********************************************/
static void __exit oled_driver_exit(void)
{
    i2c_unregister_device(oled_i2c_client);
    i2c_del_driver(&oled_driver);
    pr_info("Driver Removed!!!\n");
}

module_init(oled_driver_init);
module_exit(oled_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mayank and Urvi Barapatre");
MODULE_DESCRIPTION("Simple I2C driver explanation (SSD_1306 OLED Display Interface)");

