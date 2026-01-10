#include "io_button_handlers.h"
#include "https_server.h"
#include "nvs_manager.h"
#include "sys_ind.h"
#include "ui.h"
#include "ui_manager.h"

#include "esp_err.h"

// private api ---------------------------------------------------------------------------------------------------------

static volatile bool lcd_backlight_off = false;

// public api ----------------------------------------------------------------------------------------------------------

void on_reset_btn_click(io_input_action_t action)
{
  if (action == BTN_CLICK_LONG)
  {
    char default_password[64] = {0};
    esp_err_t err = nvs_manager_load_str(AUTH_NVS_NS, AUTH_NVS_DEFAULT_KEY, default_password, sizeof(default_password));
    if (err != ESP_OK) return;

    err = nvs_manager_save_str(AUTH_NVS_NS, AUTH_NVS_KEY, default_password);
    if (err != ESP_OK) return;

    ui_show_fixed_temp_cmd_message("Password reset");
  }
}

void on_menu_btn_click(io_input_action_t action)
{
  if (action == BTN_CLICK_LONG) ui_manager_switch_mode();
  else if (action == BTN_CLICK_SHORT) ui_manager_manual_mode();
}

void on_lcd_toggle_backlight(io_input_action_t action)
{
  if (action == BTN_CLICK_SHORT)
  {
    sys_ind_lcd_backlight_set(lcd_backlight_off);
    lcd_backlight_off = !lcd_backlight_off;
  }
}
