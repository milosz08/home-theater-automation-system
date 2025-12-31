#include "rest_server.h"
#include "rest_handlers.h"
#include "util.h"

#include "esp_log.h"

static const char *TAG = "REST_SERVER";
static httpd_handle_t server = NULL;

// self-signed certificate
extern const unsigned char cacert_pem_start[]   asm("_binary_cacert_pem_start");
extern const unsigned char cacert_pem_end[]     asm("_binary_cacert_pem_end");
extern const unsigned char prvtkey_pem_start[]  asm("_binary_prvtkey_pem_start");
extern const unsigned char prvtkey_pem_end[]    asm("_binary_prvtkey_pem_end");

httpd_err_code_t errors_to_catch[] = {
  HTTPD_404_NOT_FOUND,
  HTTPD_500_INTERNAL_SERVER_ERROR,
  HTTPD_405_METHOD_NOT_ALLOWED,
  HTTPD_411_LENGTH_REQUIRED,
  HTTPD_414_URI_TOO_LONG,
  HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE,
  HTTPD_501_METHOD_NOT_IMPLEMENTED,
  HTTPD_505_VERSION_NOT_SUPPORTED
};

esp_err_t start_rest_server(void)
{
  httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

  conf.servercert = cacert_pem_start;
  conf.servercert_len = cacert_pem_end - cacert_pem_start;
  conf.prvtkey_pem = prvtkey_pem_start;
  conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
  conf.port_secure = atoi(ESP_HTTPS_SERVER_PORT);

  if (conf.servercert_len == 0 || conf.prvtkey_len == 0)
  {
    ESP_LOGE(TAG, "Unable to read cert files!");
    return ESP_FAIL;
  }
  conf.httpd.stack_size = 10240;
  conf.httpd.max_uri_handlers = 10;

  ESP_LOGI(TAG, "Starting HTTPS Server");
  esp_err_t ret = httpd_ssl_start(&server, &conf);

  if (ESP_OK != ret)
  {
    return ret;
  }
  // silent error handlers
  size_t err_count = sizeof(errors_to_catch) / sizeof(errors_to_catch[0]);
  for (int i = 0; i < err_count; i++) {
    httpd_register_err_handler(server, errors_to_catch[i], util_silent_error_handler);
  }
  httpd_uri_t favicon_uri = {
    .uri       = "/favicon.ico",
    .method    = HTTP_GET,
    .handler   = no_content_get_handler,
    .user_ctx  = NULL
  };
  httpd_register_uri_handler(server, &favicon_uri);

  // handlers
  httpd_uri_t health_uri = {
    .uri       = "/api/health",
    .method    = HTTP_GET,
    .handler   = auth_middleware,
    .user_ctx  = no_content_get_handler
  };
  httpd_register_uri_handler(server, &health_uri);

  httpd_uri_t pass_uri = {
    .uri       = "/api/change-password",
    .method    = HTTP_POST,
    .handler   = auth_middleware,
    .user_ctx  = password_post_handler
  };
  httpd_register_uri_handler(server, &pass_uri);

  return ESP_OK;
}

void stop_rest_server(void)
{
  if (server)
  {
    httpd_ssl_stop(server);
  }
}
