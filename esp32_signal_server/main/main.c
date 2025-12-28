#include "eth_w5500.h"

#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include "driver/gpio.h"

static const char *TAG = "MAIN";

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

  // init global system services
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_ERROR_CHECK(gpio_install_isr_service(0)); // enable interrupts

  eth_init_w5500(); // init ethernet
}
