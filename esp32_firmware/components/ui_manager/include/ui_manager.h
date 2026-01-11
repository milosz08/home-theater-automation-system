#ifndef UI_MANAGER_H_
#define UI_MANAGER_H_

#include "esp_err.h"

esp_err_t ui_manager_init(void);
void ui_manager_switch_mode(void);
void ui_manager_manual_mode(void);

#endif // UI_MANAGER_H_
