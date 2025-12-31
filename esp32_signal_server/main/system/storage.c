#include "storage.h"
#include "util.h"

#include "esp_spiffs.h"
#include "esp_log.h"
#include "cJSON.h"

static const char *TAG = "STORAGE";

system_config_t g_config;

esp_err_t init_spiffs_storage(void)
{
  ESP_LOGI(TAG, "SPIFFS initialization...");

  esp_vfs_spiffs_conf_t conf = {
    .base_path = "/storage",        // mounting point
    .partition_label = "storage",   // same as in partitions.csv
    .max_files = 5,                 // max open files
    .format_if_mount_failed = true
  };

  esp_err_t ret = esp_vfs_spiffs_register(&conf);
  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      ESP_LOGE(TAG, "SPIFFS mounting error");
    }
    else if (ret == ESP_ERR_NOT_FOUND)
    {
      ESP_LOGE(TAG, "SPIFFS partition not found");
    }
    else
    {
      ESP_LOGE(TAG, "Issue during SPIFFS initialization (%s)", esp_err_to_name(ret));
    }
    return ret;
  }
  size_t total = 0, used = 0;
  ret = esp_spiffs_info(conf.partition_label, &total, &used);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Error during getting SPIFFS info (%s)", esp_err_to_name(ret));
  }
  else
  {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }
  return ESP_OK;
}

esp_err_t load_system_config(void)
{
  // safety values
  memset(&g_config, 0, sizeof(system_config_t));

  g_config.https_port = 443;
  strncpy(g_config.default_password, "admin", sizeof(g_config.default_password) - 1);

  char *buffer = read_file_to_buffer("/storage/config.json");
  if (buffer == NULL)
  {
    ESP_LOGE(TAG, "Failed to read config.json from SPIFFS");
    return ESP_FAIL;
  }

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL)
  {
    ESP_LOGE(TAG, "JSON parsing error");
    free(buffer);
    return ESP_FAIL;
  }

  // macro for safe string reading
  #define GET_STRING_SAFE(field, json_obj, key) do { \
    cJSON *item = cJSON_GetObjectItem(json_obj, key); \
    if (cJSON_IsString(item) && (item->valuestring != NULL)) { \
        strncpy(field, item->valuestring, sizeof(field) - 1); \
        field[sizeof(field) - 1] = '\0'; \
    } \
  } while(0)

  GET_STRING_SAFE(g_config.ip, json, "ip");
  GET_STRING_SAFE(g_config.gateway, json, "gateway");
  GET_STRING_SAFE(g_config.netmask, json, "netmask");
  GET_STRING_SAFE(g_config.dns, json, "dns");
  GET_STRING_SAFE(g_config.default_password, json, "default_password");

  cJSON *port_item = cJSON_GetObjectItem(json, "port");
  if (port_item != NULL)
  {
    if (cJSON_IsNumber(port_item))
    {
      g_config.https_port = port_item->valueint;
    }
    else if (cJSON_IsString(port_item))
    {
      g_config.https_port = atoi(port_item->valuestring);
    }
  }
  if (g_config.https_port <= 0 || g_config.https_port > 65535)
  {
    g_config.https_port = 443;
  }
  ESP_LOGI(TAG, "Config loaded");
  ESP_LOGI(TAG, "Network: %s, Port: %d",
                (g_config.ip[0] == '\0') ? "DHCP" : g_config.ip, g_config.https_port);

  cJSON_Delete(json);
  free(buffer);
  return ESP_OK;
}
