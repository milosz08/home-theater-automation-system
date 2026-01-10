#ifndef IO_BUTTON_HANDLERS_H_
#define IO_BUTTON_HANDLERS_H_

#include "io_input.h"

void on_reset_btn_click(io_input_action_t action);
void on_menu_btn_click(io_input_action_t action);
void on_lcd_toggle_backlight(io_input_action_t action);

#endif // IO_BUTTON_HANDLERS_H_
