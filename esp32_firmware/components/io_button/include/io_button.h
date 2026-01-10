#ifndef IO_BUTTON_H_
#define IO_BUTTON_H_

#include "io_input.h"

// pcf8574 expander buttons and other io inputs (5-7)
#define PCF_PIN_IN_BTN_RESET          5
#define PCF_PIN_IN_BTN_MENU           6
#define PCF_PIN_IN_BTN_LCD_BACKLIGHT  7

const io_input_config_t *io_button_get_buttons(void);
size_t io_button_get_buttons_count(void);

#endif // IO_BUTTON_H_
