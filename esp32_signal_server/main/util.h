#pragma once

#include <stddef.h>
#include <esp_https_server.h>

#include "esp_err.h"

int util_secure_memcmp(const void *s1, const void *s2, size_t n);
esp_err_t util_silent_error_handler(httpd_req_t *req, httpd_err_code_t err);
esp_err_t util_silent_response(httpd_req_t *req);
void util_show_network_connection(void);
