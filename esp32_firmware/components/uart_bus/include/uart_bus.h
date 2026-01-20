#ifndef UART_BUS_H_
#define UART_BUS_H_

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"

// RS485 Configuration (projection screen)

/*! \brief UART port used for RS485. */
#define RS485_UART_PORT       UART_NUM_2
/*! \brief TX Pin for RS485. */
#define RS485_TX_PIN          GPIO_NUM_17
/*! \brief RX Pin for RS485. */
#define RS485_RX_PIN          GPIO_NUM_16
/*! \brief Baud rate for RS485. */
#define RS485_BAUD_RATE       2400
/*! \brief Buffer size for RS485. */
#define RS485_BUF_SIZE        2048

// RS232 Configuration (projector)

/*! \brief UART port used for RS232. */
#define RS232_UART_PORT       UART_NUM_1
/*! \brief TX Pin for RS232. */
#define RS232_TX_PIN          GPIO_NUM_21
/*! \brief RX Pin for RS232. */
#define RS232_RX_PIN          GPIO_NUM_22
/*! \brief Baud rate for RS232. */
#define RS232_BAUD_RATE       115200
/*! \brief Buffer size for RS232. */
#define RS232_BUF_SIZE        1024

/*! \brief Initializes the RS485 interface.
 *
 * Configures UART2 parameters, pins, and installs the driver.
 *
 * \retval ESP_OK   On success.
 * \retval ESP_FAIL On error during configuring RS485.
 */
esp_err_t uart_bus_rs485_init(void);

/*! \brief Sends raw data over RS485.
 *
 * \param data  Pointer to data buffer.
 * \param len   Number of bytes to send.
 *
 * @return Number of bytes sent or -1 on error.
 */
int uart_bus_rs485_send(const uint8_t *data, uint16_t len);

/*! \brief Reads data from RS485.
 *
 * \param buf         Buffer to store received data.
 * \param len         Max bytes to read.
 * \param timeout_ms  How long to wait for data.
 * 
 * \return Number of bytes read.
 */
int uart_bus_rs485_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms);

/*! \brief Initializes the RS232 interface.
 *
 * Configures UART1 parameters, pins, and installs the driver.
 *
 * \retval ESP_OK   On success.
 * \retval ESP_FAIL On error during configuring RS232.
 */
esp_err_t uart_bus_rs232_init(void);

/*! \brief Sends raw data over RS232.
 * 
 * \param data  Pointer to data buffer.
 * \param len   Number of bytes to send.
 *
 * \return Number of bytes sent.
 */
int uart_bus_rs232_send(const uint8_t *data, uint16_t len);

/*! \brief Reads data from RS232.
 *
 * \param buf         Buffer to store received data.
 * \param len         Max bytes to read.
 * \param timeout_ms  How long to wait for data.
 *
 * \return Number of bytes read.
 */
int uart_bus_rs232_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms);

#endif // UART_BUS_H_
