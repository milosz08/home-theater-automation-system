#ifndef HTTPS_SERVER_H_
#define HTTPS_SERVER_H_

#include <stdint.h>
#include <esp_https_server.h>

#include "esp_err.h"

// keys can have max 15 characters!
#define AUTH_HEADER           "x-auth-pass"
#define AUTH_NVS_NS           "auth_storage"
#define AUTH_NVS_KEY          "server_pass"
#define AUTH_NVS_DEFAULT_KEY  "def_server_pass"

typedef void (*https_on_running_t)(void);
typedef void (*https_on_stop_t)(void);
typedef void (*https_on_error_t)(esp_err_t err);
typedef void (*https_post_handler_cb_t)(const char *endpoint_name);

typedef struct {
  const char *uri;
  httpd_method_t method;
  esp_err_t (*handler)(httpd_req_t *r);
  bool is_public;
  char *name;
} https_endpoint_t;

typedef struct {
  uint16_t port;
  const char *cacert_path;
  const char *prvtkey_path;
  // endpoints
  const https_endpoint_t *endpoints;
  size_t num_endpoints;
  // callbacks for event
  https_on_running_t on_running;              // after successfully started
  https_on_stop_t on_stop;                    // after stopped (no network)
  https_on_error_t on_error;                  // after start error
  https_post_handler_cb_t on_request_success; // after successfull request
} https_server_config_t;

esp_err_t https_server_service_init(const https_server_config_t *config);

#endif
