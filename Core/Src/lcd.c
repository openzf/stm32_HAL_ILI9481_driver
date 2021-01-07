#include "lcd.h"
#include "delay.h"

#include "stm32f407xx.h"
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8;


typedef struct
{
    u16 width;			//LCD 宽度
    u16 height;			//LCD 高度
    u16 id;				//LCD ID
    u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。
    u16	wramcmd;		//开始写gram指令
    u16 setxcmd;		//设置x坐标指令
    u16 setycmd;		//设置y坐标指令
} _lcd_dev;
_lcd_dev lcddev;

int is_use16 = 1;

void LCD_WR_REG(uint16_t regval)
{

	 LCD->LCD_REG=regval;
}

/* LCD write data and register */
void LCD_WR_DATA(uint16_t data)
{

 LCD->LCD_RAM= data;
}

//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{
    vu16 ram;			//防止被优化
    ram=LCD->LCD_RAM;
    return ram;
}



void LCDBlockILI9481(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend)
{
	  printf("start x:%x end d:%x\r\n",Xstart,Xend);
	  printf("start y:%x end d:%x\r\n",Ystart,Yend);
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(Xstart>>8);
    LCD_WR_DATA(Xstart&0xff);
    LCD_WR_DATA(Xend>>8);
    LCD_WR_DATA(Xend&0xff);

    LCD_WR_REG(0x2B);
    LCD_WR_DATA(Ystart>>8);
    LCD_WR_DATA(Ystart&0xff);
    LCD_WR_DATA(Yend>>8);
    LCD_WR_DATA(Yend&0xff);

    
}


void LCD_WriteRAM(u16 RGB_Code)
{
    if(is_use16) {
        LCD->LCD_RAM = RGB_Code;
    } else {
        LCD->LCD_RAM = RGB_Code>>8;
        LCD->LCD_RAM = RGB_Code&0xff;
    }
}







void ILI9481_CMI350IPS_Initial_Code(void)
{

//TEST  cmi35mva 20180510 ok优秀
    int i,j;
    LCD_WR_REG(0x01); //Soft_rese
    delay_ms(220);

    LCD_WR_REG(0x11);
    delay_ms(280);

    LCD_WR_REG(0xd0); //Power_Setting
    LCD_WR_DATA(0x07);//07  VC[2:0] Sets the ratio factor of Vci to generate the reference voltages Vci1
    LCD_WR_DATA(0x44);//41  BT[2:0] Sets the Step up factor and output voltage level from the reference voltages Vci1
    LCD_WR_DATA(0x1E);//1f  17   1C  VRH[3:0]: Sets the factor to generate VREG1OUT from VCILVL
    delay_ms(220);


	
    LCD_WR_REG(0xd1); //VCOM Control
    LCD_WR_DATA(0x00);//00
    LCD_WR_DATA(0x0C);//1A   VCM [6:0] is used to set factor to generate VCOMH voltage from the reference voltage VREG1OUT  15    09
    LCD_WR_DATA(0x1A);//1F   VDV[4:0] is used to set the VCOM alternating amplitude in the range of VREG1OUT x 0.70 to VREG1OUT   1F   18
	delay_ms(20);
    LCD_WR_REG(0xC5);  //Frame Rate
    LCD_WR_DATA(0x03); // 03   02
	delay_ms(20);
    LCD_WR_REG(0xd2);  //Power_Setting for Normal Mode
    LCD_WR_DATA(0x01);  //01
    LCD_WR_DATA(0x11);  //11
			delay_ms(20);
    LCD_WR_REG(0xE4);  //?
    LCD_WR_DATA(0xa0);
    LCD_WR_REG(0xf3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2a);
	delay_ms(20);
//1  OK
    LCD_WR_REG(0xc8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1f);
    LCD_WR_DATA(0x65);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0f);
    LCD_WR_DATA(0x00);
//GAMMA SETTING
	delay_ms(20);
    LCD_WR_REG(0xC0);	//Panel Driving Setting
    LCD_WR_DATA(0x00); //1//00  REV  SM  GS
    LCD_WR_DATA(0x3B); //2//NL[5:0]: Sets the number of lines to drive the LCD at an interval of 8 lines.
    LCD_WR_DATA(0x00); //3//SCN[6:0]
    LCD_WR_DATA(0x02); //4//PTV: Sets the Vcom output in non-display area drive period
    LCD_WR_DATA(0x11); //5//NDL: Sets the source output level in non-display area.  PTG: Sets the scan mode in non-display area.
	delay_ms(20);
    LCD_WR_REG(0xc6); //Interface Control
    LCD_WR_DATA(0x83);
