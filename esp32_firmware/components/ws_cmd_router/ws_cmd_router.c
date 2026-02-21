#include "ws_cmd_router.h"
#include "ws_cmd_handlers.h"

#include <string.h>

#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "WS_CMD_ROUTER";

typedef struct
{
  const char *key;
  const char *friendly_name;
  ws_cmd_cb_t cb;
} cmd_entry_t;

static cmd_entry_t s_commands[MAX_COMMANDS];
static int s_cmd_count = 0;
static ws_cmd_executed_cb_t s_on_executed_cb = NULL;
static ws_cmd_error_cb_t s_on_error_cb = NULL;

static bool ws_cmd_register(const char *key, const char *friendly_name, ws_cmd_cb_t callback)
{
  if (s_cmd_count >= MAX_COMMANDS)
  {
    ESP_LOGE(TAG, "command registry full");
    return false;
  }
  s_commands[s_cmd_count].key = key;
  s_commands[s_cmd_count].friendly_name = friendly_name;
  s_commands[s_cmd_count].cb = callback;
  s_cmd_count++;
  
  ESP_LOGI(TAG, "registered cmd: %s (%s)", key, friendly_name);
  return true;
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t ws_cmd_router_init(ws_cmd_executed_cb_t on_executed_cb, ws_cmd_error_cb_t on_error_cb)
{
  s_on_executed_cb = on_executed_cb;
  ws_cmd_register("cmd_screen_down",  "SCREEN DOWN",  ws_cmd_screen_down);
  ws_cmd_register("cmd_screen_stop",  "SCREEN STOP",  ws_cmd_screen_stop);
  ws_cmd_register("cmd_screen_up",    "SCREEN UP",    ws_cmd_screen_up);
  ws_cmd_register("cmd_get_sys_info", "GET SYS INFO", ws_cmd_get_sys_info);
  s_on_error_cb = on_error_cb;
  return ESP_OK;
}

void ws_cmd_handle(uint8_t *payload, size_t len)
{
  cJSON *root = cJSON_Parse((const char *)payload);
  if (root == NULL) return;

  cJSON *action_item = cJSON_GetObjectItem(root, "action");
  cJSON *val_item = cJSON_GetObjectItem(root, "val");

  if (!cJSON_IsString(action_item))
  {
    cJSON_Delete(root);
    return;
  }
  esp_err_t res;
  bool notify = false;
  for (int i = 0; i < s_cmd_count; i++)
  {
    if (strcmp(s_commands[i].key, action_item->valuestring) == 0)
    {
      res = s_commands[i].cb(val_item, &notify);
      if (res != ESP_OK) ESP_LOGE(TAG, "command %s failed with code %d", s_commands[i].key, res);
      if (notify)
      {
        cJSON *resp = cJSON_CreateObject();
        cJSON_AddStringToObject(resp, "cmd", s_commands[i].key);
        if (res != ESP_OK)
        {
          cJSON_AddNumberToObject(resp, "errorCode", res);
          cJSON_AddStringToObject(resp, "errorName", esp_err_to_name(res));
        }
        else
        {
          cJSON_AddNullToObject(resp, "errorCode");
          cJSON_AddNullToObject(resp, "errorName");
        }
        char *resp_str = cJSON_PrintUnformatted(resp);
        if (resp_str != NULL)
        {
          ws_queue_send(WS_EVENT_TYPE_CMD_INVOCATION, resp_str);
          free(resp_str);
        }
        cJSON_Delete(resp);
      }
      if (s_on_error_cb != NULL && res != ESP_OK) s_on_error_cb(s_commands[i].friendly_name, res);
      else if (s_on_executed_cb != NULL && res == ESP_OK) s_on_executed_cb(s_commands[i].friendly_name);
      break;
    }
  }
  cJSON_Delete(root);
}
