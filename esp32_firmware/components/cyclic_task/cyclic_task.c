#include "cyclic_task.h"
#include "env_sensor.h"
#include "ws_queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "CYCLIC_TASK";

void env_monitor_task_entry(void *param)
{
  env_data_t env;
  char json[128];
  while(1)
  {
    if (env_sensors_read_all(&env) == ESP_OK)
    {
      uint32_t free_heap = esp_get_free_heap_size();
      uint32_t uptime_sec = (uint32_t)(esp_timer_get_time() / 1000000);
      snprintf(json, sizeof(json),
               "{\"temp\":%.1f,\"hum\":%.0f,\"pres\":%.1f,\"ram\":%lu,\"up\":%lu}",
               env.temp_aht,
               env.hum_aht,
               env.pres_bmp,
               (unsigned long)free_heap,
               (unsigned long)uptime_sec);
      ws_queue_send(WS_EVENT_TYPE_ENV_UPDATE, json);
    }
    vTaskDelay(pdMS_TO_TICKS(CYCLIC_TASK_ENV_INTERVAL_MS));
  }
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t cyclic_task_env_init(void)
{
  BaseType_t task_created = xTaskCreate(env_monitor_task_entry, "env_task", 4096, NULL, 5, NULL);
  if (task_created != pdPASS) return ESP_ERR_NO_MEM;

  ESP_LOGI(TAG, "environment ws monitor started");
   return ESP_OK;
}
