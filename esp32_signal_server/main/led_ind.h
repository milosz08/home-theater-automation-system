#pragma once

#include <stddef.h>
#include "driver/gpio.h"

// pin definitions on pcf8574 expander (0-7)
#define PCF_PIN_LED_CMD         0       // p0: command invocation (red)
#define PCF_PIN_LED_ETH_LINK    1       // p1: eth connection status (orange)
#define PCF_PIN_LED_ETH_ACT     2       // p2: eth packets (green)

#define LED_HOLD_TIME_MS        100     // time (ms) after turn off LED
#define LED_BLINK_HOLD_TIME_MS  50      // time (ms) between LED blinks
#define LED_HEARTBEAT_TIME_MS   3000    // heartbeat time (ms)

void led_init(void);
void led_eth_set_link(bool on);
void led_eth_packet_activity(void);
void led_io_cmd_execution();
void led_set_error(bool active);
