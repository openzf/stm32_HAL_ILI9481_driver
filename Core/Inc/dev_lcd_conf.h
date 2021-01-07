#ifndef __DEV_LCD_CONF_H
#define __DEV_LCD_CONF_H

#include "stdio.h"
#include "stdint.h"

// M0 M1 M2 : 0 1 0  -- 16bit
// M0 M1 M2 : 1 1 0  -- 8bit 
// M0 M1 M2 : 1 0 1  -- 3 -line spi
// M0 M1 M2 : 1 1 1  -- 4 -line spi
#define USE_8080_16BIT  0
#if USE_8080_16BIT
// 16 bit  
//LCD地址结构体
typedef struct
{
	volatile uint16_t REG;
	volatile uint16_t RAM;
}FSMC_LCD_TypeDef;
#define FSMC_LCD_BASE_ADDR        ((uint32_t)(0x60000000 | 0X1FFFE))
// A116作为数据命令区分线 
#else
// 8bit
typedef struct
{
	volatile uint8_t REG;
	volatile uint8_t RAM;
} FSMC_LCD_TypeDef;

#define FSMC_LCD_BASE_ADDR        ((uint32_t)(0x60000000 | 0X0FFFF))
#endif

#define FSMC_LCD             ((FSMC_LCD_TypeDef *)FSMC_LCD_BASE_ADDR)

// LCD ID
#define LCD_ILI9481_ID  0x9481


#define LCD_DIR 0 // 0 横屏  1纵屏

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色

#define FONT_USE_BACK_COLOR  0   // 字体是否使用背景色
#define FONT_BACK_COLOR     WHITE //字体背景色 

//GUI颜色
typedef enum
{
	LCD_TYPE_8080=0,
	LCD_TYPE_RGB
}LCD_TYPE;


// 8080液晶参数
typedef struct
{
	uint16_t w_ram_cmd;		//开始写gram指令
	uint16_t set_x_cmd;		//设置x坐标指令
	uint16_t set_y_cmd;		//设置y坐标指令 
	
	void (*set_set_windows)(uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y);
	void (*set_set_cursor)(uint16_t x,uint16_t y);
	void (*write_gram_pre)(void);
	void (*write_reg)(uint16_t value);
	void (*write_data)(uint16_t value);
	void (*write_color)(uint16_t color_value);
	uint16_t (*read_data)(void);
}LCD_8080_Typedef;


typedef struct  
{		 	 
	uint16_t width;		//LCD 宽度
	uint16_t height;	//LCD 高度
	
	uint16_t now_width;		//LCD 宽度
	uint16_t now_height;	//LCD 高度
	
	uint16_t id;			//LCD ID
	uint8_t  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	LCD_TYPE  lcd_type;
	LCD_8080_Typedef lcd_8080;
}LCDTypedef; 

#endif