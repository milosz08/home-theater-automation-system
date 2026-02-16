#ifndef APP_API_HANDLERS_H_
#define APP_API_HANDLERS_H_

#include "esp_http_server.h"
#include "esp_err.h"

/*! \brief Handles the "password change" request (POST).
 *
 * Expects a JSON payload with `old_pass` and `new_pass`. Verifies the old password against NVS, and if correct, updates
 * the storage with the new one.
 *
 * \param req The HTTP request context.
 * 
 * \retval ESP_OK   If handled (even if password was wrong, we send a 400/401 response).
 * \retval ESP_FAIL On other errors.
 */
esp_err_t app_api_auth_change_password(httpd_req_t *req);

/*! \brief Main handler for the "websocket control" endpoint.
 *
 * This function is the entry point for the WS connection (`/ws/control`). It performs the handshake and then registers
 * the new client socket with the `ws_dispatcher` so it can receive updates.
 *
 * \param req The HTTP request context.
 *
 * \retval ESP_OK         On successful connection.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       On other errors.
 */
esp_err_t app_api_ws(httpd_req_t *req);

/*! \brief Handles the OTA firmware update request (POST).
 *
 * Expects the raw firmware binary in the request body with `Content-Type: application/octet-stream`. Writes the data to
 * the next OTA partition, providing progress updates via LCD and WebSockets. On success, it sets the new boot partition
 * and restarts the system.
 *
 * \param req The HTTP request context.
 *
 * \retval ESP_OK   On success (or handled error).
 * \retval ESP_FAIL On critical failure.
 */
esp_err_t app_api_ota_update(httpd_req_t *req);

#endif // APP_API_HANDLERS_H_
