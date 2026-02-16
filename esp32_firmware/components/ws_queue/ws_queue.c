#include "ws_queue.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "WS_QUEUE";
static QueueHandle_t s_ws_queue = NULL;

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t ws_queue_init(void)
{
  if (s_ws_queue != NULL) return ESP_OK;

  s_ws_queue = xQueueCreate(WS_QUEUE_SIZE, sizeof(ws_msg_t));
  if (s_ws_queue == NULL) return ESP_ERR_NO_MEM;

  ESP_LOGI(TAG, "ws queue init");
  return ESP_OK;
}

bool ws_queue_send(const char *event_name, const char *json_data)
{
  if (s_ws_queue == NULL) return false;
  ws_msg_t msg;

  strncpy(msg.event, event_name, WS_EVENT_NAME_MAX - 1);
  msg.event[WS_EVENT_NAME_MAX - 1] = '\0';

  msg.payload = strdup(json_data);
  if (msg.payload == NULL) return false;

  if (xQueueSend(s_ws_queue, &msg, 0) != pdTRUE)
  {
    free(msg.payload);
    return false;
  }
  return true;
}

bool ws_queue_receive(ws_msg_t *out_msg, uint32_t ticks_to_wait)
{
  if (s_ws_queue == NULL) return false;
  return xQueueReceive(s_ws_queue, out_msg, ticks_to_wait) == pdTRUE;
}
