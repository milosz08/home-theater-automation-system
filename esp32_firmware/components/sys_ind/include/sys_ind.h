#ifndef SYS_IND_H_
#define SYS_IND_H_

#include "esp_err.h"

#include <stddef.h>
#include <stdbool.h>

// Output mapping on the IO Expander (PCF8574)
#define PCF_PIN_OUT_LED_CMD         0       /*!< Red LED: command execution/error. */
#define PCF_PIN_OUT_LED_ETH_ACT     1       /*!< Orange LED: Ethernet activity. */
#define PCF_PIN_OUT_LED_ETH_LINK    2       /*!< Green LED: Ethernet link UP. */
#define PCF_PIN_OUT_LCD_BACKLIGHT   3       /*!< MOSFET gate for LCD backlight. */
#define PCF_PIN_OUT_BUZZER          4       /*!< PNP+NPN gate for active Buzzer. */

#define LED_HOLD_TIME_MS            100     /*!< How long to flash the LED. */
#define LED_BLINK_HOLD_TIME_MS      50      /*!< Fast blink duration. */
#define LED_HEARTBEAT_TIME_MS       3000    /*!< Idle heartbeat interval. */
#define LED_HEARTBEAT_ERROR_MS      200     /*!< Panic blink interval. */
#define BUZZER_PERIOD_MS            400     /*!< Duration of a single beep. */
#define CMD_LED_COUNT               2       /*!< Count of turn on red diode on command. */
#define ERROR_LED_COUNT             3       /*!< Count of turn on red diode on error. */

/*! \brief Sets up the system indicators.
 *
 * Creates software timers for LEDs and the buzzer. Starts the heartbeat task.
 *
 * \retval ESP_OK         On success.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       On init failure.
 */
esp_err_t sys_ind_init(void);

/*! \brief Updates the link LED (green).
 *
 * \param on `true` to turn on, `false` to turn off.
 */
void sys_ind_led_eth_set_link(bool on);

/*! \brief Triggers the activity LED (orange).
 * 
 * Turns the LED on and resets the "off-timer". This creates a nice blinking effect proportional to network traffic.
 */
void sys_ind_led_eth_packet_activity(void);

/*! \brief Blinks the IO/Command LED a specific number of times.
 *
 * Used to visually signal discrete events (like command reception or specific errors).
 *
 * \param count Number of blinks.
 */
void sys_ind_led_io_execution(int count);

/*! \brief Signals a standard command execution.
 *
 * Blinks the LED `CMD_LED_COUNT` times to confirm an action.
 */
void sys_ind_led_io_cmd_execution(void);

/*! \brief Signals a temporary error event.
 *
 * Blinks the LED `ERROR_LED_COUNT` times. Unlike `sys_ind_set_error`, this indicates a single event, not a persistent
 * error state.
 */
void sys_ind_led_io_error_execution(void);

/*! \brief Puts the system into (or out of) error mode.
 *
 * In error mode, the Red LED blinks rapidly to alert the user.
 *
 * \param active `true` to enable error state.
 */
void sys_ind_set_error(bool active);

/*! \brief Controls the LCD backlight.
 *
 * \param on `true` for light, `false` for dark.
 */
void sys_ind_lcd_backlight_set(bool on);

/*! \brief Beeps the buzzer a specific number of times.
 *
 * Uses a timer to handle the on/off intervals asynchronously.
 *
 * \param count     How many beeps.
 * \param period_ms Duration of one beep cycle.
 */
void sys_ind_buzzer_sound(int count, uint32_t period_ms);

/*! \brief Convenience wrapper for standard beep duration.
 *
 * Calls `sys_ind_buzzer_sound` with default timing.
 *
 * \param count How many beeps.
 */
void sys_ind_fixed_buzzer_sound(int count);

#endif // SYS_IND_H_
