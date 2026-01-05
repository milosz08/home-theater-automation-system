#ifndef WS_CMD_ROUTER_H_
#define WS_CMD_ROUTER_H_

#include "cJSON.h"
#include "esp_err.h"

typedef void (*ws_cmd_cb_t)(const cJSON *payload);
typedef void (*ws_cmd_executed_cb_t)(const char *friendly_name);

esp_err_t ws_cmd_router_init(ws_cmd_executed_cb_t on_executed_cb);
void ws_cmd_handle(uint8_t *payload, size_t len);

#endif // WS_CMD_ROUTER_H_
