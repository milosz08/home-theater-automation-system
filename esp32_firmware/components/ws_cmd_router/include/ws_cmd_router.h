#ifndef WS_CMD_ROUTER_H_
#define WS_CMD_ROUTER_H_

#include "cJSON.h"
#include "esp_err.h"

typedef void (*ws_cmd_cb_t)(const cJSON *payload);

esp_err_t ws_cmd_router_init(void);
void ws_cmd_handle(uint8_t *payload, size_t len);

#endif
