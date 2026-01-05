#include "ui.h"
#include "ui_icon.h"
#include "lcd_driver.h"

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "UI";

static char saved_line_0[17] = "                ";
static char saved_line_1[17] = "                ";

static TimerHandle_t ui_temp_msg_timer = NULL;
static bool is_error_active = false, is_temp_active = false;

static void ui_restore_timer_callback(TimerHandle_t xTimer)
{
  is_temp_active = false;
  ui_restore_screen();
}

static void ui_set_text_in_row(int row, const char *text)
{
  char *target_buffer = (row == 0) ? saved_line_0 : saved_line_1;
  snprintf(target_buffer, 17, "%-16s", text);

  if (is_error_active || is_temp_active) return;

  lcd_driver_set_cursor(0, row);
  lcd_driver_print(target_buffer);
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t ui_init(void)
{
  esp_err_t err = lcd_driver_init();
  if (err != ESP_OK) return err;

  // load custom icons
  err = lcd_driver_create_char(1, lcd_icon_rectangle_full);
  if (err != ESP_OK) return err;

  err = lcd_driver_create_char(2, lcd_icon_degree);
  if (err != ESP_OK) return err;

  ui_temp_msg_timer = xTimerCreate("UiTmr", pdMS_TO_TICKS(2000), pdFALSE, NULL, ui_restore_timer_callback);

  lcd_driver_clear();
  return ESP_OK;
}

void ui_set_text(const char *line0, const char *line1)
{
  ui_set_text_in_row(0, line0);
  ui_set_text_in_row(1, line1);
}

void ui_restore_screen(void)
{
  is_error_active = false;
  lcd_driver_set_cursor(0, 0);
  lcd_driver_print(saved_line_0);
  lcd_driver_set_cursor(0, 1);
  lcd_driver_print(saved_line_1);
}

void ui_show_boot_progress(const char *task_name, int step, int total_steps)
{
  if (total_steps <= 0) total_steps = 1;

  if (step < 0) step = 0;
  if (step > total_steps) step = total_steps;

  char line0[17];
  char line1[17];

  snprintf(line0, sizeof(line0), "%-16s", task_name);

  int blocks = (step * 16) / total_steps;

  for (int i = 0; i < 16; i++)
  {
    if (i < blocks) line1[i] = PROGRESS_BAR_CHAR;
    else line1[i] = ' ';
  }
  line1[16] = '\0'; // null-terminator

  ui_set_text(line0, line1);
}

void ui_show_error(const char *msg)
{
  is_error_active = true;

  lcd_driver_clear();
  lcd_driver_set_cursor(0, 0);
  lcd_driver_print("ERROR!");

  char buffer[17];
  snprintf(buffer, sizeof(buffer), "%.16s", msg);

  lcd_driver_set_cursor(0, 1);
  lcd_driver_print(buffer);
}

void ui_show_critical_error(const char *msg, esp_err_t err)
{
  char line0[17];
  char line1[17];

  snprintf(line0, sizeof(line0), "ERROR 0x%X", err);
  snprintf(line1, sizeof(line1), "%.16s", msg);

  ui_set_text(line0, line1);
  ESP_LOGE(TAG, "CRITICAL ERROR: %s (0x%x)", msg, err);

  while (1) vTaskDelay(pdMS_TO_TICKS(1000));
}

void ui_show_network_dashboard(const char *ip_str, const uint8_t *mac_addr)
{
  if (ip_str == NULL) ip_str = "0.0.0.0";

  char line0[32];
  char line1[32];
  size_t ip_len = strlen(ip_str);

  // ip
  if (ip_len <= 13) snprintf(line0, sizeof(line0), "IP:%-13s", ip_str);
  else
  {
    int spaces = (16 - (int)ip_len) / 2;
    if (spaces < 0) spaces = 0;
    snprintf(line0, sizeof(line0), "%*s%s", spaces, "", ip_str);
  }
  // mac
  if (mac_addr != NULL)
  {
    snprintf(line1, sizeof(line1), "MAC:%02X%02X%02X%02X%02X%02X",
             mac_addr[0], mac_addr[1], mac_addr[2], 
             mac_addr[3], mac_addr[4], mac_addr[5]);
  }
  else snprintf(line1, sizeof(line1), "MAC:??");

  ui_set_text(line0, line1);
}

void ui_show_temp_message(const char *text, uint32_t duration_ms)
{
  if (text == NULL || is_error_active) return;

  char l0[17] = {0};
  char l1[17] = {0};

  size_t full_len = strlen(text);
  if (full_len <= 16)
  {
    snprintf(l0, sizeof(l0), "%-16.16s", text);
    memset(l1, ' ', 16);
  }
  else // break long message at 2 rows
  {
    memcpy(l0, text, 16);
    l0[16] = '\0';
    snprintf(l1, sizeof(l1), "%-16.16s", text + 16);
  }
  // display without persisting
  lcd_driver_set_cursor(0, 0);
  lcd_driver_print(l0);
  lcd_driver_set_cursor(0, 1);
  lcd_driver_print(l1);

  if (ui_temp_msg_timer != NULL)
  {
    is_temp_active = true;
    xTimerChangePeriod(ui_temp_msg_timer, pdMS_TO_TICKS(duration_ms), 0);
    xTimerStart(ui_temp_msg_timer, 0);
  }
}

void ui_show_temp_cmd_message(const char *text, uint32_t duration_ms)
{
  char formatted_msg[64];
  snprintf(formatted_msg, sizeof(formatted_msg), "CMD: %s", text);
  ui_show_temp_message(formatted_msg, duration_ms);
}
