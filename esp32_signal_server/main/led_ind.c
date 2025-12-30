#include "led_ind.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG_LED_IND = "LED_IND";

static TimerHandle_t act_led_timer = NULL,
                     cmd_led_timer = NULL;

static const gpio_num_t ALL_LEDS[] = {
  PIN_LED_ETH_LINK,
  PIN_LED_ETH_ACT,
  PIN_LED_CMD
};

static void turn_off_led_callback(TimerHandle_t xTimer)
{
  gpio_num_t gpio_pin = (gpio_num_t) pvTimerGetTimerID(xTimer);
  gpio_set_level(gpio_pin, 0);
}

static void led_heartbeat_task(void *pvParameters)
{
  while (1)
  {
    if (xTimerIsTimerActive(cmd_led_timer) == pdFALSE)
    {
      gpio_set_level(PIN_LED_CMD, 1);
      vTaskDelay(pdMS_TO_TICKS(LED_HOLD_TIME_MS));
      gpio_set_level(PIN_LED_CMD, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(LED_HEARTBEAT_TIME_MS));
  }
}

static void check_leds(void)
{
  size_t led_count = sizeof(ALL_LEDS) / sizeof(ALL_LEDS[0]);
  for (int i = 0; i < led_count; i++)
  {
    gpio_set_level(ALL_LEDS[i], 1);
  }
  vTaskDelay(pdMS_TO_TICKS(200));
  for (int i = 0; i < led_count; i++)
  {
    gpio_set_level(ALL_LEDS[i], 0);
  }
}

static void prepare_leds(void)
{
  size_t led_count = sizeof(ALL_LEDS) / sizeof(ALL_LEDS[0]);
  for (int i = 0; i < led_count; i++)
  {
    gpio_reset_pin(ALL_LEDS[i]);
    gpio_set_direction(ALL_LEDS[i], GPIO_MODE_OUTPUT);
    gpio_set_level(ALL_LEDS[i], 0);
  }
}

static void set_led_activity(TimerHandle_t timer, gpio_num_t pin)
{
  if (timer != NULL)
  {
    gpio_set_level(pin, 1);
    xTimerReset(timer, 0);
  }
}

void led_init(void)
{
  prepare_leds();

  TickType_t xDelayTicks = pdMS_TO_TICKS(LED_HOLD_TIME_MS);
  if (xDelayTicks == 0 && LED_HOLD_TIME_MS > 0)
  {
    xDelayTicks = 1;
  }
  act_led_timer = xTimerCreate("ActTmr", xDelayTicks, pdFALSE, (void*)PIN_LED_ETH_ACT, turn_off_led_callback);
  cmd_led_timer = xTimerCreate("CmdTmr", xDelayTicks, pdFALSE, (void*)PIN_LED_CMD, turn_off_led_callback);

  check_leds();

  xTaskCreate(led_heartbeat_task, "led_heartbeat", 2048, NULL, 1, NULL);
  ESP_LOGI(TAG_LED_IND, "LEDs initialized");
}

void led_eth_set_link(bool on)
{
  gpio_set_level(PIN_LED_ETH_LINK, on ? 1 : 0);
}

void led_eth_packet_activity(void)
{
  set_led_activity(act_led_timer, PIN_LED_ETH_ACT);
}

void led_io_cmd_execution(void)
{
  xTimerStop(cmd_led_timer, 0); // turn off heartbeat timer
  const TickType_t blink_delay = pdMS_TO_TICKS(LED_BLINK_HOLD_TIME_MS);
  for (int i = 0; i < 2; i++)
  {
    gpio_set_level(PIN_LED_CMD, 1);
    vTaskDelay(blink_delay);
    gpio_set_level(PIN_LED_CMD, 0);
    vTaskDelay(blink_delay);
  }
}
