#pragma once

#include <esp_https_server.h>

#include "esp_err.h"

#define AUTH_HEADER "X-Auth-Pass"

typedef esp_err_t (*httpd_handler_func_t)(httpd_req_t *req);
esp_err_t auth_middleware(httpd_req_t *req);

esp_err_t no_content_get_handler(httpd_req_t *req);
esp_err_t password_post_handler(httpd_req_t *req);
