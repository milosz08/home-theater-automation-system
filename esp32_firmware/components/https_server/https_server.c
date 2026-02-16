#include "https_server.h"
#include "helper.h"

#include <string.h>
#include <stdlib.h>

#include "esp_eth.h"
#include "esp_https_server.h"
#include "esp_log.h"
#include "esp_netif.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "HTTPS_SERVER";

static httpd_handle_t server = NULL;
static char *cert_buffer = NULL,
            *key_buffer = NULL;

typedef esp_err_t (*httpd_handler_func_t)(httpd_req_t *req);

typedef struct
{
  https_server_config_t config;
  https_success_handler_cb_t on_success;
  https_error_handler_cb_t on_error;
} server_context_t;

static esp_err_t ws_auth_callback(httpd_req_t *req)
{
  server_context_t *ctx = (server_context_t *)httpd_get_global_user_ctx(req->handle);
  if (ctx && ctx->config.auth_cb) return ctx->config.auth_cb(req) ? ESP_OK : ESP_FAIL;
  return ESP_FAIL;
}

static esp_err_t http_middleware(httpd_req_t *req)
{
  const https_endpoint_t *ep = (const https_endpoint_t *)req->user_ctx;
  if (!ep || !ep->handler) return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, NULL);

  server_context_t *ctx = (server_context_t *)httpd_get_global_user_ctx(req->handle);
  if (!ep->is_public)
  {
    bool authorized = false;
    if (ctx && ctx->config.auth_cb) authorized = ctx->config.auth_cb(req);
    if (!authorized)
    {
      httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
      return ESP_FAIL;
    }
  }
  void *api_ctx = NULL;
  if (ctx && ctx->config.create_request_ctx) api_ctx = ctx->config.create_request_ctx(req);

  void *original_endpoint_ctx = req->user_ctx;
  if (api_ctx) req->user_ctx = api_ctx;

  esp_err_t logic_res = ep->handler(req);
  req->user_ctx = original_endpoint_ctx;

  esp_err_t final_res = logic_res;
  if (ctx->config.handle_response) final_res = ctx->config.handle_response(req, api_ctx, logic_res);

  if (final_res != ESP_OK && ctx->config.on_request_error) ctx->config.on_request_error(ep->name, final_res);
  else if (final_res == ESP_OK && ctx->config.on_request_success) ctx->config.on_request_success(ep->name);

  return ESP_OK;
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
  ESP_LOGI(TAG, "https server stopped");
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
  ctx->config = *cfg;
  ctx->on_success = cfg->on_request_success;
  ctx->on_error = cfg->on_request_error;

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

  ESP_LOGI(TAG, "starting https server on port %d", cfg->port);
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
      ESP_LOGI(TAG, "registering %s uri: %s [%s]",
               ep->is_public ? "public" : "secure",
               ep->uri,
               ep->is_ws ? "ws" : "http");
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
    ESP_LOGI(TAG, "network up -> starting web server...");
    esp_err_t err = https_server_start(cfg);
    if (err == ESP_OK)
    {
      if (cfg->on_running) cfg->on_running();
    }
    else if (cfg->on_error) cfg->on_error(err);
  }
  else if (event_base == ETH_EVENT && event_id == ETHERNET_EVENT_DISCONNECTED)
  {
    ESP_LOGW(TAG, "network down -> stopping web server...");
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

  ESP_LOGI(TAG, "service init, waiting for ip to start server...");
  return ESP_OK;
}
