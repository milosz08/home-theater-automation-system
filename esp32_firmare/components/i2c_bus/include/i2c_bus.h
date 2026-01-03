#ifndef I2C_BUS_H_
#define I2C_BUS_H_

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"

#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_SCL_IO           GPIO_NUM_33
#define I2C_MASTER_SDA_IO           GPIO_NUM_27
#define I2C_MASTER_FREQ_HZ          40000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       100

esp_err_t i2c_bus_init(void);
esp_err_t i2c_bus_pcf8574_write_byte(uint8_t addr, uint8_t data);
esp_err_t i2c_bus_pcf8574_read_byte(uint8_t addr, uint8_t *data);
esp_err_t i2c_bus_lock(void);
void i2c_bus_unlock(void);

#endif
