#include "lcd_driver.h"
#include "i2c_bus.h"

#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "LCD_DRIVER";

static esp_err_t lcd_driver_write_nibble(uint8_t nibble, uint8_t mode)
{
  // compose byte: data + mode (rs) + backlight
  uint8_t data = (nibble & 0xF0) | mode | LCD_BL_BIT;
  esp_err_t err = i2c_bus_lock();
  if (err == ESP_OK)
  {
    err = i2c_bus_pcf8574_write_byte(LCD_I2C_ADDR, data);
    if (err == ESP_OK) err = i2c_bus_pcf8574_write_byte(LCD_I2C_ADDR, data | LCD_EN_BIT);
    if (err == ESP_OK)
    {
      ets_delay_us(1);
      err = i2c_bus_pcf8574_write_byte(LCD_I2C_ADDR, data & ~LCD_EN_BIT);
    }
    i2c_bus_unlock();
  }
  ets_delay_us(20);
  return err;
}

static esp_err_t lcd_driver_send_byte(uint8_t val, uint8_t mode)
{
  esp_err_t err = lcd_driver_write_nibble(val & 0xF0, mode);
  if (err != ESP_OK) return err;
  return lcd_driver_write_nibble((val << 4) & 0xF0, mode);
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t lcd_driver_init(void)
{
  esp_err_t err;

  // startup sequence according to hd44780 datasheet
  vTaskDelay(pdMS_TO_TICKS(50));

  err = lcd_driver_write_nibble(0x30, 0);
  if (err != ESP_OK) return err;
  vTaskDelay(pdMS_TO_TICKS(5));

  err = lcd_driver_write_nibble(0x30, 0);
  if (err != ESP_OK) return err;
  vTaskDelay(pdMS_TO_TICKS(1));

  err = lcd_driver_write_nibble(0x30, 0);
  if (err != ESP_OK) return err;
  vTaskDelay(pdMS_TO_TICKS(1));

  err = lcd_driver_write_nibble(0x20, 0); // switch to 4-bit mode

  // configuration
  lcd_driver_cmd(0x28); // function set: 4-bit, 2 lines, 5x8 font
  lcd_driver_cmd(0x08); // display off
  lcd_driver_clear();   // clear display
  lcd_driver_cmd(0x06); // entry mode set: increment cursor
  lcd_driver_cmd(0x0C); // display on, cursor off, blink off

  ESP_LOGI(TAG, "LCD initialized at 0x%X", LCD_I2C_ADDR);
  return ESP_OK;
}

esp_err_t lcd_driver_create_char(uint8_t location, const uint8_t *charmap)
{
  esp_err_t err;
  location &= 0x7; // only 7 slots
  
  err = lcd_driver_cmd(0x40 | (location << 3));
  if (err != ESP_OK) return err;
  
  for (int i = 0; i < 8; i++)
  {
    err = lcd_driver_send_byte(charmap[i], LCD_RS_BIT);
    if (err != ESP_OK) return err;
  }
  return ESP_OK;
}

esp_err_t lcd_driver_cmd(uint8_t cmd)
{
  return lcd_driver_send_byte(cmd, 0);
}

esp_err_t lcd_driver_print(const char *str)
{
  esp_err_t err = ESP_OK;
  while (*str)
  {
    err = lcd_driver_send_byte((uint8_t)(*str), LCD_RS_BIT);
    if (err != ESP_OK) break;
    str++;
  }
  return err;
}

esp_err_t lcd_driver_clear(void)
{
  esp_err_t err = lcd_driver_cmd(0x01);
  vTaskDelay(pdMS_TO_TICKS(2));
  return err;
}

esp_err_t lcd_driver_set_cursor(uint8_t col, uint8_t row)
{
  uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row > 1) row = 1;
  return lcd_driver_cmd(0x80 | (col + row_offsets[row]));
}
