#ifndef __DELAY_H
#define __DELAY_H 			   
  
//////////////////////////////////////////////////////////////////////////////////  
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//使用SysTick的普通计数模式对延迟进行管理(支持OS)
//包括delay_us,delay_ms
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20160418
//修改delay_init里面fac_ms的计算方式，减少delay_ms在非8M整数倍主频下的误差
////////////////////////////////////////////////////////////////////////////////// 

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"

void delay_init(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif
