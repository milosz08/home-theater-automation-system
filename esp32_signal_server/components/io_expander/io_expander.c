#include "io_expander.h"
#include "i2c_bus.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "IO_EXP";

static uint8_t port_shadow = 0xFF;          // shadow register (remmeber state of all pins)
static uint8_t input_pins_mask = 0x00;      // input mask
static SemaphoreHandle_t io_mutex = NULL;   // mutex for I2C protection

static esp_err_t commit_to_hardware(void)
{
  // io_mutex protect port_shadow and i2c_bus_lock protect physical bus
  if (io_mutex == NULL) return ESP_ERR_INVALID_STATE;
  uint8_t data_to_write = port_shadow | input_pins_mask;

  i2c_bus_lock();
  esp_err_t err = i2c_bus_pcf8574_write_byte(EXPANDER_ADDR, data_to_write);
  i2c_bus_unlock();

  return err;
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t io_expander_init(uint8_t input_mask)
{
  io_mutex = xSemaphoreCreateMutex();
  if (io_mutex == NULL) return ESP_FAIL;

  input_pins_mask = input_mask;
  port_shadow = 0xFF; // init with high state

  xSemaphoreTake(io_mutex, portMAX_DELAY);
  esp_err_t err = commit_to_hardware();
  xSemaphoreGive(io_mutex);

  if (err != ESP_OK)
  {
    vSemaphoreDelete(io_mutex);
    io_mutex = NULL;
    return err;
  }

  ESP_LOGI(TAG, "I2C expander initialized, input mask: 0x%02X", input_mask);
  return ESP_OK;
}

esp_err_t io_expander_set_level(int pin, int level)
{
  if (io_mutex == NULL) return ESP_ERR_INVALID_STATE;

  esp_err_t err;
  xSemaphoreTake(io_mutex, portMAX_DELAY);

  if (level) port_shadow |= (1 << pin);   // set bit (high)
  else port_shadow &= ~(1 << pin);        // clear bit (low)

  err = commit_to_hardware();
  xSemaphoreGive(io_mutex);

  return err;
}

esp_err_t io_expander_get_level(int pin, uint8_t *level)
{
  if (io_mutex == NULL) return ESP_ERR_INVALID_STATE;
  if (level == NULL) return ESP_ERR_INVALID_ARG;

  uint8_t val = 0;
  esp_err_t err;

  xSemaphoreTake(io_mutex, portMAX_DELAY);
  err = i2c_bus_pcf8574_read_byte(EXPANDER_ADDR, &val);
  xSemaphoreGive(io_mutex);

  if (err != ESP_OK) return err;

  *level = (val >> pin) & 0x01;
  return ESP_OK;
}
