#include "driver_LCD_ILI9481.h"
#include "delay.h"

/***  生长方向
//------>  Y   480
|
|
|
|
X

320

***/

#define ILI9481_PIX_WIDTH  480 
#define ILI9481_PIX_HEIGHT 320


#define ILI9481_SET_X_CMD  0X2A
#define ILI9481_SET_Y_CMD  0X2B
#define ILI9481_WRITE_GRAM_CMD  0X2C

#define lcd_delay_ms  delay_ms
#define lcd_write_reg driver_LCD_ILI9481_write_reg
#define lcd_write_data driver_LCD_ILI9481_write_data
#define lcd_read_data driver_LCD_ILI9481_read_data

static void lcd_config(LCDTypedef *lcd);
static void lcd_init(void);

//----------- start---------
// 初始化
void driver_LCD_ILI9481_init(LCDTypedef *lcd)
{
	// 配置初始化
	lcd_config(lcd);
	
	// 初始化液晶屏
	lcd_init();
}

// 写命令
void driver_LCD_ILI9481_write_reg(uint16_t value)
{
	FSMC_LCD->REG =  value;
}

// 写数据
void driver_LCD_ILI9481_write_data(uint16_t value)
{
	FSMC_LCD->RAM = value;
}

// 写颜色
void driver_LCD_ILI9481_write_color(uint16_t value)
{
	#if USE_8080_16BIT
	FSMC_LCD->RAM = value;
	#else
	FSMC_LCD->RAM = value>>8;
	FSMC_LCD->RAM = value&0xff;
	#endif
}
// 读取数据
uint16_t driver_LCD_ILI9481_read_data(void)
{
	volatile uint16_t value = FSMC_LCD->RAM;
	return value;
}

void driver_LCD_ILI9481_write_gram_pre(void)
{
	FSMC_LCD->REG = ILI9481_WRITE_GRAM_CMD;
}

// 写窗口
void driver_LCD_ILI9481_set_windows(uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y)
{
	lcd_write_reg(ILI9481_SET_X_CMD);   
	lcd_write_data(start_x>>8);
	lcd_write_data(start_x&0xff);
	lcd_write_data(end_x>>8);
	lcd_write_data(end_x&0xff);

	lcd_write_reg(ILI9481_SET_Y_CMD);   
	lcd_write_data(start_y>>8);
	lcd_write_data(start_y&0xff);
	lcd_write_data(end_y>>8);
	lcd_write_data(end_y&0xff);

	lcd_write_reg(ILI9481_WRITE_GRAM_CMD);	
}

// 写窗口
void driver_LCD_ILI9481_set_cursor(uint16_t x,uint16_t y)
{
	lcd_write_reg(ILI9481_SET_X_CMD);   
	lcd_write_data(x>>8);
	lcd_write_data(x&0xff);
	lcd_write_data(x>>8);
	lcd_write_data(x&0xff);

	lcd_write_reg(ILI9481_SET_Y_CMD);   
	lcd_write_data(y>>8);
	lcd_write_data(y&0xff);
	lcd_write_data(y>>8);
	lcd_write_data(y&0xff);

	lcd_write_reg(ILI9481_WRITE_GRAM_CMD);	
}




// ----------- end -----------

// 配置初始化
static void lcd_config(LCDTypedef *lcd)
{
	
	lcd->width = ILI9481_PIX_WIDTH;
	lcd->height = ILI9481_PIX_HEIGHT;
	
	lcd->lcd_8080.set_x_cmd = ILI9481_SET_X_CMD;
	lcd->lcd_8080.set_y_cmd = ILI9481_SET_Y_CMD;
	lcd->lcd_8080.w_ram_cmd = ILI9481_WRITE_GRAM_CMD;
	
	lcd->lcd_8080.set_set_windows = driver_LCD_ILI9481_set_windows;
	lcd->lcd_8080.set_set_cursor = driver_LCD_ILI9481_set_cursor;
	lcd->lcd_8080.write_reg = driver_LCD_ILI9481_write_reg;
	lcd->lcd_8080.write_data = driver_LCD_ILI9481_write_data;
	lcd->lcd_8080.read_data = driver_LCD_ILI9481_read_data;
	lcd->lcd_8080.write_color = driver_LCD_ILI9481_write_color;
	lcd->lcd_8080.write_gram_pre = driver_LCD_ILI9481_write_gram_pre;
}


