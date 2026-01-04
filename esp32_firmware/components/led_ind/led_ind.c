#include "led_ind.h"
#include "io_expander.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "LED_IND";

static volatile bool is_error_state = false;
static TimerHandle_t act_led_timer = NULL,
                     cmd_led_timer = NULL;

static const int ALL_LED_PINS[] = {
  PCF_PIN_OUT_LED_CMD,
  PCF_PIN_OUT_LED_ETH_LINK,
  PCF_PIN_OUT_LED_ETH_ACT
};

static void led_ind_set_state(int pin, bool on)
{
  io_expander_set_level(pin, on ? 0 : 1);
}

static void turn_off_led_callback(TimerHandle_t xTimer)
{
  int pcf_pin = (int)pvTimerGetTimerID(xTimer);
  if (pcf_pin == PCF_PIN_OUT_LED_CMD && is_error_state) return;
  led_ind_set_state(pcf_pin, false);
}

static void led_ind_heartbeat_task(void *pvParameters)
{
  while (1)
  {
    if (is_error_state)
    {
      led_ind_set_state(PCF_PIN_OUT_LED_CMD, true);
      vTaskDelay(pdMS_TO_TICKS(LED_HEARTBEAT_ERROR_MS));
      led_ind_set_state(PCF_PIN_OUT_LED_CMD, false);
      vTaskDelay(pdMS_TO_TICKS(LED_HEARTBEAT_ERROR_MS));
    }
    else
    {
      if (xTimerIsTimerActive(cmd_led_timer) == pdFALSE)
      {
        led_ind_set_state(PCF_PIN_OUT_LED_CMD, true);
        vTaskDelay(pdMS_TO_TICKS(LED_HOLD_TIME_MS));
        led_ind_set_state(PCF_PIN_OUT_LED_CMD, false);
      }
      vTaskDelay(pdMS_TO_TICKS(LED_HEARTBEAT_TIME_MS));
    }
  }
}

static void led_ind_check(void)
{
  size_t led_count = sizeof(ALL_LED_PINS) / sizeof(ALL_LED_PINS[0]);

  for (int i = 0; i < led_count; i++) led_ind_set_state(ALL_LED_PINS[i], true);
  vTaskDelay(pdMS_TO_TICKS(500));
  for (int i = 0; i < led_count; i++) led_ind_set_state(ALL_LED_PINS[i], false);
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t led_ind_init(void)
{
  act_led_timer = xTimerCreate("ActTmr", pdMS_TO_TICKS(LED_HOLD_TIME_MS), pdFALSE, (void*)PCF_PIN_OUT_LED_ETH_ACT,
                               turn_off_led_callback);
  cmd_led_timer = xTimerCreate("CmdTmr", pdMS_TO_TICKS(LED_HOLD_TIME_MS), pdFALSE, (void*)PCF_PIN_OUT_LED_CMD,
                               turn_off_led_callback);

  if (act_led_timer == NULL || cmd_led_timer == NULL) return ESP_ERR_NO_MEM;
  led_ind_check();

  BaseType_t res = xTaskCreate(led_ind_heartbeat_task, "led_heartbeat", 2048, NULL, 1, NULL);
  if (res != pdPASS) return ESP_ERR_NO_MEM;

  ESP_LOGI(TAG, "LEDs initialized via IO expander");
  return ESP_OK;
}

void led_ind_eth_set_link(bool on)
{
  led_ind_set_state(PCF_PIN_OUT_LED_ETH_LINK, on);
}

void led_ind_eth_packet_activity(void)
{
  if (act_led_timer != NULL)
  {
    led_ind_set_state(PCF_PIN_OUT_LED_ETH_ACT, true);
    xTimerReset(act_led_timer, 0);
  }
}

void led_ind_io_cmd_execution(void)
{
  xTimerStop(cmd_led_timer, 0);
    
  const TickType_t blink_delay = pdMS_TO_TICKS(LED_BLINK_HOLD_TIME_MS);
  for (int i = 0; i < 2; i++)
  {
    led_ind_set_state(PCF_PIN_OUT_LED_CMD, true);
    vTaskDelay(blink_delay);
    led_ind_set_state(PCF_PIN_OUT_LED_CMD, false);
    vTaskDelay(blink_delay);
  }
}

void led_ind_set_error(bool active)
{
  is_error_state = active;
}
