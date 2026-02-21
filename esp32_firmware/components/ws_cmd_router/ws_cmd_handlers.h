#ifndef WS_CMD_HANDLERS_H_
#define WS_CMD_HANDLERS_H_

#include <stdbool.h>

#include "cJSON.h"
#include "esp_err.h"

#define MAX_COMMANDS 8  /*!< Max number of commands we can register in the router. */

/*! \brief Handles the GET_SYS_INFO command.
 *
 * Retrieves system information (version, project name, compile time) and sends it back to the client via WebSocket
 * queue handler.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 * \param notify  Pointer to a boolean flag. Set to true if a command status response should be sent, false to skip.
 *
 * \retval ESP_OK   On successful execution.
 * \retval ESP_FAIL If an error occurred during execution.
 */
esp_err_t ws_cmd_get_sys_info(const cJSON *payload, bool *notify);

/*! \brief Handles the START_PROJECTION command.
 *
 * Executes a macro sequence that lowers the projection screen and turns on the projector simultaneously.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 * \param notify  Pointer to a boolean flag. Set to true if a command status response should be sent, false to skip.
 *
 * \retval ESP_OK   On successful execution.
 * \retval ESP_FAIL If an error occurred during execution.
 */
esp_err_t ws_cmd_start_projection(const cJSON *payload, bool *notify);

/*! \brief Handles the END_PROJECTION command.
 *
 * Executes a macro sequence that retracts the projection screen and turns off the projector simultaneously.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 * \param notify  Pointer to a boolean flag. Set to true if a command status response should be sent, false to skip.
 *
 * \retval ESP_OK   On successful execution.
 * \retval ESP_FAIL If an error occurred during execution.
 */
esp_err_t ws_cmd_end_projection(const cJSON *payload, bool *notify);

/*! \brief Handles the SCREEN_DOWN command.
 *
 * Triggers the RS485/relay logic to lower the projection screen.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 * \param notify  Pointer to a boolean flag. Set to true if a command status response should be sent, false to skip.
 *
 * \retval ESP_OK   On successful execution.
 * \retval ESP_FAIL If an error occurred during execution.
 */
esp_err_t ws_cmd_screen_down(const cJSON *payload, bool *notify);

/*! \brief Handles the SCREEN_STOP command.
 *
 * Halts the screen motor immediately.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 * \param notify  Pointer to a boolean flag. Set to true if a command status response should be sent, false to skip.
 *
 * \retval ESP_OK   On successful execution.
 * \retval ESP_FAIL If an error occurred during execution.
 */
esp_err_t ws_cmd_screen_stop(const cJSON *payload, bool *notify);

/*! \brief Handles the SCREEN_UP command.
 *
 * Triggers the RS485/relay logic to retract the screen.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 * \param notify  Pointer to a boolean flag. Set to true if a command status response should be sent, false to skip.
 *
 * \retval ESP_OK   On successful execution.
 * \retval ESP_FAIL If an error occurred during execution.
 */
esp_err_t ws_cmd_screen_up(const cJSON *payload, bool *notify);

/*! \brief Handles the PROJECTOR_ON command.
 *
 * Triggers the logic to power on the projector.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 * \param notify  Pointer to a boolean flag. Set to true if a command status response should be sent, false to skip.
 *
 * \retval ESP_OK   On successful execution.
 * \retval ESP_FAIL If an error occurred during execution.
 */
esp_err_t ws_cmd_projector_on(const cJSON *payload, bool *notify);

/*! \brief Handles the PROJECTOR_OFF command.
 *
 * Triggers the logic to power off the projector.
 *
 * \param payload The JSON object (unused here, but required by typedef signature).
 * \param notify  Pointer to a boolean flag. Set to true if a command status response should be sent, false to skip.
 *
 * \retval ESP_OK   On successful execution.
 * \retval ESP_FAIL If an error occurred during execution.
 */
esp_err_t ws_cmd_projector_off(const cJSON *payload, bool *notify);

#endif // WS_CMD_HANDLERS_H_
