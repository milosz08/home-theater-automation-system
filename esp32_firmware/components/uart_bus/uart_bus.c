#include "uart_bus.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "UART_BUS";

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t uart_bus_rs485_init(void)
{
  uart_config_t rs485_conf = {
    .baud_rate    = RS485_BAUD_RATE,
    .data_bits    = UART_DATA_8_BITS,
    .parity       = UART_PARITY_DISABLE,
    .stop_bits    = UART_STOP_BITS_1,
    .flow_ctrl    = UART_HW_FLOWCTRL_DISABLE,
    .source_clk   = UART_SCLK_DEFAULT,
  };
  esp_err_t err = uart_param_config(RS485_UART_PORT, &rs485_conf);
  if (err != ESP_OK) return err;
  
  err = uart_set_pin(RS485_UART_PORT, RS485_TX_PIN, RS485_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (err != ESP_OK) return err;

  err = uart_driver_install(RS485_UART_PORT, RS485_BUF_SIZE, 0, 0, NULL, 0);
  if (err != ESP_OK) return err;

  // half-duplex controlled by external chip on RS485 translation module (for ESP32 it is a regular serial)
  err = uart_set_mode(RS485_UART_PORT, UART_MODE_UART); 
  if (err != ESP_OK) return err;

  ESP_LOGI(TAG, "init rs485 interface, tx:%d rx:%d", RS485_TX_PIN, RS485_RX_PIN);
  return ESP_OK;
}

esp_err_t uart_bus_rs485_send(const uint8_t *data, uint16_t len)
{
  int bytes_written = uart_write_bytes(RS485_UART_PORT, (const char*)data, len);

  if (bytes_written == len) return ESP_OK;
  if (bytes_written < 0) return ESP_ERR_INVALID_STATE;

  return ESP_ERR_TIMEOUT;
}

esp_err_t uart_bus_rs485_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms, int *out_len)
{
  int rx_len = uart_read_bytes(RS485_UART_PORT, buf, len, pdMS_TO_TICKS(timeout_ms));
  if (rx_len < 0)
  {
    if (out_len) *out_len = 0;
    return ESP_ERR_INVALID_STATE;
  }
  if (out_len) *out_len = rx_len;
  return (rx_len > 0) ? ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t uart_bus_rs232_init(void) {
  const uart_config_t rs232_conf = {
    .baud_rate    = RS232_BAUD_RATE,
    .data_bits    = UART_DATA_8_BITS,
    .parity       = UART_PARITY_DISABLE,
    .stop_bits    = UART_STOP_BITS_1,
    .flow_ctrl    = UART_HW_FLOWCTRL_DISABLE,
    .source_clk   = UART_SCLK_DEFAULT,
  };
  esp_err_t err = uart_param_config(RS232_UART_PORT, &rs232_conf);
  if (err != ESP_OK) return err;

  err = uart_set_pin(RS232_UART_PORT, RS232_TX_PIN, RS232_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (err != ESP_OK) return err;

  err = uart_driver_install(RS232_UART_PORT, RS232_BUF_SIZE, 0, 0, NULL, 0);
  if (err != ESP_OK) return err;

  ESP_LOGI(TAG, "init rs232 interface, tx:%d rx:%d", RS232_TX_PIN, RS232_RX_PIN);
  return ESP_OK;
}

esp_err_t uart_bus_rs232_send(const uint8_t *data, uint16_t len)
{
  int bytes_written = uart_write_bytes(RS232_UART_PORT, (const char*)data, len);
  
  if (bytes_written == len) return ESP_OK;
  if (bytes_written < 0) return ESP_ERR_INVALID_STATE;

  return ESP_ERR_TIMEOUT;
}

esp_err_t uart_bus_rs232_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms, int *out_len)
{
  int rx_len = uart_read_bytes(RS232_UART_PORT, buf, len, pdMS_TO_TICKS(timeout_ms));
  if (rx_len < 0)
  {
    if (out_len) *out_len = 0;
    return ESP_ERR_INVALID_STATE;
  }
  if (out_len) *out_len = rx_len;
  return (rx_len > 0) ? ESP_OK : ESP_ERR_TIMEOUT;
}
