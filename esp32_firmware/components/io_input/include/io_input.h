#ifndef IO_INPUT_H_
#define IO_INPUT_H_

#include "esp_err.h"

#define BTN_TIME_SHORT    50    /*!< Minimum press time (ms) to register a click (debounce). */
#define BTN_TIME_LONG     2000  /*!< Hold time (ms) to register a long click. */
#define BTN_TIME_FACTORY  5000  /*!< Hold time (ms) to trigger a factory reset. */

/*! \brief Types of button interactions. */
typedef enum
{
  BTN_CLICK_SHORT,  /*!< Quick tap. */
  BTN_CLICK_LONG,   /*!< Held for >2s. */
  BTN_CLICK_FACTORY /*!< Held for >5s (nuclear option). */
} io_input_action_t;

/*! \brief Callback function type for button events. */
typedef void (*io_input_callback_t)(io_input_action_t action);

/*! \brief Configuration for a single input button. */
typedef struct
{
  int pin;                      /*!< The pin number on the IO expander (PCF8574). */
  io_input_callback_t callback; /*!< Function to call when event occurs. */
  const char *name;             /*!< Friendly name for logging. */
} io_input_config_t;

/*! \brief Initializes the input monitoring task.
 *
 * Scans the configured pins on the IO Expander periodically to detect state changes and trigger callbacks based on
 * press duration.
 *
 * \param config      Array of button configurations.
 * \param num_buttons Number of buttons in the array.
 *
 * \retval ESP_OK         On success.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       On initialization error.
 */
esp_err_t io_input_init(const io_input_config_t *config, size_t num_buttons);

#endif // IO_INPUT_H_
