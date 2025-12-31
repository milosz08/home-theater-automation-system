#pragma once

#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/uart.h"

// rs485
#define RS485_UART_PORT       UART_NUM_2
#define RS485_TX_PIN          GPIO_NUM_17
#define RS485_RX_PIN          GPIO_NUM_16
#define RS485_EN_PIN          GPIO_NUM_14
#define RS485_BAUD_RATE       9600
#define RS485_BUF_SIZE        2048

// rs232
#define RS232_UART_PORT       UART_NUM_1
#define RS232_TX_PIN          GPIO_NUM_21
#define RS232_RX_PIN          GPIO_NUM_22
#define RS232_BAUD_RATE       115200
#define RS232_BUF_SIZE        1024

esp_err_t rs485_init(void);
int rs485_send(const uint8_t *data, uint16_t len);

esp_err_t rs232_init(void);
int rs232_send(const uint8_t *data, uint16_t len);
int rs232_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms);
