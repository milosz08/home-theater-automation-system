#pragma once

#include "esp_err.h"

#ifndef ESP_HTTPS_SERVER_PORT
#define ESP_HTTPS_SERVER_PORT "443"
#endif

esp_err_t start_rest_server(void);
void stop_rest_server(void);
