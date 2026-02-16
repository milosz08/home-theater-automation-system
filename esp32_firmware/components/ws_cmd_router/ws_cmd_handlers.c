#include "ws_cmd_handlers.h"
#include "peripheral_control.h"
#include "ws_queue.h"

#include "esp_app_desc.h"
#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "WS_CMD";

// public api ----------------------------------------------------------------------------------------------------------

void ws_cmd_screen_down(const cJSON *payload)
{
  ESP_LOGI(TAG, "set projection screen DOWN");
  peripheral_control_screen_down();
}

void ws_cmd_screen_stop(const cJSON *payload)
{
  ESP_LOGI(TAG, "set projection screen STOP");
  peripheral_control_screen_stop();
}

void ws_cmd_screen_up(const cJSON *payload)
{
  ESP_LOGI(TAG, "set projection screen UP");
  peripheral_control_screen_up();
}

void ws_cmd_get_sys_info(const cJSON *payload)
{
  const esp_app_desc_t *app_desc = esp_app_get_description();

  cJSON *resp_root = cJSON_CreateObject();
  cJSON_AddStringToObject(resp_root, "version", app_desc->version);
  cJSON_AddStringToObject(resp_root, "project", app_desc->project_name);
  cJSON_AddStringToObject(resp_root, "compile_date", app_desc->date);
  cJSON_AddStringToObject(resp_root, "compile_time", app_desc->time);

  char *json_str = cJSON_PrintUnformatted(resp_root);
  if (json_str != NULL)
  {
    ws_queue_send(WS_EVENT_TYPE_SYS_INFO, json_str);
  }
  cJSON_Delete(resp_root);
}
