#ifndef WS_CMD_HANDLERS_H_
#define WS_CMD_HANDLERS_H_

#include "cJSON.h"

#define MAX_COMMANDS 4  /*!< Max number of commands we can register in the router. */

// Projection screen command handlers

/*! \brief Handles the SCREEN_DOWN command.
 *
 * Triggers the RS485/relay logic to lower the screen.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 */
void ws_cmd_screen_down(const cJSON *payload);

/*! \brief Handles the SCREEN_STOP command.
 *
 * Halts the screen motor immediately.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 */
void ws_cmd_screen_stop(const cJSON *payload);

/*! \brief Handles the SCREEN_UP command.
 *
 * Triggers the RS485/relay logic to retract the screen.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 */
void ws_cmd_screen_up(const cJSON *payload);

/*! \brief Handles the GET_SYS_INFO command.
 *
 * Retrieves system information (version, project name, compile time) and sends it back to the client via WebSocket
 * queue handler.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 */
void ws_cmd_get_sys_info(const cJSON *payload);

#endif // WS_CMD_HANDLERS_H_
