#include "app_api_handlers.h"
#include "app_api.h"
#include "helper.h"
#include "https_server.h"
#include "nvs_manager.h"

#include <string.h>

#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "API_AUTH";

// public api ----------------------------------------------------------------------------------------------------------

// POST /api/change-password
// content-type: application/x-www-form-urlencoded
// body: old=password&new=password
esp_err_t app_api_auth_change_password(httpd_req_t *req)
{
  char buf[256];
  int ret, remaining = req->content_len;

  if (remaining >= sizeof(buf)) return APP_API_RETURN_ERR(req, 400, ESP_ERR_NO_MEM);

  ret = httpd_req_recv(req, buf, remaining);
  if (ret <= 0)
  {
    if (ret == HTTPD_SOCK_ERR_TIMEOUT) return APP_API_RETURN_ERR(req, 408, ESP_ERR_TIMEOUT);
    return APP_API_RETURN_ERR(req, 500, ESP_FAIL);
  }

  buf[ret] = '\0';

  char old_pass[64];
  char new_pass[64];

  if (httpd_query_key_value(buf, "old", old_pass, sizeof(old_pass)) != ESP_OK ||
      httpd_query_key_value(buf, "new", new_pass, sizeof(new_pass)) != ESP_OK)
  {
    return APP_API_RETURN_ERR(req, 400, ESP_ERR_INVALID_ARG);
  }

  esp_err_t err = nvs_manager_load_str(AUTH_NVS_NS, AUTH_NVS_KEY, buf, sizeof(buf));
  if (err != ESP_OK) return APP_API_RETURN_ERR(req, 500, err);

  size_t len_in = strlen(old_pass);
  size_t len_st = strlen(buf);

  if (len_in != len_st || helper_secure_memcmp(old_pass, buf, len_in) != 0)
  {
    return APP_API_RETURN_ERR(req, 403, APP_ERR_INVALID_PASSWORD);
  }
  err = nvs_manager_save_str(AUTH_NVS_NS, AUTH_NVS_KEY, new_pass);
  if (err != ESP_OK) return APP_API_RETURN_ERR(req, 500, err);

  ESP_LOGI(TAG, "password changed");
  return app_api_return_ok(req);
}
