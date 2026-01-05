#ifndef WS_DISPATCHER_H_
#define WS_DISPATCHER_H_

#include "esp_err.h"
#include "esp_http_server.h"

esp_err_t ws_dispatcher_init(void);
void ws_dispatcher_add_client(httpd_handle_t server_handle, int new_sockfd);
void ws_dispatcher_remove_client(int sockfd);

#endif // WS_DISPATCHER_H_
