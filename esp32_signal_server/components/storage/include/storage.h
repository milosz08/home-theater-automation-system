#ifndef STORAGE_H_
#define STORAGE_H_

#include "esp_err.h"

typedef struct {
  char ip[16];
  char gateway[16];
  char netmask[16];
  char dns[16];
  int https_port;
  char default_password[64];
} system_config_t;

esp_err_t storage_spiffs_init(void);
esp_err_t storage_load_system_config(system_config_t *config);

#endif
