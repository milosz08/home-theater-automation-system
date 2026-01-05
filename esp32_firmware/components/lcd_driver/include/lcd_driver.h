#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include <stdint.h>

#include "esp_err.h"

// pcf8574 pin mapping for lcd converter
#define LCD_RS_BIT          0x01
#define LCD_RW_BIT          0x02
#define LCD_EN_BIT          0x04
#define LCD_BL_BIT          0x08  // backlight flag

#define LCD_I2C_ADDR        0x27 // I2C address

esp_err_t lcd_driver_init(void);
esp_err_t lcd_driver_create_char(uint8_t location, const uint8_t *charmap);
esp_err_t lcd_driver_cmd(uint8_t cmd);
esp_err_t lcd_driver_print(const char *str);
esp_err_t lcd_driver_set_cursor(uint8_t col, uint8_t row);
esp_err_t lcd_driver_clear(void);

#endif // LCD_DRIVER_H_
