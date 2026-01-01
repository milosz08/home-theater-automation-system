#include "io_input.h"
#include "i2c_bus.h"
#include "io_expander.h"

#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "IO_INPUT";

typedef struct {
  io_input_config_t cfg;
  int hold_timer;
} btn_internal_t;

static btn_internal_t *btns = NULL;
static size_t btns_count = 0;

static void io_input_task(void *arg)
{
  uint8_t port_val = 0xFF;
  const int interval_ms = 50;
  while (1)
  {
    if (i2c_bus_pcf8574_read_byte(EXPANDER_ADDR, &port_val) == ESP_OK)
    {
      for (size_t i = 0; i < btns_count; i++)
      {
        bool is_pressed = !((port_val >> btns[i].cfg.pin) & 0x01);
        if (is_pressed)
        {
          btns[i].hold_timer += interval_ms;
          io_input_action_t action = -1;

          if (btns[i].hold_timer == BTN_TIME_SHORT) action = BTN_CLICK_SHORT;
          else if (btns[i].hold_timer == BTN_TIME_LONG) action = BTN_CLICK_LONG;
          else if (btns[i].hold_timer == BTN_TIME_FACTORY) action = BTN_CLICK_FACTORY;

          if (action != -1 && btns[i].cfg.callback) btns[i].cfg.callback(action);
        }
        else btns[i].hold_timer = 0;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(interval_ms));
  }
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t io_input_init(const io_input_config_t *config, size_t num_buttons)
{
  if (config == NULL || num_buttons == 0) return ESP_ERR_INVALID_ARG;

  btns_count = num_buttons;
  btns = malloc(sizeof(btn_internal_t) * num_buttons);
  if (!btns) return ESP_ERR_NO_MEM;

  for (size_t i = 0; i < num_buttons; i++)
  {
    btns[i].cfg = config[i];
    btns[i].hold_timer = 0;
  }
  xTaskCreate(io_input_task, "io_input_task", 3072, NULL, 5, NULL);

  ESP_LOGI(TAG, "init %d button(s) on I2C expander", (int)num_buttons);
  for(size_t i = 0; i < num_buttons; i++)
  {
    ESP_LOGI(TAG, " -> button '%s' registered on pin %d", config[i].name, config[i].pin);
  }
  return ESP_OK;
}