// 液晶初始化
static void lcd_init(void)
{
	//TEST  cmi35mva 20180510 ok优秀
	int i,j;	
	lcd_write_reg(0x01); //Soft_rese
	lcd_delay_ms(220);

	lcd_write_reg(0x11);
	lcd_delay_ms(280);

	lcd_write_reg(0xd0); //Power_Setting
	lcd_write_data(0x07);//07  VC[2:0] Sets the ratio factor of Vci to generate the reference voltages Vci1
	lcd_write_data(0x44);//41  BT[2:0] Sets the Step up factor and output voltage level from the reference voltages Vci1
	lcd_write_data(0x1E);//1f  17   1C  VRH[3:0]: Sets the factor to generate VREG1OUT from VCILVL
	lcd_delay_ms(220);

	lcd_write_reg(0xd1); //VCOM Control
	lcd_write_data(0x00);//00
	lcd_write_data(0x0C);//1A   VCM [6:0] is used to set factor to generate VCOMH voltage from the reference voltage VREG1OUT  15    09
	lcd_write_data(0x1A);//1F   VDV[4:0] is used to set the VCOM alternating amplitude in the range of VREG1OUT x 0.70 to VREG1OUT   1F   18

	lcd_write_reg(0xC5);  //Frame Rate
	lcd_write_data(0x03); // 03   02

	lcd_write_reg(0xd2);  //Power_Setting for Normal Mode 
	lcd_write_data(0x01);  //01
	lcd_write_data(0x11);  //11

	lcd_write_reg(0xE4);  //?
	lcd_write_data(0xa0);
	lcd_write_reg(0xf3);
	lcd_write_data(0x00);
	lcd_write_data(0x2a);

	//1  OK
	lcd_write_reg(0xc8);
	lcd_write_data(0x00);
	lcd_write_data(0x26);
	lcd_write_data(0x21);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x1f);
	lcd_write_data(0x65);
	lcd_write_data(0x23);
	lcd_write_data(0x77);
	lcd_write_data(0x00);
	lcd_write_data(0x0f);
	lcd_write_data(0x00);
	//GAMMA SETTING

	lcd_write_reg(0xC0);	//Panel Driving Setting																          
	lcd_write_data(0x00); //1//00  REV  SM  GS
	lcd_write_data(0x3B); //2//NL[5:0]: Sets the number of lines to drive the lcd at an interval of 8 lines. 
	lcd_write_data(0x00); //3//SCN[6:0]
	lcd_write_data(0x02); //4//PTV: Sets the Vcom output in non-display area drive period
	lcd_write_data(0x11); //5//NDL: Sets the source output level in non-display area.  PTG: Sets the scan mode in non-display area.

	lcd_write_reg(0xc6); //Interface Control 
	lcd_write_data(0x83);
	//GAMMA SETTING 

	lcd_write_reg(0xf0); //?
	lcd_write_data(0x01);

	lcd_write_reg(0xE4);//?
	lcd_write_data(0xa0);


	//////倒装设置   NG
	lcd_write_reg(0x36);   
	lcd_write_data(0x8C); //  8C:出来两行   CA：出来一个点


	lcd_write_reg(0x3a);
	lcd_write_data(0x55);

	lcd_write_reg(0xb4);//Display Mode and Frame Memory Write Mode Setting
	lcd_write_data(0x02);
	lcd_write_data(0x00); //?
	lcd_write_data(0x00);
	lcd_write_data(0x01);

	lcd_delay_ms(280);

	lcd_write_reg(0x2a);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x01);
	lcd_write_data(0x3F); //3F

	lcd_write_reg(0x2b);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x01);
	lcd_write_data(0xDf); //DF

	//lcd_write_reg(0x21);
	lcd_write_reg(0x29);	
	lcd_write_reg(0x2c);	

}




