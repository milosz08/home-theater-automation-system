#include "nvs_manager.h"

#include <string.h>

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "NVS_MGR";

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t nvs_manager_init(void)
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
      ESP_LOGW(TAG, "nvs flash recovery needed (erase & re-init)...");
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  if (ret != ESP_OK) return ret;

  ESP_LOGI(TAG, "nvs init");
  return ESP_OK;
}

esp_err_t nvs_manager_save_strings_batch(const char *ns, const nvs_entry_t *entries, size_t count)
{
  if (entries == NULL || count == 0) return ESP_ERR_INVALID_ARG;

  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(ns, NVS_READWRITE, &handle);
  if (err != ESP_OK) return err;

  for (size_t i = 0; i < count; i++) {
    if (entries[i].key == NULL || entries[i].value == NULL) {
      continue;
    }
    err = nvs_set_str(handle, entries[i].key, entries[i].value);
    if (err != ESP_OK) break;
  }

  if (err == ESP_OK) {
    err = nvs_commit(handle);
    if (err == ESP_OK) {
      ESP_LOGI(TAG, "batch save: saved %d items successfully", (int)count);
    }
  }
  nvs_close(handle);
  return err;
}

esp_err_t nvs_manager_save_str(const char *ns, const char *key, const char *val)
{
  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(ns, NVS_READWRITE, &handle);
  if (err != ESP_OK) return err;

  err = nvs_set_str(handle, key, val);
  if (err != ESP_OK) return err;

  err = nvs_commit(handle);

  nvs_close(handle);
  return err;
}

esp_err_t nvs_manager_load_str(const char *ns, const char *key, char *out_buf, size_t max_len)
{
  if (out_buf == NULL || max_len == 0) return ESP_ERR_INVALID_ARG;

  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(ns, NVS_READONLY, &handle);
  if (err != ESP_OK) return err;

  size_t required_size = max_len;
  err = nvs_get_str(handle, key, out_buf, &required_size);

  nvs_close(handle);
  return err;
}
