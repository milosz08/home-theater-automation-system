#ifndef APP_API_HANDLERS_H_
#define APP_API_HANDLERS_H_

#include "esp_http_server.h"
#include "esp_err.h"

// auth
esp_err_t app_api_auth_change_password(httpd_req_t *req);

// system
esp_err_t app_api_system_health(httpd_req_t *req);

#endif
