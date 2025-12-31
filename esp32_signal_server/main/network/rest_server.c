#include "rest_server.h"
#include "rest_handlers.h"
#include "util.h"
#include "storage.h"

#include "esp_log.h"

static const char *TAG = "REST_SERVER";

static httpd_handle_t server = NULL;
static char *cert_buffer = NULL;
static char *key_buffer = NULL;

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

  cert_buffer = read_file_to_buffer("/storage/cacert.pem");
  key_buffer = read_file_to_buffer("/storage/prvtkey.pem");

  if (cert_buffer == NULL || key_buffer == NULL)
  {
    ESP_LOGE(TAG, "Unable to read cert files!");
    free(cert_buffer);
    free(key_buffer);
    cert_buffer = NULL;
    key_buffer = NULL;
    return ESP_FAIL;
  }

  conf.servercert = (const unsigned char*)cert_buffer;
  conf.servercert_len = strlen(cert_buffer);
  conf.prvtkey_pem = (const unsigned char*)key_buffer;
  conf.prvtkey_len = strlen(key_buffer);

  conf.port_secure = g_config.https_port;;

  conf.httpd.stack_size = 10240;
  conf.httpd.max_uri_handlers = 10;

  ESP_LOGI(TAG, "Starting HTTPS server");
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
    server = NULL;
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
}
