#ifndef LED_IND_H_
#define LED_IND_H_

#include "esp_err.h"

#include <stddef.h>
#include <stdbool.h>

// pin definitions on pcf8574 expander (0-3 out, 4-7 in)
#define PCF_PIN_OUT_LED_CMD         0       // p0: command invocation (red)
#define PCF_PIN_OUT_LED_ETH_LINK    1       // p1: eth connection status (orange)
#define PCF_PIN_OUT_LED_ETH_ACT     2       // p2: eth packets (green)

#define LED_HOLD_TIME_MS            100     // time (ms) after turn off LED
#define LED_BLINK_HOLD_TIME_MS      50      // time (ms) between LED blinks
#define LED_HEARTBEAT_TIME_MS       3000    // heartbeat time (ms)
#define LED_HEARTBEAT_ERROR_MS      200     // heartbeat time on error (ms)

esp_err_t led_ind_init(void);
void led_ind_eth_set_link(bool on);
void led_ind_eth_packet_activity(void);
void led_ind_io_cmd_execution(void);
void led_ind_set_error(bool active);

#endif // LED_IND_H_
