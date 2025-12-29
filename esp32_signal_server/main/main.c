#include "eth_w5500.h"
#include "rest_server.h"
#include "reset_button.h"
#include "led_ind.h"

#include "esp_netif.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

static const char *TAG = "MAIN";

static void server_connection_handler(void *arg, esp_event_base_t event_base,
                                      int32_t event_id, void *event_data)
{
  if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP)
  {
    ESP_LOGI(TAG, "Starting web server...");
    ESP_ERROR_CHECK(start_rest_server());
  }
  else if (event_base == ETH_EVENT && event_id == ETHERNET_EVENT_DISCONNECTED)
  {
    ESP_LOGW(TAG, "Stopping web server...");
    stop_rest_server();
  }
}

void app_main(void)
{
  ESP_LOGI(TAG, "System starting...");

  // init flash memory (required by network stack)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  led_init();

  // init global system services
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_ERROR_CHECK(gpio_install_isr_service(0)); // enable interrupts

  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &server_connection_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &server_connection_handler, NULL));

  eth_init_w5500(); // init ethernet
  init_reset_button();
}
