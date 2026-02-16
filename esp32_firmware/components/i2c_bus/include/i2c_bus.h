#ifndef I2C_BUS_H_
#define I2C_BUS_H_

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"

#define I2C_MASTER_NUM              I2C_NUM_0   /*!< I2C port number used for the master interface. */
#define I2C_MASTER_SCL_IO           GPIO_NUM_33 /*!< GPIO pin for the clock (SCL) line. */
#define I2C_MASTER_SDA_IO           GPIO_NUM_27 /*!< GPIO pin for the data (SDA) line. */
#define I2C_MASTER_FREQ_HZ          40000       /*!< Bus frequency in Hz (standard 40 kHz for this setup). */
#define I2C_MASTER_TX_BUF_DISABLE   0           /*!< Disable TX buffer (not needed here). */
#define I2C_MASTER_RX_BUF_DISABLE   0           /*!< Disable RX buffer (not needed here). */
#define I2C_MASTER_TIMEOUT_MS       100         /*!< Default timeout for I2C operations in milliseconds. */

/*! \brief Sets up the I2C Master bus.
 *
 * Takes care of configuring the GPIO pins, setting the clock speed, and installing the drivers. It also creates a
 * mutex to make sure we don't crash when multiple tasks try to use the bus at the same time.
 *
 * \retval ESP_OK Everything went fine.
 * \retval ESP_FAIL Something went wrong during config or driver installation.
 */
esp_err_t i2c_bus_init(void);

/*! \brief Helper to write a single byte to a PCF8574 expander.
 *
 * Wraps the whole transaction: sends START, the address with WRITE bit, the actual byte, and then STOP. Handy for
 * quick IO toggling.
 * 
 * \param addr The 7-bit address of the device.
 * \param data The byte you want to push out.
 *
 * \retval ESP_OK   Byte sent successfully.
 * \retval ESP_FAIL Communication failed (e.g., timeout or NACK).
 */
esp_err_t i2c_bus_pcf8574_write_byte(uint8_t addr, uint8_t data);

/*! \brief Helper to read a single byte from a PCF8574 expander.
 *
 * Similar to write, but for reading. Sends address with READ bit, gets the byte, sends NACK (to say "I'm done"), and
 * stops.
 *
 * \param addr      The 7-bit address of the device.
 * \param[out] data Pointer where we'll store the received byte.
 *
 * \retval ESP_OK   Got the data.
 * \retval ESP_FAIL Bus trouble.
 */
esp_err_t i2c_bus_pcf8574_read_byte(uint8_t addr, uint8_t *data);

/*! \brief Tries to lock the I2C bus for exclusive access.
 *
 * Call this before running a complex transaction if you want to be sure no other task interrupts you. It waits forever
 * (portMAX_DELAY) until the bus is free.
 *
 * \retval ESP_OK                 You have the lock.
 * \retval ESP_FAIL               Couldn't grab the mutex (weird, but possible).
 * \retval ESP_ERR_INVALID_STATE  Mutex wasn't created yet (did you call init?).
 */
esp_err_t i2c_bus_lock(void);

/*! \brief Unlocks the I2C bus.
 *
 * Releases the mutex so other tasks can use the bus again. Don't forget to call this after you're done, or the bus will
 * hang forever!
 */
void i2c_bus_unlock(void);

#endif // I2C_BUS_H_
