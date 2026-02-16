#ifndef HTTPS_SERVER_H_
#define HTTPS_SERVER_H_

#include <stdint.h>
#include <esp_https_server.h>

#include "esp_err.h"

// Keys can have max 15 characters!
#define AUTH_HEADER           "x-auth-pass"
#define AUTH_NVS_NS           "auth_storage"
#define AUTH_NVS_KEY          "server_pass"
#define AUTH_NVS_DEFAULT_KEY  "def_server_pass"

// Callback type definitions
typedef void (*https_on_running_t)(void);
typedef void (*https_on_stop_t)(void);
typedef void (*https_on_error_t)(esp_err_t err);
typedef void (*https_success_handler_cb_t)(const char *endpoint_name);
typedef void (*https_error_handler_cb_t)(const char *endpoint_name, esp_err_t err);

// Response handler type definitions
typedef void* (*https_ctx_creator_t)(httpd_req_t *req);
typedef esp_err_t (*https_response_handler_t)(httpd_req_t *req, void *ctx, esp_err_t res);

/*! \brief Describes a single URL endpoint. */
typedef struct
{
  const char *uri;                      /*!< The URL path (e.g., "/api/status"). */
  httpd_method_t method;                /*!< HTTP method (GET, POST, etc.). */
  esp_err_t (*handler)(httpd_req_t *r); /*!< Function to handle the request. */
  bool is_public;                       /*!< If `true`, no password header is required. */
  bool is_ws;                           /*!< If `true`, this is a WebSocket endpoint. */
  char *name;                           /*!< Friendly name for logging/debugging. */
} https_endpoint_t;

/*! \brief Main configuration struct for the HTTPS server. */
typedef struct
{
  uint16_t port;                                  /*!< TCP port (usually 443). */
  const char *cacert_path;                        /*!< File path to the server certificate. */
  const char *prvtkey_path;                       /*!< File path to the private key. */

  // Endpoint configuration
  const https_endpoint_t *endpoints;              /*!< Array of defined endpoints. */
  size_t num_endpoints;                           /*!< Number of endpoints in the array. */

  // response handlers
  https_ctx_creator_t create_request_ctx;         /*!< Factory to create a custom context. */
  https_response_handler_t handle_response;       /*!< Finalizer to format and send the HTTP response. */

  // Event callbacks
  https_on_running_t on_running;                  /*!< Called when server starts successfully. */
  https_on_stop_t on_stop;                        /*!< Called when server stops (e.g., network lost). */
  https_on_error_t on_error;                      /*!< Called if startup fails. */
  https_success_handler_cb_t on_request_success;  /*!< Called after any successful request. */
  https_error_handler_cb_t on_request_error;      /*!< Called after any error in request. */
} https_server_config_t;

/*! \brief Sets up the HTTPS server service.
 *
 * It doesn't start the server immediately. Instead, it registers event handlers for network events (IP acquired,
 * Ethernet disconnected). The server automatically starts/stops based on connection status.
 *
 * \param config Pointer to the configuration struct (will be copied).
 *
 * \retval ESP_OK         On successful setup.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       On failure during setup.
 */
esp_err_t https_server_service_init(const https_server_config_t *config);

#endif // HTTPS_SERVER_H_
