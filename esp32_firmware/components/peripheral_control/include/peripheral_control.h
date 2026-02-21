#ifndef PERIPHERAL_CONTROL_H_
#define PERIPHERAL_CONTROL_H_

#include "esp_err.h"

/*! \brief Lowers the projection screen.
 *
 * Sends the specific command via RS485 or activates the relay/motor responsible for moving the screen down.
 *
 * \retval ESP_OK   If the command was successfully sent to the screen controller.
 * \retval ESP_FAIL If the RS485 bus is busy or the hardware interface failed.
 */
esp_err_t peripheral_control_screen_down(void);

/*! \brief Stops the projection screen movement.
 *
 * Sends the STOP command. Useful if you want the screen halfway.
 *
 * \retval ESP_OK   If the stop command was successfully transmitted.
 * \retval ESP_FAIL If the communication with the screen controller failed.
 */
esp_err_t peripheral_control_screen_stop(void);

/*! \brief Raises the projection screen.
 *
 * Sends the UP command to retract the screen.
 *
 * \retval ESP_OK   If the retract command was successfully transmitted.
 * \retval ESP_FAIL If the hardware interface is unavailable.
 */
esp_err_t peripheral_control_screen_up(void);

/*! \brief Powers on the projector.
 *
 * Sends the power-on sequence to the projector.
 *
 * \retval ESP_OK   If the power-on sequence was initiated successfully.
 * \retval ESP_FAIL If the projector is unresponsive or the command failed to send.
 */
esp_err_t peripheral_control_projector_on(void);

/*! \brief Powers off the projector.
 *
 * Sends the power-off sequence to the projector. May include a cooldown period handling.
 *
 * \retval ESP_OK   If the power-off sequence was initiated successfully.
 * \retval ESP_FAIL If communication with the projector failed.
 */
esp_err_t peripheral_control_projector_off(void);

#endif // PERIPHERAL_CONTROL_H_
