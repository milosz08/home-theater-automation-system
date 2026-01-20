#ifndef UI_H_
#define UI_H_

#include "esp_err.h"

// custom characters
#define PROGRESS_BAR_CHAR         0x01
#define DEGREE_CHAR               0x02

/*! \brief How long messages stay on screen (ms). */
#define REQ_MESS_DURATION_MS      2000

/*! \brief Initializes the User Interface.
 *
 * Sets up the LCD driver and loads custom characters (like the degree symbol).
 *
 * \retval ESP_OK   On success.
 * \retval ESP_FAIL On error during UI intialization.
 */
esp_err_t ui_init(void);

/*! \brief Sets the text for the 2 lines of the LCD.
 *
 * Updates the internal buffer and redraws the screen immediately.
 *
 * \param line0 Text for the top row.
 * \param line1 Text for the bottom row.
 */
void ui_set_text(const char *line0, const char *line1);

/*! \brief Redraws the standard info screen.
 *
 * Useful after a temporary message (like an error or popup) has finished displaying.
 */
void ui_restore_screen(void);

/*! \brief Shows a progress bar during boot.
 *
 * Visualizes initialization steps (e.g., [###...]).
 *
 * \param task_name   Name of the current step (e.g., "Eth").
 * \param step        Current step number.
 * \param total_steps Total number of steps.
 */
void ui_show_boot_progress(const char *task_name, int step, int total_steps);

/*! \brief Displays an error message.
 *
 * \param msg The error text.
 */
void ui_show_error(const char *msg);

/*! \brief Displays a critical error and halts/loops.
 *
 * Used when the system cannot continue (e.g., memory failure).
 *
 * \param msg Error description.
 * \param err The ESP error code.
 */
void ui_show_critical_error(const char *msg, esp_err_t err);

/*! \brief Shows the network status screen.
 *
 * Displays the current IP and MAC address.
 */
void ui_show_network_dashboard(const char *ip_str, const uint8_t *mac_addr);

/*! \brief Shows a temporary popup message.
 *
 * Replaces the screen content for `duration_ms` and then restores the previous screen.
 */
void ui_show_temp_message(const char *text, uint32_t duration_ms);

/*! \brief Shows a temporary command execution feedback.
 *
 * Similar to `ui_show_temp_message` but specifically styled for commands.
 */
void ui_show_temp_cmd_message(const char *text, uint32_t duration_ms);

/*! \brief Shows a command message with default duration. */
void ui_show_fixed_temp_cmd_message(const char *text);

#endif // UI_H_
