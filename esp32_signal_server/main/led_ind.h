#pragma once

#include <stddef.h>
#include "driver/gpio.h"

#define PIN_LED_LINK  GPIO_NUM_25   // ETH connection status (orange)
#define PIN_LED_ACT   GPIO_NUM_33   // ETH packets (green)
#define LED_ACT_TICK  100           // time after make inactive LED (ETH packets, green)

void led_init(void);
void led_set_link(bool on);
void led_blink_activity(void);
