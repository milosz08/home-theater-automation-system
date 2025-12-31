#pragma once

#include <stdint.h>

#include "esp_netif_types.h"
#include "esp_err.h"

void ui_set_status(int row, const char *text);
void ui_restore_screen(void);
void ui_show_screen(const char *line0, const char *line1);
void ui_show_boot_progress(const char *task_name, int percent);
void ui_show_error(const char *msg);
void ui_show_critical_error(const char *msg, esp_err_t err);
void ui_show_network_dashboard(const esp_netif_ip_info_t *ip_info, const uint8_t *mac_addr);
