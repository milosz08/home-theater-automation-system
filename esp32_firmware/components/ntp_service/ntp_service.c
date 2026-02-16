#include "ntp_service.h"

#include <string.h>
#include <sys/time.h>

#include "esp_log.h"
#include "esp_sntp.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "NTP";

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t ntp_service_init(const char *server, const char *timezone)
{
  if (server == NULL || timezone == NULL)
  {
    ESP_LOGE(TAG, "invalid arguments");
    return ESP_ERR_INVALID_ARG;
  }
  ESP_LOGI(TAG, "initializing sntp with server: %s, tz: %s", server, timezone);

  if (setenv("TZ", timezone, 1) != 0)
  {
    ESP_LOGE(TAG, "failed to set tz environment variable");
    return ESP_FAIL;
  }
  tzset();

  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, server);

  if (esp_sntp_enabled()) esp_sntp_stop();
  esp_sntp_init();

  ESP_LOGI(TAG, "waiting for system time to be set...");

  int retry = 0;
  const int retry_count = NTP_SYNC_TIMEOUT_MS / 100;
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) vTaskDelay(pdMS_TO_TICKS(100));

  if (retry == retry_count)
  {
    ESP_LOGE(TAG, "time sync timeout");
    return ESP_ERR_TIMEOUT;
  }

  char date_buf[32], time_buf[32];
  if (ntp_service_get_time_and_date_strs(date_buf, sizeof(date_buf), time_buf, sizeof(time_buf)))
  {
    ESP_LOGI(TAG, "current datetime: %s %s", date_buf, time_buf);
  }
  return ESP_OK;
}

bool ntp_service_get_time_and_date_strs(char *date_buffer, size_t date_len, char *time_buffer, size_t time_len)
{
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);

  if (timeinfo.tm_year < (2020 - 1900)) return false;

  if (date_buffer != NULL && date_len > 0) strftime(date_buffer, date_len, "%a %d.%m.%Y", &timeinfo);
  if (time_buffer != NULL && time_len > 0) strftime(time_buffer, time_len, "%H:%M:%S", &timeinfo);

  return true;
}