//GAMMA SETTING
	delay_ms(20);
    LCD_WR_REG(0xf0); //?
    LCD_WR_DATA(0x01);
	delay_ms(20);
    LCD_WR_REG(0xE4);//?
    LCD_WR_DATA(0xa0);


	delay_ms(20);
//////倒装设置   NG
    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x8C); //  8C:出来两行   CA：出来一个点

	delay_ms(20);
    LCD_WR_REG(0x3a);
    LCD_WR_DATA(0x55);
	delay_ms(20);
    LCD_WR_REG(0xb4);//Display Mode and Frame Memory Write Mode Setting
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00); //?
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);

    delay_ms(280);

    LCD_WR_REG(0x2a);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x3F); //3F
	delay_ms(20);
    LCD_WR_REG(0x2b);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0xDf); //DF
	delay_ms(20);
//LCD_WR_REG(0x21);
    LCD_WR_REG(0x29);
    LCD_WR_REG(0x2c);

}


void LCD_Clear(u16 color)
{		
		int mcolor = 0;
		int index = 0;

    u32 totalpoint=  300 *300;

//	BlockWrite(0,lcddev.width,0,lcddev.height);
    //LCDBlockILI9481(0,320-1,0,320-1);  //for SPFD5408  ILI9481 OK  20180329
	  LCDBlockILI9481(0,300-1,0,300-1);  //for SPFD5408  ILI9481 OK  20180329
	  //printf("x:%d y:%d",lcddev.width-1,lcddev.height-1);
    for(index=0; index<totalpoint; index++)
    {
		
					LCD_WriteRAM(color);
    }
}






#include "gpio.h"

void LCD_ILI9481SetCursor(u16 Xpos, u16 Ypos)  //ILI9481 20180328_OK
{
	

////9481  20180329 OK FPC在左边
//LCD_WR_8REG(0x36);
//LCD_WR_8DATA(0xEC);//EC  0B

LCD_WR_REG(0X2A);   
LCD_WR_DATA(Xpos>>8);
LCD_WR_DATA(Xpos&0xff);
LCD_WR_DATA(0x01);
LCD_WR_DATA(0x3F);	
	
LCD_WR_REG(0x2B);   
LCD_WR_DATA(Ypos>>8);
LCD_WR_DATA(Ypos&0xff);
LCD_WR_DATA(0x01);
LCD_WR_DATA(0xDF);	

LCD_WR_REG(0x2C);	
	
}
void lcd_cofig(void)
{
	  lcddev.width=320;    //LCD 宽度
    lcddev.height=480;   //LCD 高度
    //	//横向设置
    //lcddev.width=480;    //LCD 宽度
    //lcddev.height=320;   //LCD 高度
    lcddev.setxcmd=0X2A;  //设置x坐标指令2A
    lcddev.setycmd=0X2B;  //设置y坐标指令2B
    lcddev.wramcmd=0X2C;  //开始写gram指令
}
void lcd_init1()
{
//    LCD_WR_REG(0xB9);
//    LCD_WR_DATA(0XFF);
//    LCD_WR_DATA(0X83);
//    LCD_WR_DATA(0X69);

//    LCD_WR_REG(0XBF);
//    lcddev.id=LCD_RD_DATA();	//1dummy read
//    lcddev.id=LCD_RD_DATA();	//2读到0X02
//    lcddev.id=LCD_RD_DATA();   	//3读取04
//    lcddev.id=LCD_RD_DATA();   	//4读取94
//    lcddev.id<<=8;
//    lcddev.id|=LCD_RD_DATA();   //5读取81
////	lcddev.id=LCD_RD_DATA();   	//6读取FF


//    printf("LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组


//    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin,1);
//    delay_ms(50);
//    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin,0);
//    delay_ms(50);
//    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin,1);
//    delay_ms(50);



//	// 参数设置
//9481


	lcd_cofig();

    ILI9481_CMI350IPS_Initial_Code();
		//LCD_Clear(0xFFFF);
    delay_ms(500); // delay 50 ms

		
	for(int i = 0;i<50;i++){
		for(int j = 0;j<10;j++){
			
		u16 Xpos = i;
		u16 Ypos = j;
		LCD_WR_REG(0X2A);   
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0xff);
//	  LCD_WR_DATA(Xpos>>8);
//		LCD_WR_DATA(Xpos&0xff);
			
		LCD_WR_REG(0x2B);   
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0xff);
//		LCD_WR_DATA(Ypos>>8);
//		LCD_WR_DATA(Ypos&0xff);
	
			LCD->LCD_REG=0x2C;
			LCD->LCD_RAM = 0x0000 ;
		}
	}
			
		

}

