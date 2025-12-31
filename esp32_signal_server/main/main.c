#include "eth_w5500.h"
#include "rest_server.h"
#include "reset_button.h"
#include "led_ind.h"
#include "uart_bus.h"
#include "i2c_bus.h"
#include "lcd_driver.h"
#include "ui.h"
#include "util.h"
#include "storage.h"

#include "esp_netif.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

static const char *TAG = "MAIN";

#define CHECK_CRITICAL(x, msg) do { \
  esp_err_t err_rc = (x); \
  if (err_rc != ESP_OK) { \
      ui_show_critical_error(msg, err_rc); \
  } \
} while(0)

static void srv_conn_h(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP)
  {
    ESP_LOGI(TAG, "Starting web server...");
    esp_err_t err = start_rest_server();
    if (err == ESP_OK)
    {
      led_set_error(false);
      ui_restore_screen();
    }
    else
    {
      ESP_LOGE(TAG, "Failed to start server: %s", esp_err_to_name(err));
      led_set_error(true);
      ui_show_error("Server fail");
    }
  }
  else if (event_base == ETH_EVENT && event_id == ETHERNET_EVENT_DISCONNECTED)
  {
    ESP_LOGW(TAG, "Stopping web server...");
    led_set_error(true);
    stop_rest_server();
    ui_show_error("Net: link DOWN");
  }
}

void app_main(void)
{
  ESP_LOGI(TAG, "System starting...");

  // critical: this must be first! nothing else works without i2c
  ESP_ERROR_CHECK(i2c_bus_init());

  lcd_init();
  ui_show_boot_progress("Booting...", 0);
  vTaskDelay(pdMS_TO_TICKS(200));

  // init flash memory (required by network stack)
  ui_show_boot_progress("Init NVS Flash", 10);
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  if (ret != ESP_OK)
  {
    ui_show_critical_error("NVS Fail", ret);
  }

  ui_show_boot_progress("Mounting Storage", 20);
  CHECK_CRITICAL(init_spiffs_storage(), "SPIFFS Fail");

  ui_show_boot_progress("Loading Config", 30);
  CHECK_CRITICAL(load_system_config(), "Config Fail");

  ui_show_boot_progress("Config IO ports", 40);

  led_init();
  CHECK_CRITICAL(rs485_init(), "RS485 init");
  CHECK_CRITICAL(rs232_init(), "RS232 init");

  vTaskDelay(pdMS_TO_TICKS(100));

  // init global system services
  ui_show_boot_progress("Start services", 60);

  CHECK_CRITICAL(esp_netif_init(), "Netif init");
  CHECK_CRITICAL(esp_event_loop_create_default(), "EventLoop");
  CHECK_CRITICAL(gpio_install_isr_service(0), "GPIO ISR"); // enable interrupts

  ui_show_boot_progress("Register events", 75);
  CHECK_CRITICAL(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &srv_conn_h, NULL), "Evt IP reg");
  CHECK_CRITICAL(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &srv_conn_h, NULL), "Evt Eth reg");

  ui_show_boot_progress("Init Ethernet", 85);
  CHECK_CRITICAL(eth_init_w5500(), "Eth init"); // init ethernet

  ui_show_boot_progress("Init buttons", 95);
  init_reset_button();

  ui_show_boot_progress("System ready!", 100);
  vTaskDelay(pdMS_TO_TICKS(1000));

  util_show_network_connection();
  vTaskDelay(pdMS_TO_TICKS(4000));

  lcd_clear();
  ui_show_screen("Status: RUNNING", "Mode: MASTER");

  while (1)
  {
    // TODO
  }
}
