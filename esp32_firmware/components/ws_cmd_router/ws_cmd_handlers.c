#include "ws_cmd_handlers.h"
#include "peripheral_control.h"

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
