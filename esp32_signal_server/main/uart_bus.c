#include "uart_bus.h"
#include "esp_log.h"

static const char *TAG = "UART_BUS";

void rs485_init(void)
{
  uart_config_t rs485_conf = {
    .baud_rate = RS485_BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };
  ESP_ERROR_CHECK(uart_param_config(RS485_UART_PORT, &rs485_conf));
  ESP_ERROR_CHECK(uart_set_pin(RS485_UART_PORT, RS485_TX_PIN, RS485_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  ESP_ERROR_CHECK(uart_driver_install(RS485_UART_PORT, RS485_BUF_SIZE, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_set_mode(RS485_UART_PORT, UART_MODE_UART)); // do not check RX before send

  // rs485 only for send data, receiving is disabled
  gpio_reset_pin(RS485_EN_PIN);
  gpio_set_direction(RS485_EN_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(RS485_EN_PIN, 1);

  ESP_LOGI(TAG, "Init RS485 interface");
}

int rs485_send(const uint8_t* data, uint16_t len)
{
  return uart_write_bytes(RS485_UART_PORT, (const char*)data, len);;
}

void rs232_init(void) {
  const uart_config_t rs232_conf = {
    .baud_rate = RS232_BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };
  ESP_ERROR_CHECK(uart_param_config(RS232_UART_PORT, &rs232_conf));
  ESP_ERROR_CHECK(uart_set_pin(RS232_UART_PORT, RS232_TX_PIN, RS232_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  ESP_ERROR_CHECK(uart_driver_install(RS232_UART_PORT, RS232_BUF_SIZE, 0, 0, NULL, 0));

  ESP_LOGI(TAG, "Init RS232 interface");
}

int rs232_send(const uint8_t* data, uint16_t len)
{
  return uart_write_bytes(RS232_UART_PORT, (const char*)data, len);
}

int rs232_read(uint8_t* buf, uint32_t len, uint32_t timeout_ms)
{
  return uart_read_bytes(RS232_UART_PORT, buf, len, pdMS_TO_TICKS(timeout_ms));
}
