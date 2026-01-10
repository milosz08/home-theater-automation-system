#ifndef UI_H_
#define UI_H_

#include "esp_err.h"

// custom characters
#define PROGRESS_BAR_CHAR         0x01
#define DEGREE_CHAR               0x02

#define REQ_MESS_DURATION_MS      2000  // request success message duration

esp_err_t ui_init(void);
void ui_set_text(const char *line0, const char *line1);
void ui_restore_screen(void);
void ui_show_boot_progress(const char *task_name, int step, int total_steps);
void ui_show_error(const char *msg);
void ui_show_critical_error(const char *msg, esp_err_t err);
void ui_show_network_dashboard(const char *ip_str, const uint8_t *mac_addr);
void ui_show_temp_message(const char *text, uint32_t duration_ms);
void ui_show_temp_cmd_message(const char *text, uint32_t duration_ms);
void ui_show_fixed_temp_cmd_message(const char *text);

#endif // UI_H_
