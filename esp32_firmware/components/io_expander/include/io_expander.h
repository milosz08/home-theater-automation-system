#ifndef IO_EXPANDER_H_
#define IO_EXPANDER_H_

#include <stdbool.h>

#include "esp_err.h"

/*! \brief I2C address of the expander (assuming A0-A2 are grounded). */
#define EXPANDER_ADDR             0x20
/*! \brief Pin direction mask. In this setup: pins 7,6,5 are inputs; 4,3,2,1,0 are outputs. */
#define EXPANDER_INIT_PINS_MASK   0xE0  // 11100000
/*! \brief Default state for the shadow register (all high). */
#define EXPANDER_PORT_SHADOW      0xFF  // 1111111

/*! \brief Initializes the IO expander component.
 *
 * Creates the mutex for thread safety, sets up the initial shadow register, and pushes the initial configuration (mask)
 * to the hardware.
 *
 * \retval ESP_OK   Ready to use.
 * \retval ESP_FAIL Mutex creation failed or I2C bus error.
 */
esp_err_t io_expander_init();

/*! \brief Sets a specific pin to high or low.
 *
 * It updates the internal shadow register (to remember other pins' states) and writes the whole byte to the PCF8574.
 * This function is thread-safe.
 *
 * \param pin   The pin number (0-7).
 * \param level 1 for high, 0 for low.
 * 
 * \retval ESP_OK                 Success.
 * \retval ESP_ERR_INVALID_STATE  Driver not initialized.
 */
esp_err_t io_expander_set_level(int pin, int level);

/*! \brief Reads the state of a specific pin.
 *
 * Performs a real I2C read from the device to get the current logic level of the input pin. Thread-safe.
 *
 * \param pin         The pin number (0-7).
 * \param[out] level  Pointer where the result (0 or 1) will be stored.
 *
 * \retval ESP_OK   Read successful.
 * \retval ESP_FAIL I2C failure.
 */
esp_err_t io_expander_get_level(int pin, uint8_t *level);

#endif // IO_EXPANDER_H_
