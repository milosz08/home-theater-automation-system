#include "app_api_handlers.h"
#include "app_api.h"
#include "ui.h"
#include "ui_manager.h"
#include "ws_queue.h"

#include <sys/param.h>

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "API_OTA_UPDATE";

static void ota_reboot_task(void *param)
{
  ui_show_temp_message("Rebooting...", 2000);
  vTaskDelay(pdMS_TO_TICKS(2000));
  esp_restart();
  vTaskDelete(NULL);
}

static void send_ota_progress_event(int current, int total)
{
  char json_buf[128];
  int pct = (total > 0) ? (current * 100 / total) : 0;
  
  snprintf(json_buf, sizeof(json_buf),
           "{\"progress\":%d,\"bytes_processed\":%d,\"bytes_total\":%d}",
           pct, current, total);

  ws_queue_send(WS_EVENT_TYPE_OTA_UPDATE_PROGRESS, json_buf);
}

// public api ----------------------------------------------------------------------------------------------------------

// POST /api/update
// content-type: application/octet-stream (raw .bin file with firmare)
esp_err_t app_api_ota_update(httpd_req_t *req)
{
  ui_manager_suspend();

  char content_type[32];
  if (httpd_req_get_hdr_value_str(req, "Content-Type", content_type, sizeof(content_type)) == ESP_OK &&
      strcmp(content_type, "application/octet-stream") != 0)
  {
    ui_manager_resume();
    return API_RETURN_ERR(req, 400, ESP_ERR_NOT_SUPPORTED);
  }
  esp_ota_handle_t update_handle = 0;
  const esp_partition_t *update_partition = NULL;
  char buf[1024];
  int received;
  int total_len = req->content_len;
  int remaining = total_len;
  int current_len = 0;
  esp_err_t res;

  int last_progress_percent = -1;
  int current_percent = 0;

  update_partition = esp_ota_get_next_update_partition(NULL);
  if (update_partition == NULL)
  {
    ui_manager_resume();
    return API_RETURN_ERR(req, 500, ESP_ERR_NOT_FOUND);
  }
  res = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
  if (res != ESP_OK)
  {
    ui_manager_resume();
    return API_RETURN_ERR(req, 500, res);
  }
  ESP_LOGI(TAG, "starting ota update, size: %d bytes", remaining);
  ui_show_boot_progress("Firmware update", 0, total_len);

  while (remaining > 0)
  {
    received = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
    if (received <= 0)
    {
      if (received == HTTPD_SOCK_ERR_TIMEOUT) continue;
      ui_manager_resume();
      esp_ota_end(update_handle);
      return API_RETURN_ERR(req, 500, ESP_FAIL);
    }
    res = esp_ota_write(update_handle, buf, received);
    if (res != ESP_OK)
    {
      esp_ota_end(update_handle);
      ui_manager_resume();
      return API_RETURN_ERR(req, 500, res);
    }
    remaining -= received;
    current_len += received;

    if (total_len > 0) current_percent = (current_len * 100) / total_len;
    if (current_percent > last_progress_percent)
    {
      last_progress_percent = current_percent;

      if (current_percent % 2 == 0) send_ota_progress_event(current_len, total_len);
      if (current_percent % 5 == 0) ui_show_boot_progress("Firmware update", current_len, total_len);
      if (current_percent % 10 == 0) ESP_LOGI(TAG, "progress: %d%% (%d/%d)", current_percent, current_len, total_len);
    }
  }
  res = esp_ota_end(update_handle);
  if (res != ESP_OK)
  {
    ui_manager_resume();
    return API_RETURN_ERR(req, 500, res);
  }
  res = esp_ota_set_boot_partition(update_partition);
  if (res != ESP_OK)
  {
    ui_manager_resume();
    return API_RETURN_ERR(req, 500, res);
  }
  
  ESP_LOGI(TAG, "ota update success, queuing reboot...");

  ui_show_boot_progress("Complete", total_len, total_len);
  send_ota_progress_event(total_len, total_len);

  xTaskCreate(ota_reboot_task, "ota_reboot", 2048, NULL, 5, NULL);
  return api_return_ok(req);
}
