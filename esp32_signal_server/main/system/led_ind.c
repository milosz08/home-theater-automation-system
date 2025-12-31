#include "led_ind.h"
#include "i2c_bus.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG_LED_IND = "LED_IND";

static uint8_t pcf8574_port_state = 0xFF; // shadow register for led
static volatile bool is_error_state = false;

static TimerHandle_t act_led_timer = NULL,
                     cmd_led_timer = NULL;

static const gpio_num_t ALL_PCF_PINS[] = {
  PCF_PIN_LED_ETH_LINK,
  PCF_PIN_LED_ETH_ACT,
  PCF_PIN_LED_CMD
};

static void leds_commit_state(void)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (EXPANDER_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, pcf8574_port_state, true);
  i2c_master_stop(cmd);
  
  // using i2c manager from i2c_bus.h
  i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
  i2c_cmd_link_delete(cmd);
}

static void set_pcf_led(int pin, bool on)
{
  if (on)
  {
    pcf8574_port_state &= ~(1 << pin); // low = on
  }
  else
  {
    pcf8574_port_state |= (1 << pin);  // high = off
  }
  leds_commit_state();
}

static void turn_off_led_callback(TimerHandle_t xTimer)
{
  int pcf_pin = (int)pvTimerGetTimerID(xTimer);
  if (pcf_pin == PCF_PIN_LED_CMD && is_error_state)
  {
    return;
  }
  set_pcf_led(pcf_pin, false);
}

static void led_heartbeat_task(void *pvParameters)
{
  while (1)
  {
    if (is_error_state)
    {
      set_pcf_led(PCF_PIN_LED_CMD, true);
      vTaskDelay(pdMS_TO_TICKS(200));
    }
    else
    {
      if (xTimerIsTimerActive(cmd_led_timer) == pdFALSE)
      {
        set_pcf_led(PCF_PIN_LED_CMD, true);
        vTaskDelay(pdMS_TO_TICKS(LED_HOLD_TIME_MS));
        if (!is_error_state)
        {
          set_pcf_led(PCF_PIN_LED_CMD, false);
        }
      }
      vTaskDelay(pdMS_TO_TICKS(LED_HEARTBEAT_TIME_MS));
    }
  }
}

static void check_leds(void)
{
  size_t led_count = sizeof(ALL_PCF_PINS) / sizeof(ALL_PCF_PINS[0]);
  // turn on all
  for (int i = 0; i < led_count; i++)
  {
    pcf8574_port_state &= ~(1 << ALL_PCF_PINS[i]);
  }
  leds_commit_state();

  vTaskDelay(pdMS_TO_TICKS(LED_HOLD_TIME_MS * 2));

  // turn off all
  for (int i = 0; i < led_count; i++)
  {
    pcf8574_port_state |= (1 << ALL_PCF_PINS[i]);
  }
  leds_commit_state();
}

static void set_led_activity(TimerHandle_t timer, int pcf_pin) {
  if (timer != NULL)
  {
    set_pcf_led(pcf_pin, true);
    xTimerReset(timer, 0);
  }
}

void led_set_error(bool active)
{
  is_error_state = active;
}

void led_init(void)
{
  pcf8574_port_state = 0xFF;
  leds_commit_state();

  TickType_t xDelayTicks = pdMS_TO_TICKS(LED_HOLD_TIME_MS);
  if (xDelayTicks == 0 && LED_HOLD_TIME_MS > 0)
  {
    xDelayTicks = 1;
  }
  act_led_timer = xTimerCreate("ActTmr", xDelayTicks, pdFALSE, (void*)PCF_PIN_LED_ETH_ACT, turn_off_led_callback);
  cmd_led_timer = xTimerCreate("CmdTmr", xDelayTicks, pdFALSE, (void*)PCF_PIN_LED_CMD, turn_off_led_callback);

  check_leds();

  xTaskCreate(led_heartbeat_task, "led_heartbeat", 2048, NULL, 1, NULL);
  ESP_LOGI(TAG_LED_IND, "LEDs initialized");
}

void led_eth_set_link(bool on)
{
  set_pcf_led(PCF_PIN_LED_ETH_LINK, on);
}

void led_eth_packet_activity(void)
{
  set_led_activity(act_led_timer, PCF_PIN_LED_ETH_ACT);
}

void led_io_cmd_execution(void)
{
  xTimerStop(cmd_led_timer, 0); // turn off heartbeat timer
  const TickType_t blink_delay = pdMS_TO_TICKS(LED_BLINK_HOLD_TIME_MS);
  for (int i = 0; i < 2; i++)
  {
    set_pcf_led(PCF_PIN_LED_CMD, true);
    vTaskDelay(blink_delay);
    set_pcf_led(PCF_PIN_LED_CMD, false);
    vTaskDelay(blink_delay);
  }
}
