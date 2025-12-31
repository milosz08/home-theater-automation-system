#include "rest_handlers.h"
#include "auth_nvs.h"
#include "util.h"
#include "led_ind.h"

#include <string.h>

#include "esp_log.h"

static const char *TAG = "HANDLERS";

static bool is_authorized(httpd_req_t *req) {
  char received_pass[64];
  size_t header_len = httpd_req_get_hdr_value_len(req, AUTH_HEADER);

  if (header_len == 0)
  {
    ESP_LOGW(TAG, "Auth failed: missing header '%s'", AUTH_HEADER);
    return false;
  }
  if (header_len >= sizeof(received_pass))
  {
    ESP_LOGW(TAG, "Auth failed: header value too long");
    return false;
  }
  if (httpd_req_get_hdr_value_str(req, AUTH_HEADER, received_pass, sizeof(received_pass)) != ESP_OK)
  {
    ESP_LOGE(TAG, "Auth failed: error reading header value");
    return false;
  }
  char current_pass[32];
  get_current_password(current_pass, sizeof(current_pass));

  if (strcmp(received_pass, current_pass) == 0)
  {
    return true;
  }
  ESP_LOGW(TAG, "Auth failed: wrong password provided");
  return false;
}

esp_err_t auth_middleware(httpd_req_t *req) {
  if (!is_authorized(req))
  {
    ESP_LOGW(TAG, "Unauthorized access attempt to: %s", req->uri);
    return util_silent_error_handler(req, HTTPD_401_UNAUTHORIZED);
  }
  httpd_handler_func_t real_handler = (httpd_handler_func_t)req->user_ctx;
  if (real_handler)
  {
    return real_handler(req);
  }
  ESP_LOGE(TAG, "Middleware Error: No real handler found in user_ctx");
  return util_silent_error_handler(req, HTTPD_500_INTERNAL_SERVER_ERROR);
}

esp_err_t no_content_get_handler(httpd_req_t *req) {
  return util_silent_response(req);
}

esp_err_t password_post_handler(httpd_req_t *req) {
  char buf[256];
  int ret, remaining = req->content_len;
  if (remaining >= sizeof(buf))
  {
    return util_silent_error_handler(req, HTTPD_400_BAD_REQUEST);
  }
  if ((ret = httpd_req_recv(req, buf, remaining)) <= 0)
  {
    if (ret == HTTPD_SOCK_ERR_TIMEOUT)
    {
      return util_silent_error_handler(req, HTTPD_408_REQ_TIMEOUT);
    }
    return ESP_FAIL;
  }
  buf[ret] = '\0';

  char old_pass_val[64] = {0};
  char new_pass_val[64] = {0};

  if (httpd_query_key_value(buf, "old", old_pass_val, sizeof(old_pass_val)) != ESP_OK ||
      httpd_query_key_value(buf, "new", new_pass_val, sizeof(new_pass_val)) != ESP_OK)
  {
    return util_silent_error_handler(req, HTTPD_400_BAD_REQUEST);
  }
  char current_pass_storage[64]= {0};
  get_current_password(current_pass_storage, sizeof(current_pass_storage));

  if (util_secure_memcmp(old_pass_val, current_pass_storage, sizeof(current_pass_storage)) != 0)
  {
    return util_silent_error_handler(req, HTTPD_403_FORBIDDEN);
  }
  if (set_new_password(new_pass_val) == ESP_OK)
  {
    led_io_cmd_execution();
    ESP_LOGI(TAG, "Password has successfully changed");
    return util_silent_response(req);
  }
  ESP_LOGE(TAG, "Unable to change password");
  return util_silent_error_handler(req, HTTPD_500_INTERNAL_SERVER_ERROR);
}
