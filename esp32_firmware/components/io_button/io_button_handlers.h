#ifndef IO_BUTTON_HANDLERS_H_
#define IO_BUTTON_HANDLERS_H_

#include "io_input.h"

/*! \brief Logic for the RESET button.
 *
 * Short press: ignored (safety).
 * Long press: ignored (or maybe soft restart?).
 * Factory press (>5s): wipes the NVS configuration and restarts the ESP.
 *
 * \param action Type of press (short, long, factory).
 */
void on_reset_btn_click(io_input_action_t action);

/*! \brief Logic for the MENU button.
 *
 * Short press: switches the page on the LED display (manual mode).
 * Long press: toggles between auto-scroll and manual display modes.
 *
 * \param action Type of press (short, long).
 */
void on_menu_btn_click(io_input_action_t action);

/*! \brief Logic for the LCD backlight button.
 *
 * Simply toggles the backlight state (On/Off).
 *
 * \param action Type of press (reacts to short press).
 */
void on_lcd_toggle_backlight(io_input_action_t action);

#endif // IO_BUTTON_HANDLERS_H_
