#ifndef IO_BUTTON_H_
#define IO_BUTTON_H_

#include "io_input.h"

// PCF8574 expander inputs mapping

#define PCF_PIN_IN_BTN_RESET          5   /*!< Pin on the expander for the RESET button. */
#define PCF_PIN_IN_BTN_MENU           6   /*!< Pin on the expander for the MENU button. */
#define PCF_PIN_IN_BTN_LCD_BACKLIGHT  7   /*!< Pin used for LCD backlight control button. */

/*! \brief Gets the list of configured buttons.
 *
 * Returns a pointer to the static array where we defined which pin does what and which callback function to run when
 * it's pressed.
 *
 * \return Pointer to the `io_input_config_t` array.
 */
const io_input_config_t *io_button_get_buttons(void);

/*! \brief Tells you how many buttons are set up.
 *
 * Useful if you need to loop through the button array returned by `io_button_get_buttons`.
 *
 * \return The number of buttons in the list.
 */
size_t io_button_get_buttons_count(void);

#endif // IO_BUTTON_H_
