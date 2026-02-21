#include "ws_cmd_handlers.h"
#include "peripheral_control.h"
#include "ws_queue.h"

#include "esp_app_desc.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "WS_CMD";

// public api ----------------------------------------------------------------------------------------------------------

// user <---> system
esp_err_t ws_cmd_get_sys_info(const cJSON *payload, bool *notify)
{
  const esp_app_desc_t *app_desc = esp_app_get_description();
  uint32_t total_heap = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);

  cJSON *resp_root = cJSON_CreateObject();
  cJSON_AddStringToObject(resp_root, "version", app_desc->version);
  cJSON_AddStringToObject(resp_root, "project", app_desc->project_name);
  cJSON_AddStringToObject(resp_root, "compileDate", app_desc->date);
  cJSON_AddStringToObject(resp_root, "compileTime", app_desc->time);
  cJSON_AddNumberToObject(resp_root, "ramMax", (unsigned long)total_heap);

  char *json_str = cJSON_PrintUnformatted(resp_root);
  if (json_str != NULL)
  {
    ws_queue_send(WS_EVENT_TYPE_SYS_INFO, json_str);
    free(json_str);
  }
  cJSON_Delete(resp_root);

  *notify = false;
  return ESP_OK;
}

// user ----> system
esp_err_t ws_cmd_start_projection(const cJSON *payload, bool *notify)
{
  esp_err_t res;
  ESP_LOGI(TAG, "start projection");
  res = peripheral_control_screen_down();
  res = peripheral_control_projector_on();
  *notify = true;
  return res;
}

// user ----> system
esp_err_t ws_cmd_end_projection(const cJSON *payload, bool *notify)
{
  esp_err_t res;
  ESP_LOGI(TAG, "end projection");
  res = peripheral_control_screen_up();
  res = peripheral_control_projector_off();
  *notify = true;
  return res;
}

// user ----> system
esp_err_t ws_cmd_screen_down(const cJSON *payload, bool *notify)
{
  esp_err_t res;
  ESP_LOGI(TAG, "set projection screen down");
  res = peripheral_control_screen_down();
  *notify = true;
  return res;
}

// user ----> system
esp_err_t ws_cmd_screen_stop(const cJSON *payload, bool *notify)
{
  esp_err_t res;
  ESP_LOGI(TAG, "set projection screen stop");
  res = peripheral_control_screen_stop();
  *notify = true;
  return res;
}

// user ----> system
esp_err_t ws_cmd_screen_up(const cJSON *payload, bool *notify)
{
  esp_err_t res;
  ESP_LOGI(TAG, "set projection screen up");
  res = peripheral_control_screen_up();
  *notify = true;
  return res;
}

// user ----> system
esp_err_t ws_cmd_projector_on(const cJSON *payload, bool *notify)
{
  esp_err_t res;
  ESP_LOGI(TAG, "set projector on");
  res = peripheral_control_projector_on();
  *notify = true;
  return res;
}

// user ----> system
esp_err_t ws_cmd_projector_off(const cJSON *payload, bool *notify)
{
  esp_err_t res;
  ESP_LOGI(TAG, "set projector off");
  res = peripheral_control_projector_off();
  *notify = true;
  return res;
}
