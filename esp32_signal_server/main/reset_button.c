#include "reset_button.h"
#include "auth_nvs.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static const char *TAG = "RESET_BTN";

static void reset_actions(void)
{
  if (reset_password_nvs() != ESP_OK)
  {
    ESP_LOGE(TAG, "Password reset failure!");
  }
}

void reset_button_task(void *arg) {
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << RESET_BUTTON_GPIO);
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  int hold_counter = 0;
  while (1)
  {
    if (gpio_get_level(RESET_BUTTON_GPIO) == 0)
    {
      hold_counter += 100;
    }
    else
    {
      hold_counter = 0;
    }
    if (hold_counter >= HOLD_TIME_MS)
    {
      reset_actions();
      while(gpio_get_level(RESET_BUTTON_GPIO) == 0)
      {
        vTaskDelay(pdMS_TO_TICKS(100));
      }
      hold_counter = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void init_reset_button(void)
{
  xTaskCreate(reset_button_task, "reset_btn_task", 3072, NULL, 10, NULL);
}
