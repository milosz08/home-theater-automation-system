#include "app_api_handlers.h"
#include "ws_dispatcher.h"
#include "ws_cmd_router.h"

#include "cJSON.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// public api ----------------------------------------------------------------------------------------------------------

// GET /ws/control (websocket)
esp_err_t app_api_ws_handler(httpd_req_t *req)
{
  if (req->method == HTTP_GET)
  {
    ws_dispatcher_add_client(req->handle, httpd_req_to_sockfd(req));
    return ESP_OK;
  }

  httpd_ws_frame_t ws_pkt;
  memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
  ws_pkt.type = HTTPD_WS_TYPE_TEXT;

  esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
  if (ret != ESP_OK) return ret;

  // receive data
  if (ws_pkt.len > 0)
  {
    ws_pkt.payload = malloc(ws_pkt.len + 1);
    if (ws_pkt.payload == NULL) return ESP_ERR_NO_MEM;

    ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
    if (ret != ESP_OK)
    {
      free(ws_pkt.payload);
      return ret;
    }
    ws_pkt.payload[ws_pkt.len] = 0;

    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) ws_cmd_handle(ws_pkt.payload, ws_pkt.len);
    free(ws_pkt.payload);
  }

  // close
  if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE)
  {
    ws_dispatcher_remove_client(httpd_req_to_sockfd(req));
    return ESP_OK;
  }

  return ESP_OK;
}
