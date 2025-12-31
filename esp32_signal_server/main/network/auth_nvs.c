#include "auth_nvs.h"
#include "storage.h"

#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "AUTH_NVS";

void get_current_password(char *buf, size_t len)
{
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
  if (err == ESP_OK)
  {
    size_t required_size = len;
    err = nvs_get_str(my_handle, NVS_KEY_PASS, buf, &required_size);
    nvs_close(my_handle);
  }
  // if not found in NVS or other issue, get hardcored password
  if (err != ESP_OK)
  {
    strncpy(buf, g_config.default_password, len);
    ESP_LOGD(TAG, "Using default password");
  }
}

esp_err_t set_new_password(const char *new_pass)
{
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "Error opening NVS handle!");
    return err;
  }
  err = nvs_set_str(my_handle, NVS_KEY_PASS, new_pass);
  if (err == ESP_OK)
  {
    err = nvs_commit(my_handle);
    ESP_LOGI(TAG, "Password updated in NVS");
  }
  nvs_close(my_handle);
  return err;
}

esp_err_t reset_password_nvs(void)
{
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
  if (err == ESP_OK)
  {
    err = nvs_erase_key(my_handle, NVS_KEY_PASS);
    if (err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND)
    {
      nvs_commit(my_handle);
      ESP_LOGW(TAG, "Password reset to default!");
      err = ESP_OK; // ignore not found error
    }
    nvs_close(my_handle);
  }
  return err;
}
