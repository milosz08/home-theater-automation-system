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
      uint32_t min_free_heap = esp_get_minimum_free_heap_size();
      snprintf(json, sizeof(json),
               "{\"temp\":%.1f,\"hum\":%.0f,\"pres\":%.1f,\"ram\":%lu,\"ramMin\":%lu}",
               env.temp_aht,
               env.hum_aht,
               env.pres_bmp,
               (unsigned long)free_heap,
               (unsigned long)min_free_heap);
      ws_queue_send(WS_EVENT_TYPE_ENV_UPDATE, json);
    }
    vTaskDelay(pdMS_TO_TICKS(CYCLIC_TASK_ENV_INTERVAL_MS));
  }
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t cyclic_task_env_init(void)
{
  BaseType_t task_created = xTaskCreate(env_monitor_task_entry, "EnvTask", 4096, NULL, 5, NULL);
  if (task_created != pdPASS) return ESP_ERR_NO_MEM;

  ESP_LOGI(TAG, "environment WS monitor started");
   return ESP_OK;
}
