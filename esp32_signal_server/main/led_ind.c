#include "led_ind.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"

static const char *TAG_LED_IND = "LED_IND";
static TimerHandle_t act_led_timer = NULL;

static void turn_off_act_led_callback(TimerHandle_t xTimer)
{
  gpio_set_level(PIN_LED_ACT, 0);
}

void led_init(void)
{
  // ETH orange
  gpio_reset_pin(PIN_LED_LINK);
  gpio_set_direction(PIN_LED_LINK, GPIO_MODE_OUTPUT);
  gpio_set_level(PIN_LED_LINK, 0);

  // ETH green
  gpio_reset_pin(PIN_LED_ACT);
  gpio_set_direction(PIN_LED_ACT, GPIO_MODE_OUTPUT);
  gpio_set_level(PIN_LED_ACT, 1);

  act_led_timer = xTimerCreate("LedTimer", pdMS_TO_TICKS(LED_ACT_TICK), pdFALSE, (void*)0, turn_off_act_led_callback);
  ESP_LOGI(TAG_LED_IND, "LEDs initialized");
}

void led_set_link(bool on)
{
  gpio_set_level(PIN_LED_LINK, on ? 1 : 0);
}

void led_blink_activity(void)
{
  if (act_led_timer)
  {
    gpio_set_level(PIN_LED_ACT, 1);
    xTimerReset(act_led_timer, 0);
  }
}
