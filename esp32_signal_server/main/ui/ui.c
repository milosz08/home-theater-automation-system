#include "ui.h"
#include "lcd_driver.h"
#include "lcd_icons.h"
#include "led_ind.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "UI";

static char saved_line_0[17] = "                ";
static char saved_line_1[17] = "                ";

void ui_set_status(int row, const char *text)
{
  char *target_buffer = (row == 0) ? saved_line_0 : saved_line_1;
  snprintf(target_buffer, 17, "%-16s", text);

  lcd_set_cursor(0, row);
  lcd_print(target_buffer);
}

void ui_restore_screen(void)
{
  lcd_set_cursor(0, 0);
  lcd_print(saved_line_0);
  
  lcd_set_cursor(0, 1);
  lcd_print(saved_line_1);
}

void ui_show_screen(const char *line0, const char *line1)
{
  lcd_clear();
  ui_set_status(0, line0);
  ui_set_status(1, line1);
}

void ui_show_boot_progress(const char *task_name, int percent)
{
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;

  char line0[17];
  char line1[17];

  snprintf(line0, sizeof(line0), "%-16s", task_name);

  int blocks = (percent * 16) / 100;
  for (int i = 0; i < 16; i++)
  {
    if (i < blocks)
    {
      line1[i] = PROGRESS_BAR_CHAR;
    }
    else
    {
      line1[i] = ' ';
    }
  }
  line1[16] = '\0';

  ui_show_screen(line0, line1);
}

void ui_show_error(const char *msg)
{
  led_set_error(true);

  lcd_clear();
  lcd_set_cursor(0, 0);
  lcd_print("     ERROR!     ");

  char buffer[17];
  snprintf(buffer, sizeof(buffer), "%.16s", msg);
  
  lcd_set_cursor(0, 1);
  lcd_print(buffer);
}

void ui_show_critical_error(const char *msg, esp_err_t err)
{
  led_set_error(true);

  char line0[17];
  char line1[17];

  snprintf(line0, sizeof(line0), "ERR 0x%X", err);
  snprintf(line1, sizeof(line1), "%.16s", msg);

  ui_show_screen(line0, line1);
  ESP_LOGE(TAG, "CRITICAL ERROR: %s (0x%x)", msg, err);

  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void ui_show_network_dashboard(const esp_netif_ip_info_t *ip_info, const uint8_t *mac_addr)
{
  char line0[32];
  char line1[32];
  char ip_str[16];

  // ip
  snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&ip_info->ip));
  size_t ip_len = strlen(ip_str);
  if (ip_len <= 13)
  {
    snprintf(line0, sizeof(line0), "IP:%-13s", ip_str);
  }
  else
  {
    int spaces = (16 - ip_len) / 2;
    snprintf(line0, sizeof(line0), "%*s%s", spaces, "", ip_str);
  }
  // mac
  snprintf(line1, sizeof(line1), "MAC:%02X%02X%02X%02X%02X%02X",
            mac_addr[0], mac_addr[1], mac_addr[2], 
            mac_addr[3], mac_addr[4], mac_addr[5]);

  ui_show_screen(line0, line1);
}
