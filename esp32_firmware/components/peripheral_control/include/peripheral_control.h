#ifndef PERIPHERAL_CONTROL_H_
#define PERIPHERAL_CONTROL_H_

/*! \brief Lowers the projection screen.
 *
 * Sends the specific command via RS485 or activates the relay/motor responsible for moving the screen down.
 *
 * \return 0 on success, non-zero on error.
 */
int peripheral_control_screen_down(void);

/*! \brief Stops the projection screen movement.
 *
 * Sends the STOP command. Useful if you want the screen halfway.
 *
 * \return 0 on success, non-zero on error.
 */
int peripheral_control_screen_stop(void);

/*! \brief Raises the projection screen.
 *
 * Sends the UP command to retract the screen.
 *
 * \return 0 on success, non-zero on error.
 */
int peripheral_control_screen_up(void);

#endif // PERIPHERAL_CONTROL_H_
