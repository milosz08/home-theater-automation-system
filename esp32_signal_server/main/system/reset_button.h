#pragma once

#include "driver/gpio.h"

#define RESET_BUTTON_GPIO  GPIO_NUM_34
#define HOLD_TIME_MS       3000

void init_reset_button(void);
