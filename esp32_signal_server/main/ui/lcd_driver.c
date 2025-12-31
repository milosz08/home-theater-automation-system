#include "lcd_driver.h"
#include "i2c_bus.h"
#include "lcd_icons.h"

#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "rom/ets_sys.h"

static const char *TAG = "LCD_DRIVER";

// sends raw byte to pcf8574
static esp_err_t lcd_pcf_write(uint8_t data)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (LCD_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, data, true);
  i2c_master_stop(cmd);
  
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
  i2c_cmd_link_delete(cmd);
  return ret;
}

static void lcd_write_nibble(uint8_t nibble, uint8_t mode)
{
  // compose byte: data + mode (rs) + backlight
  uint8_t data = (nibble & 0xF0) | mode | LCD_BL_BIT;
  lcd_pcf_write(data);

  lcd_pcf_write(data | LCD_EN_BIT);
  ets_delay_us(1);

  lcd_pcf_write(data & ~LCD_EN_BIT);
  ets_delay_us(20);
}

static void lcd_send_byte(uint8_t val, uint8_t mode)
{
  lcd_write_nibble(val & 0xF0, mode);
  lcd_write_nibble((val << 4) & 0xF0, mode);
}

// location: 0-7 slots
static void lcd_create_char(uint8_t location, uint8_t charmap[])
{
  location &= 0x7;
  lcd_cmd(0x40 | (location << 3));
  for (int i = 0; i < 8; i++)
  {
    lcd_send_byte(charmap[i], LCD_RS_BIT);
  }
}

void lcd_cmd(uint8_t cmd)
{
  lcd_send_byte(cmd, 0);
}

void lcd_print(const char *str)
{
  while (*str)
  {
    lcd_send_byte((uint8_t)(*str), LCD_RS_BIT);
    str++;
  }
}

void lcd_clear(void)
{
  lcd_cmd(0x01);
  vTaskDelay(pdMS_TO_TICKS(2));
}

void lcd_set_cursor(uint8_t col, uint8_t row)
{
  uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if (row > 1)
  {
    row = 1;
  }
  lcd_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_init(void)
{
  // startup sequence according to hd44780 datasheet
  vTaskDelay(pdMS_TO_TICKS(50));

  lcd_write_nibble(0x30, 0);
  vTaskDelay(pdMS_TO_TICKS(5));

  lcd_write_nibble(0x30, 0);
  vTaskDelay(pdMS_TO_TICKS(1));

  lcd_write_nibble(0x30, 0);
  vTaskDelay(pdMS_TO_TICKS(1));

  lcd_write_nibble(0x20, 0); // switch to 4-bit mode

  // configuration
  lcd_cmd(0x28); // function set: 4-bit, 2 lines, 5x8 font
  lcd_cmd(0x08); // display off
  lcd_clear();   // clear display
  lcd_cmd(0x06); // entry mode set: increment cursor
  lcd_cmd(0x0C); // display on, cursor off, blink off

  // custom lcd icons registration
  lcd_create_char(1, lcd_icon_block);

  ESP_LOGI(TAG, "LCD initialized", LCD_I2C_ADDR);
}
