#include "io_button.h"
#include "io_button_handlers.h"

// private api ---------------------------------------------------------------------------------------------------------

static const io_input_config_t s_buttons[] = {
  {
    .pin = PCF_PIN_IN_BTN_RESET,
    .callback = on_reset_btn_click,
    .name = "RESET"
  },
  {
    .pin = PCF_PIN_IN_BTN_MENU,
    .callback = on_menu_btn_click,
    .name = "MENU"
  },
  {
    .pin = PCF_PIN_IN_BTN_LCD_BACKLIGHT,
    .callback = on_lcd_toggle_backlight,
    .name = "BACKLIGHT"
  }
};

// public api ----------------------------------------------------------------------------------------------------------

const io_input_config_t *io_button_get_buttons(void)
{
  return s_buttons;
}

size_t io_button_get_buttons_count(void)
{
  return sizeof(s_buttons) / sizeof(s_buttons[0]);
}
