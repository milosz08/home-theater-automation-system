#ifndef WS_CMD_ROUTER_H_
#define WS_CMD_ROUTER_H_

#include <stdbool.h>

#include "cJSON.h"
#include "esp_err.h"

/*! \brief Callback for processing a parsed JSON command. */
typedef esp_err_t (*ws_cmd_cb_t)(const cJSON *payload, bool *notify);
/*! \brief Callback triggered when a command is successfully routed and invocated with no error. */
typedef void (*ws_cmd_executed_cb_t)(const char *friendly_name);
/*! \brief Callback triggered when a command is successfully routed and invocated with error. */
typedef void (*ws_cmd_error_cb_t)(const char *friendly_name, esp_err_t error_code);

/*! \brief Initializes the websocket command router.
 *
 * Registers the mapping between JSON command strings (e.g., SCREEN_UP) and their C function handlers.
 *
 * \param on_executed_cb  Optional callback to run after a command executes successfully.
 * \param on_error_cb     Optional callback to run after a command executes failure.
 *
 * \retval ESP_OK   On success.
 * \retval ESP_FAIL On websocket command router init failure.
 */
esp_err_t ws_cmd_router_init(ws_cmd_executed_cb_t on_executed_cb, ws_cmd_error_cb_t on_error_cb);

/*! \brief Parses and routes an incoming websocket message.
 *
 * Checks if the payload is valid JSON, looks for a `cmd` field, and finds the matching handler in the registry.
 *
 * \param payload Raw data received from WS.
 * \param len     Length of the data.
 */
void ws_cmd_handle(uint8_t *payload, size_t len);

#endif // WS_CMD_ROUTER_H_
