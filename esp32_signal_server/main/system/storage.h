#pragma once

#include "esp_err.h"

typedef struct {
  char ip[16];
  char gateway[16];
  char netmask[16];
  char dns[16];
  int https_port;
  char default_password[32];
} system_config_t;

extern system_config_t g_config;

esp_err_t init_spiffs_storage(void);
esp_err_t load_system_config(void);
