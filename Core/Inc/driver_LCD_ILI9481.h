#ifndef __DRIVER_LCD_ILI9481_H
#define __DRIVER_LCD_ILI9481_H

#include "dev_lcd_conf.h"

void driver_LCD_ILI9481_init(LCDTypedef *lcd);

void driver_LCD_ILI9481_write_reg(uint16_t value);
void driver_LCD_ILI9481_write_data(uint16_t value);
uint16_t driver_LCD_ILI9481_read_data(void);

void driver_LCD_ILI9481_set_windows(uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y);

#endif