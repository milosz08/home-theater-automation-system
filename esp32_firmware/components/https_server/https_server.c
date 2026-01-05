#include "https_server.h"
#include "helper.h"
#include "nvs_manager.h"

#include <string.h>
#include <stdlib.h>

#include "esp_eth.h"
#include "esp_https_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "nvs.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "HTTPS_SERVER";

static httpd_handle_t server = NULL;
static char *cert_buffer = NULL,
            *key_buffer = NULL;

typedef esp_err_t (*httpd_handler_func_t)(httpd_req_t *req);

typedef struct
{
  https_post_handler_cb_t on_success;
} server_context_t;

static bool is_authorized(httpd_req_t *req)
{
  char token_buf[64] = {0};
  size_t header_len = httpd_req_get_hdr_value_len(req, AUTH_HEADER);

  if (header_len == 0 || header_len >= sizeof(token_buf)) return false;
  if (httpd_req_get_hdr_value_str(req, AUTH_HEADER, token_buf, sizeof(token_buf)) != ESP_OK) return false;

  char current_pass[64] = {0};
  esp_err_t err = nvs_manager_load_str(AUTH_NVS_NS, AUTH_NVS_KEY, current_pass, sizeof(current_pass));
  if (err != ESP_OK) return false;

  size_t cur_len = strlen(current_pass);
  size_t rcv_len = strlen(token_buf);

  if (cur_len == 0 || cur_len != rcv_len) return false;
  if (helper_secure_memcmp(token_buf, current_pass, cur_len) == 0) return true;

  return false;
}

static esp_err_t ws_auth_callback(httpd_req_t *req)
{
  return is_authorized(req) ? ESP_OK : ESP_FAIL;
}

static esp_err_t http_middleware(httpd_req_t *req)
{
  const https_endpoint_t *ep = (const https_endpoint_t *)req->user_ctx;
  if (!ep || !ep->handler) return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, NULL);

  server_context_t *ctx = (server_context_t *)httpd_get_global_user_ctx(req->handle);
  if (!ep->is_public && !is_authorized(req))
  {
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  esp_err_t res = ep->handler(req);

  if (res == ESP_OK && ctx && ctx->on_success) ctx->on_success(ep->name);
  return res;
}

static void https_server_stop(void)
{
  if (server)
  {
    httpd_ssl_stop(server);
    server = NULL;
  }
  if (cert_buffer)
  {
    free(cert_buffer);
    cert_buffer = NULL;
  }
  if (key_buffer)
  {
    free(key_buffer);
    key_buffer = NULL;
  }
  ESP_LOGI(TAG, "HTTPS server stopped");
}

static esp_err_t https_server_start(const https_server_config_t *cfg)
{
  if (cfg == NULL) return ESP_ERR_INVALID_ARG;

  httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

  cert_buffer = helper_read_file_to_buffer(cfg->cacert_path);
  key_buffer = helper_read_file_to_buffer(cfg->prvtkey_path);

  if (!cert_buffer || !key_buffer)
  {
    ESP_LOGE(TAG, "failed to load certs");
    https_server_stop();
    return ESP_FAIL;
  }

  server_context_t *ctx = malloc(sizeof(server_context_t));
  if (!ctx) return ESP_ERR_NO_MEM;
  ctx->on_success = cfg->on_request_success;

  conf.servercert = (const unsigned char*)cert_buffer;
  conf.servercert_len = strlen(cert_buffer) + 1;
  conf.prvtkey_pem = (const unsigned char*)key_buffer;
  conf.prvtkey_len = strlen(key_buffer) + 1;
  conf.port_secure = cfg->port;

  conf.httpd.max_uri_handlers = cfg->num_endpoints + 2;
  conf.httpd.stack_size = 10240;
  conf.httpd.lru_purge_enable = true;
  conf.httpd.global_user_ctx = ctx;
  conf.httpd.global_user_ctx_free_fn = free; // automatic free after stopped server

  ESP_LOGI(TAG, "starting HTTPS server on port %d", cfg->port);
  esp_err_t ret = httpd_ssl_start(&server, &conf);
  if (ret != ESP_OK)
  {
    free(ctx);
    https_server_stop();
    return ret;
  }
  memset(cert_buffer, 0, strlen(cert_buffer));
  free(cert_buffer);
  cert_buffer = NULL;

  memset(key_buffer, 0, strlen(key_buffer));
  free(key_buffer);
  key_buffer = NULL;

  // endpoints registration
  if (cfg->endpoints && cfg->num_endpoints > 0)
  {
    const https_endpoint_t *endpoints_arr = (const https_endpoint_t *)cfg->endpoints;
    for (size_t i = 0; i < cfg->num_endpoints; i++)
    {
      const https_endpoint_t *ep = &endpoints_arr[i];
      httpd_uri_t uri_conf = {
        .uri          = ep->uri,
        .method       = ep->method,
        .is_websocket = ep->is_ws
      };
      if (ep->is_ws)
      {
        uri_conf.handler = ep->handler;
        uri_conf.user_ctx = NULL;
        if (!ep->is_public) uri_conf.ws_pre_handshake_cb = ws_auth_callback;
      }
      else
      {
        uri_conf.handler = http_middleware;
        uri_conf.user_ctx = (void *)ep;
      }
      ESP_LOGI(TAG, "registering %s URI: %s [%s]",
               ep->is_public ? "PUBLIC" : "SECURE",
               ep->uri,
               ep->is_ws ? "WS" : "HTTP");
      httpd_register_uri_handler(server, &uri_conf);
    }
  }
  return ESP_OK;
}

static void https_server_connection_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  const https_server_config_t *cfg = (const https_server_config_t *)arg;
  if (cfg == NULL) return;

  if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP)
  {
    ESP_LOGI(TAG, "network UP -> starting web server...");
    esp_err_t err = https_server_start(cfg);
    if (err == ESP_OK)
    {
      if (cfg->on_running) cfg->on_running();
    }
    else if (cfg->on_error) cfg->on_error(err);
  }
  else if (event_base == ETH_EVENT && event_id == ETHERNET_EVENT_DISCONNECTED)
  {
    ESP_LOGW(TAG, "network DOWN -> stopping web server...");
    https_server_stop();
    if (cfg->on_stop) cfg->on_stop();
  }
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t https_server_service_init(const https_server_config_t *cfg)
{
  if (cfg == NULL) return ESP_ERR_INVALID_ARG;

  https_server_config_t *persistent_cfg = malloc(sizeof(https_server_config_t));
  if (persistent_cfg == NULL) return ESP_ERR_NO_MEM;

  *persistent_cfg = *cfg;
  esp_err_t err;

  err = esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, https_server_connection_handler, persistent_cfg);
  if (err != ESP_OK)
  {
    free(persistent_cfg);
    return err;
  }
  err = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, https_server_connection_handler,
                                   persistent_cfg);
  if (err != ESP_OK) 
  {
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, https_server_connection_handler);
    free(persistent_cfg);
    return err;
  }

  ESP_LOGI(TAG, "service init, waiting for IP to start server...");
  return ESP_OK;
}
