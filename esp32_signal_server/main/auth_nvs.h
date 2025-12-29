#pragma once

#include <stddef.h>

#include "esp_err.h"

// password come from CMake
#ifndef ESP_DEFAULT_PASSWORD
#define ESP_DEFAULT_PASSWORD "cinema123"
#endif

#define STORAGE_NAMESPACE   "storage"
#define NVS_KEY_PASS        "server_pass"

void get_current_password(char *buf, size_t len);
esp_err_t set_new_password(const char *new_pass);
esp_err_t reset_password_nvs(void);
