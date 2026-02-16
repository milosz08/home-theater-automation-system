#include "storage.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "cJSON.h"
#include "esp_log.h"
#include "esp_littlefs.h"

#include "helper.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "STORAGE";

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t storage_init(void)
{
  esp_vfs_littlefs_conf_t conf = {
    .base_path                = "/storage",    // mounting point
    .partition_label          = "littlefs",    // same as in partitions.csv
    .format_if_mount_failed   = true,
    .dont_mount               = false
  };

  esp_err_t ret = esp_vfs_littlefs_register(&conf);
  if (ret != ESP_OK) return ret;

  size_t total = 0, used = 0;
  ret = esp_littlefs_info(conf.partition_label, &total, &used);
  if (ret != ESP_OK) return ret;
  
  ESP_LOGI(TAG, "partition size: total: %d, used: %d", total, used);
  return ESP_OK;
}

esp_err_t storage_load_system_config(system_config_t *config)
{
  if (config == NULL)
  {
    ESP_LOGE(TAG, "output config pointer is null");
    return ESP_ERR_INVALID_ARG;
  }

  // safety values
  memset(config, 0, sizeof(system_config_t));
  config->https_port = 443;
  strncpy(config->default_password, "admin", sizeof(config->default_password) - 1);

  char *buffer = helper_read_file_to_buffer("/storage/config.json");
  if (buffer == NULL)
  {
    ESP_LOGE(TAG, "failed to read config.json from spiffs (using defaults)");
    return ESP_OK;
  }

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL)
  {
    ESP_LOGE(TAG, "json parsing error");
    free(buffer);
    return ESP_FAIL;
  }

  // macro for safe string reading
  #define GET_STRING_SAFE(target_arr, json_obj, key) do { \
    cJSON *item = cJSON_GetObjectItem(json_obj, key); \
    if (cJSON_IsString(item) && (item->valuestring != NULL)) { \
      strncpy(target_arr, item->valuestring, sizeof(target_arr) - 1); \
      target_arr[sizeof(target_arr) - 1] = '\0'; \
    } \
  } while(0)

  GET_STRING_SAFE(config->ip, json, "ip");
  GET_STRING_SAFE(config->gateway, json, "gateway");
  GET_STRING_SAFE(config->netmask, json, "netmask");
  GET_STRING_SAFE(config->dns, json, "dns");
  GET_STRING_SAFE(config->default_password, json, "default_password");

  cJSON *port_item = cJSON_GetObjectItem(json, "port");
  if (port_item != NULL)
  {
    if (cJSON_IsNumber(port_item)) config->https_port = port_item->valueint;
    else if (cJSON_IsString(port_item)) config->https_port = atoi(port_item->valuestring);
  }

  if (config->https_port <= 0 || config->https_port > 65535) config->https_port = 443;

  ESP_LOGI(TAG, "config loaded");
  ESP_LOGI(TAG, "network: %s, port: %d", (config->ip[0] == '\0') ? "dhcp" : config->ip, config->https_port);

  cJSON_Delete(json);
  free(buffer);
  return ESP_OK;
}
