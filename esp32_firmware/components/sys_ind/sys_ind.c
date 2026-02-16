#include "sys_ind.h"
#include "io_expander.h"
#include "helper.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// private api ---------------------------------------------------------------------------------------------------------

static const char *TAG = "SYS_IND";

static volatile bool is_error_state = false;
static TimerHandle_t act_led_timer = NULL,
                     cmd_led_timer = NULL,
                     buzzer_timer  = NULL;

static volatile int buzzer_beep_counter = 0;
static volatile bool buzzer_state = false;

static const int ALL_LED_PINS[] = {
  PCF_PIN_OUT_LED_CMD,
  PCF_PIN_OUT_LED_ETH_LINK,
  PCF_PIN_OUT_LED_ETH_ACT
};

static void sys_ind_led_set_state(int pin, bool on)
{
  io_expander_set_level(pin, on ? 0 : 1);
}

static void sys_ind_buzzer_set_state(bool on)
{
  io_expander_set_level(PCF_PIN_OUT_BUZZER, on ? 0 : 1);
  buzzer_state = on;
}

static void turn_off_led_callback(TimerHandle_t xTimer)
{
  int pcf_pin = (int)pvTimerGetTimerID(xTimer);
  if (pcf_pin == PCF_PIN_OUT_LED_CMD && is_error_state) return;
  sys_ind_led_set_state(pcf_pin, false);
}

static void buzzer_callback(TimerHandle_t xTimer)
{
  if (buzzer_beep_counter > 0)
  {
    sys_ind_buzzer_set_state(!buzzer_state);
    buzzer_beep_counter--;
  }
  else
  {
    xTimerStop(xTimer, 0);
    sys_ind_buzzer_set_state(false);
  }
}

static void sys_ind_led_heartbeat_task(void *pvParameters)
{
  while (1)
  {
    if (is_error_state)
    {
      sys_ind_led_set_state(PCF_PIN_OUT_LED_CMD, true);
      vTaskDelay(pdMS_TO_TICKS(LED_HEARTBEAT_ERROR_MS));
      sys_ind_led_set_state(PCF_PIN_OUT_LED_CMD, false);
      vTaskDelay(pdMS_TO_TICKS(LED_HEARTBEAT_ERROR_MS));
    }
    else
    {
      if (xTimerIsTimerActive(cmd_led_timer) == pdFALSE)
      {
        sys_ind_led_set_state(PCF_PIN_OUT_LED_CMD, true);
        vTaskDelay(pdMS_TO_TICKS(LED_HOLD_TIME_MS));
        sys_ind_led_set_state(PCF_PIN_OUT_LED_CMD, false);
      }
      vTaskDelay(pdMS_TO_TICKS(LED_HEARTBEAT_TIME_MS));
    }
  }
}

static void sys_ind_led_check(void)
{
  size_t led_count = ARRAY_SIZE(ALL_LED_PINS);

  for (int i = 0; i < led_count; i++) sys_ind_led_set_state(ALL_LED_PINS[i], true);
  vTaskDelay(pdMS_TO_TICKS(500));
  for (int i = 0; i < led_count; i++) sys_ind_led_set_state(ALL_LED_PINS[i], false);
}

// public api ----------------------------------------------------------------------------------------------------------

esp_err_t sys_ind_init(void)
{
  act_led_timer = xTimerCreate("ActTmr", pdMS_TO_TICKS(LED_HOLD_TIME_MS), pdFALSE, (void *)PCF_PIN_OUT_LED_ETH_ACT,
                               turn_off_led_callback);
  cmd_led_timer = xTimerCreate("CmdTmr", pdMS_TO_TICKS(LED_HOLD_TIME_MS), pdFALSE, (void *)PCF_PIN_OUT_LED_CMD,
                               turn_off_led_callback);
  buzzer_timer = xTimerCreate("BuzTmr", pdMS_TO_TICKS(100), pdTRUE, (void *)0, buzzer_callback);

  if (act_led_timer == NULL || cmd_led_timer == NULL || buzzer_timer == NULL) return ESP_ERR_NO_MEM;

  sys_ind_led_check();

  BaseType_t res = xTaskCreate(sys_ind_led_heartbeat_task, "LedHeartbeat", 2048, NULL, 1, NULL);
  if (res != pdPASS) return ESP_ERR_NO_MEM;

  ESP_LOGI(TAG, "System indicators initialized via IO expander");
  return ESP_OK;
}

void sys_ind_led_eth_set_link(bool on)
{
  sys_ind_led_set_state(PCF_PIN_OUT_LED_ETH_LINK, on);
}

void sys_ind_led_eth_packet_activity(void)
{
  if (act_led_timer != NULL)
  {
    sys_ind_led_set_state(PCF_PIN_OUT_LED_ETH_ACT, true);
    xTimerReset(act_led_timer, 0);
  }
}

void sys_ind_led_io_execution(int count)
{
  xTimerStop(cmd_led_timer, 0);
  const TickType_t blink_delay = pdMS_TO_TICKS(LED_BLINK_HOLD_TIME_MS);
  for (int i = 0; i < count; i++)
  {
    sys_ind_led_set_state(PCF_PIN_OUT_LED_CMD, true);
    vTaskDelay(blink_delay);
    sys_ind_led_set_state(PCF_PIN_OUT_LED_CMD, false);
    vTaskDelay(blink_delay);
  }
}

void sys_ind_led_io_cmd_execution(void)
{
  sys_ind_led_io_execution(CMD_LED_COUNT);
}

void sys_ind_led_io_error_execution(void)
{
  sys_ind_led_io_execution(ERROR_LED_COUNT);
}

void sys_ind_set_error(bool active)
{
  is_error_state = active;
}

void sys_ind_lcd_backlight_set(bool on)
{
  io_expander_set_level(PCF_PIN_OUT_LCD_BACKLIGHT, on ? 1 : 0);
}

void sys_ind_buzzer_sound(int count, uint32_t period_ms)
{
  if (buzzer_timer == NULL) return;

  xTimerStop(buzzer_timer, 0);
  buzzer_beep_counter = (count * 2) - 1;

  sys_ind_buzzer_set_state(true);
  if (xTimerChangePeriod(buzzer_timer, pdMS_TO_TICKS(period_ms), 0) != pdPASS)
  {
    xTimerReset(buzzer_timer, 0);
  }
}

void sys_ind_fixed_buzzer_sound(int count)
{
  sys_ind_buzzer_sound(count, BUZZER_PERIOD_MS);
}
