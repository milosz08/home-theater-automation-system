#include "ui_manager.h"
#include "eth_w5500.h"
#include "helper.h"
#include "ntp_service.h"
#include "ui.h"
#include "env_sensor.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "UI_MGR";

typedef enum
{
  PAGE_SYSTEM,
  PAGE_CLOCK,
  PAGE_NETWORK,
  PAGE_ENV,
  PAGE_RESOURCES,
  PAGE_COUNT
} ui_page_t;

static ui_page_t current_page = PAGE_SYSTEM;
static bool auto_mode = true;
static bool is_suspended = false;
static TimerHandle_t auto_scroll_timer = NULL;

static void render_system_page(void)
{
  static char time_buf[12], line1[17];
  uint32_t sec = (uint32_t)(esp_timer_get_time() / 1000000);
  helper_format_uptime(sec, time_buf, sizeof(time_buf));
  snprintf(line1, sizeof(line1), "UP: %s", time_buf);
  ui_set_text("System: ACTIVE", line1);
}

static void render_clock_page(void)
{
  static char date_str[17], time_str[17];
  if (ntp_service_get_time_and_date_strs(date_str, sizeof(date_str), time_str, sizeof(time_str)))
  {
    ui_set_text(date_str, time_str);
  }
  else ui_set_text("NTP syncing...", "Please wait");
}

static void render_network_page(void)
{
  static char ip_buf[16] = {0};
  static uint8_t mac_buf[6] = {0};
  eth_w5500_network_info(ip_buf, mac_buf);
  ui_show_network_dashboard(ip_buf, mac_buf);
}

static void render_env_page(void)
{
  env_data_t data;
  static char line0[17], line1[17];
  if (env_sensors_read_all(&data) == ESP_OK)
  {
    snprintf(line0, sizeof(line0), "T:%.1f%cC H:%.0f%%", data.temp_aht, DEGREE_CHAR, data.hum_aht);
    snprintf(line1, sizeof(line1), "P:%.1fhPa", data.pres_bmp);
  }
  else
  {
    snprintf(line0, sizeof(line0), "T:--%cC H:--%%", DEGREE_CHAR);
    snprintf(line1, sizeof(line1), "P:----hPa");
  }
  ui_set_text(line0, line1);
}

static void render_resources_page(void)
{
  uint32_t free_heap = esp_get_free_heap_size() / 1024;
  uint32_t min_free_heap = esp_get_minimum_free_heap_size() / 1024;

  static char line0[17], line1[17];
  snprintf(line0, sizeof(line0), "RAM: %luKB", free_heap);
  snprintf(line1, sizeof(line1), "Min: %luKB", min_free_heap);
  ui_set_text(line0, line1);
}

static void render_current_page(void)
{
  switch (current_page)
  {
    case PAGE_SYSTEM:     render_system_page();     break;
    case PAGE_NETWORK:    render_network_page();    break;
    case PAGE_CLOCK:      render_clock_page();      break;
    case PAGE_ENV:        render_env_page();        break;
    case PAGE_RESOURCES:  render_resources_page();  break;
    default: break;
  }
}

static void auto_scroll_callback(TimerHandle_t xTimer)
{
  if (auto_mode && !is_suspended)
  {
    current_page = (current_page + 1) % PAGE_COUNT;
    render_current_page();
  }
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t ui_manager_init(void)
{
  auto_scroll_timer = xTimerCreate("UiCarousel", pdMS_TO_TICKS(5000), pdTRUE, NULL, auto_scroll_callback);
  if (auto_scroll_timer == NULL) return ESP_ERR_NO_MEM;
  
  xTimerStart(auto_scroll_timer, 0);
  render_current_page();

  ESP_LOGI(TAG, "ui manager init");
  return ESP_OK;
}

void ui_manager_switch_mode(void)
{
  if (is_suspended || auto_scroll_timer == NULL) return;
  auto_mode = !auto_mode;
  if (auto_mode)
  {
    ESP_LOGI(TAG, "mode: auto");
    ui_show_temp_message("MODE: AUTO", 1000);
    xTimerStart(auto_scroll_timer, 0);
  }
  else
  {
    ESP_LOGI(TAG, "mode: manual");
    ui_show_temp_message("MODE: MANUAL", 1000);
    xTimerStop(auto_scroll_timer, 0);
  }
}

void ui_manager_manual_mode(void)
{
  if (is_suspended || auto_scroll_timer == NULL) return;
  current_page = (current_page + 1) % PAGE_COUNT;
  render_current_page();
  if (auto_mode) xTimerReset(auto_scroll_timer, 0);
}

void ui_manager_suspend(void)
{
  is_suspended = true;
  if (auto_scroll_timer) xTimerStop(auto_scroll_timer, 0);
}

void ui_manager_resume(void)
{
  is_suspended = false;
  if (auto_mode && auto_scroll_timer) xTimerStart(auto_scroll_timer, 0);
  render_current_page();
}
