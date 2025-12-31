#pragma once

#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_SCL_IO           GPIO_NUM_33
#define I2C_MASTER_SDA_IO           GPIO_NUM_27
#define I2C_MASTER_FREQ_HZ          40000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       100

#define EXPANDER_I2C_ADDR           0x20  // 3xL (GND)
#define LCD_I2C_ADDR                0x27  // standard LCD address

esp_err_t i2c_bus_init(void);
esp_err_t pcf8574_write_byte(uint8_t addr, uint8_t data);
