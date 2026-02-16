#include "ws_dispatcher.h"
#include "ws_queue.h"

#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "WS_DISP";

static int s_active_socket = -1;
static httpd_handle_t s_server_handle = NULL;

static void send_ws_json(httpd_handle_t server, int socket, const char *event, const char *data)
{
  size_t len = strlen(event) + strlen(data) + 30;
  char *buffer = malloc(len);
  if (!buffer) return;

  snprintf(buffer, len, "{\"type\":\"%s\",\"data\":%s}", event, data);

  httpd_ws_frame_t ws_pkt;
  memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
  ws_pkt.payload = (uint8_t *)buffer;
  ws_pkt.len = strlen(buffer);
  ws_pkt.type = HTTPD_WS_TYPE_TEXT;

  esp_err_t ret = httpd_ws_send_frame_async(server, socket, &ws_pkt);
  if (ret != ESP_OK) s_active_socket = -1;

  free(buffer);
}

static void ws_task_entry(void *arg)
{
  ws_msg_t msg;
  while(1)
  {
    if (!ws_queue_receive(&msg, portMAX_DELAY)) continue;
    if (s_active_socket != -1 && s_server_handle != NULL)
    {
      send_ws_json(s_server_handle, s_active_socket, msg.event, msg.payload);
    }
    free(msg.payload);
  }
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t ws_dispatcher_init(void)
{
  BaseType_t task_created = xTaskCreate(ws_task_entry, "ws_dispatcher_task", 4096, NULL, 5, NULL);
  if (task_created != pdPASS) return ESP_ERR_NO_MEM;

  ESP_LOGI(TAG, "ws dispatcher init");
  return ESP_OK;
}

void ws_dispatcher_add_client(httpd_handle_t server_handle, int new_sockfd)
{
  s_active_socket = new_sockfd;
  s_server_handle = server_handle;
  ESP_LOGI(TAG, "new ws client registered: %d", new_sockfd);
}

void ws_dispatcher_remove_client(int sockfd)
{
  if (s_active_socket == sockfd)
  {
    s_active_socket = -1;
    ESP_LOGI(TAG, "ws client removed: %d", sockfd);
  }
}
