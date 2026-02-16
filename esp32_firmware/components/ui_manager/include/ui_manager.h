#ifndef UI_MANAGER_H_
#define UI_MANAGER_H_

#include "esp_err.h"

/*! \brief Fires up the UI Manager.
 *
 * Starts the internal timer for the auto-scroll carousel and renders the initial page on the display.
 *
 * \retval ESP_OK         If memory allocation was successful.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       Other UI manager initialization error.
 */
esp_err_t ui_manager_init(void);

/*! \brief Toggles between auto and manual mode.
 *
 * In auto mode, pages switch automatically every few seconds. In manual mode, the user stays on the current page until
 * they press a button. Displays a temporary message on the screen confirming the change.
 */
void ui_manager_switch_mode(void);

/*! \brief Manually switches to the next page.
 *
 * Useful when in manual mode. If called in auto mode, it just jumps to the next page and resets the auto-scroll timer.
 */
void ui_manager_manual_mode(void);

/*! \brief Pauses the UI manager updates.
 *
 * Stops the auto-scroll timer and prevents page rendering. Use this before running blocking tasks that need exclusive
 * screen access (e.g., OTA updates) to avoid visual glitches.
 */
void ui_manager_suspend(void);

/*! \brief Resumes the UI manager updates.
 *
 * Restarts the auto-scroll timer (if previously enabled) and immediately refreshes the current page. Call this after a
 * blocking task is finished or failed to restore the dashboard.
 */
void ui_manager_resume(void);

#endif // UI_MANAGER_H_
