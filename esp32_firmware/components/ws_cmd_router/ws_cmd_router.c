#include "ws_cmd_router.h"
#include "ws_cmd_handlers.h"

#include <string.h>

#include "esp_log.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "WS_CMD_ROUTER";

typedef struct
{
  const char *name;
  ws_cmd_cb_t cb;
} cmd_entry_t;

static cmd_entry_t s_commands[MAX_COMMANDS];
static int s_cmd_count = 0;

static bool ws_cmd_register(const char *command_name, ws_cmd_cb_t callback)
{
  if (s_cmd_count >= MAX_COMMANDS)
  {
    ESP_LOGE(TAG, "command registry full!");
    return false;
  }
  s_commands[s_cmd_count].name = command_name;
  s_commands[s_cmd_count].cb = callback;
  s_cmd_count++;
  
  ESP_LOGI(TAG, "registered command: %s", command_name);
  return true;
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t ws_cmd_router_init(void)
{
  ws_cmd_register("cmd_screen_down",  ws_cmd_screen_down);
  ws_cmd_register("cmd_screen_stop",  ws_cmd_screen_stop);
  ws_cmd_register("cmd_screen_up",    ws_cmd_screen_up);
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
  for (int i = 0; i < s_cmd_count; i++)
  {
    if (strcmp(s_commands[i].name, action_item->valuestring) == 0)
    {
      s_commands[i].cb(val_item);
      break;
    }
  }
  cJSON_Delete(root);
}
