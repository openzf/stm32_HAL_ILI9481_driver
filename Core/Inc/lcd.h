#ifndef __LCD_H
#define __LCD_H

#include "stdio.h"
#include "stdint.h"
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8; 


//LCD地址结构体
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;
	    
#define LCD_BASE        ((u32)(0x60000000 | 0X1FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)



//////LCD地址结构体8BIT
//typedef struct
//{
//	u8 LCD_REG;
//	u8 LCD_RAM;
//} LCD_TypeDef;

////#define LCD_BASE        ((u32)(0x60000000 | 0X1FFFE))
//#define LCD_BASE        ((u32)(0x60000000 | 0X0FFFF))
//#define LCD             ((LCD_TypeDef *) LCD_BASE)



void lcd_cofig(void);
void lcd_init1(void);
void LCD_Clear(u16 color);
#endif