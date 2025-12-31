#pragma once

#include <stdint.h>

// pcf8574 pin mapping for lcd converter
#define LCD_RS_BIT          0x01
#define LCD_RW_BIT          0x02
#define LCD_EN_BIT          0x04
#define LCD_BL_BIT          0x08  // backlight flag

void lcd_init(void);
void lcd_cmd(uint8_t cmd);
void lcd_print(const char *str);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_clear(void);
