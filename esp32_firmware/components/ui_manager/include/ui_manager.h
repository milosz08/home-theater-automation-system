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

#endif // UI_MANAGER_H_
