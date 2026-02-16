#include "i2c_bus.h"

#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "I2C_BUS";

static SemaphoreHandle_t bus_mutex = NULL;

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t i2c_bus_init(void)
{
  if (bus_mutex == NULL) bus_mutex = xSemaphoreCreateMutex();

  i2c_config_t conf = {
    .mode               = I2C_MODE_MASTER,
    .sda_io_num         = I2C_MASTER_SDA_IO,
    .scl_io_num         = I2C_MASTER_SCL_IO,
    .sda_pullup_en      = GPIO_PULLUP_ENABLE,
    .scl_pullup_en      = GPIO_PULLUP_ENABLE,
    .master.clk_speed   = I2C_MASTER_FREQ_HZ,
    .clk_flags          = 0,
  };
  ESP_LOGI(TAG, "init i2c, sda:%d scl:%d", I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);

  // configure driver
  esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
  if (err != ESP_OK) return err;

  // install driver
  err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
  if (err != ESP_OK) return err;

  return ESP_OK;
}

esp_err_t i2c_bus_pcf8574_write_byte(uint8_t addr, uint8_t data)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, data, true);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
  i2c_cmd_link_delete(cmd);
  return ret;
}

esp_err_t i2c_bus_pcf8574_read_byte(uint8_t addr, uint8_t *data)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
  i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
  i2c_cmd_link_delete(cmd);
  return ret;
}

esp_err_t i2c_bus_lock(void)
{
  if (bus_mutex == NULL) return ESP_ERR_INVALID_STATE;
  return xSemaphoreTake(bus_mutex, portMAX_DELAY) == pdPASS ? ESP_OK : ESP_FAIL;
}

void i2c_bus_unlock(void)
{
  if (bus_mutex != NULL) xSemaphoreGive(bus_mutex);
}
