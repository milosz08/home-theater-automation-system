#pragma once

#include <stddef.h>
#include "driver/gpio.h"

#define PIN_LED_ETH_LINK        GPIO_NUM_25   // ETH connection status (orange)
#define PIN_LED_ETH_ACT         GPIO_NUM_26   // ETH packets (green)
#define PIN_LED_CMD             GPIO_NUM_33   // command invocation (red)

#define LED_HOLD_TIME_MS        100           // time (ms) after turn off LED
#define LED_BLINK_HOLD_TIME_MS  50            // time (ms) between LED blinks

typedef enum {
  CMD_TYPE_IN,
  CMD_TYPE_OUT
} led_cmd_type_t;

void led_init(void);
void led_eth_set_link(bool on);
void led_eth_packet_activity(void);
void led_io_cmd_execution(led_cmd_type_t type);
