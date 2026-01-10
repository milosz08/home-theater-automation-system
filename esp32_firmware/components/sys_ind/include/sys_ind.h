#ifndef SYS_IND_H_
#define SYS_IND_H_

#include "esp_err.h"

#include <stddef.h>
#include <stdbool.h>

// pin definitions on pcf8574 expander (0-4 out, 5-7 in)
#define PCF_PIN_OUT_LED_CMD         0       // p0: command invocation (red diode)
#define PCF_PIN_OUT_LED_ETH_LINK    1       // p1: eth connection status (orange diode)
#define PCF_PIN_OUT_LED_ETH_ACT     2       // p2: eth packets (green diode)
#define PCF_PIN_OUT_LCD_BACKLIGHT   3       // p3: lcd backlight (via mosfet)
#define PCF_PIN_OUT_BUZZER          4       // p4: buzzer (via mosfet)

#define LED_HOLD_TIME_MS            100     // time (ms) after turn off LED
#define LED_BLINK_HOLD_TIME_MS      50      // time (ms) between LED blinks
#define LED_HEARTBEAT_TIME_MS       3000    // heartbeat time (ms)
#define LED_HEARTBEAT_ERROR_MS      200     // heartbeat time on error (ms)
#define BUZZER_PERIOD_MS            400     // buzzer sound duration

esp_err_t sys_ind_init(void);
void sys_ind_led_eth_set_link(bool on);
void sys_ind_led_eth_packet_activity(void);
void sys_ind_led_io_cmd_execution(void);
void sys_ind_set_error(bool active);
void sys_ind_lcd_backlight_set(bool on);
void sys_ind_buzzer_sound(int count, uint32_t period_ms);
void sys_ind_fixed_buzzer_sound(int count);

#endif // SYS_IND_H_
