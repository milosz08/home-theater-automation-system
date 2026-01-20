#ifndef WS_DISPATCHER_H_
#define WS_DISPATCHER_H_

#include "esp_err.h"
#include "esp_http_server.h"

/*! \brief Initializes the WebSocket Dispatcher.
 *
 * Starts the task responsible for emptying the event queue and broadcasting messages to all connected clients.
 *
 * \retval ESP_OK         If success.
 * \retval ESP_ERR_NO_MEM If no heap memory available.
 * \retval ESP_FAIL       If websocket dispatcher initialization failure.
 */
esp_err_t ws_dispatcher_init(void);

/*! \brief Registers a new WebSocket client.
 *
 * Adds the file descriptor (sockfd) to the internal list of active clients so they receive broadcasts.
 *
 * \param server_handle Handle to the HTTP server.
 * \param new_sockfd    Socket ID of the new client.
 */
void ws_dispatcher_add_client(httpd_handle_t server_handle, int new_sockfd);

/*! \brief Removes a client from the broadcast list.
 *
 * Called when a socket closes or errors out.
 *
 * \param sockfd Socket ID to remove.
 */
void ws_dispatcher_remove_client(int sockfd);

#endif // WS_DISPATCHER_H_
