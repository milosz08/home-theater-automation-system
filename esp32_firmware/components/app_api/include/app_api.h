#ifndef APP_API_H_
#define APP_API_H_

#include "https_server.h"

#include <stddef.h>
#include <stdbool.h>

// custom error codes
#define APP_ERR_INVALID_PASSWORD 0x10001

/*! \brief List of generic variable names used in handlers.
 *
 * If the stringified name from APP_API_RETURN_ERR matches one of these, the system will fallback to esp_err_to_name().
 */
#define API_GENERIC_VAR_NAMES "res", "err"

/*! \brief Helper macro to return an API error.
 *
 * Captures the error code name automatically using the stringify operator (#). This macro returns `ESP_OK` to the
 * caller (the middleware) so that the connection remains open, allowing `app_api_handle_response` to send the JSON
 * error.
 *
 * \param req    The HTTP request handle.
 * \param status The HTTP status code (e.g., 400, 500).
 * \param code   The ESP error code (esp_err_t).
 */
#define APP_API_RETURN_ERR(req, status, code) \
  _app_api_return_err_helper(req, status, code, #code)

/*! \brief Context structure passed between the middleware and the handler.
 *
 * Used to store response state (status code, error details) to generate a standardized JSON response automatically.
 */
typedef struct
{
  int http_status;            /*!< The HTTP status code to return (e.g., 204, 400, 500). */
  esp_err_t error_code;       /*!< The internal ESP-IDF error code or custom app error. */
  const char *error_name_str; /*!< The stringified name of the error variable/constant (from macro). */
  bool manual_response;       /*!< If true, the handler has already sent the response; middleware skips JSON gen. */
} api_ctx_t;

/*! \brief Allocates and initializes the API context.
 *
 * Used by the HTTPS server middleware as the `create_request_ctx` callback.
 *
 * \param req The HTTP request handle.
 *
 * \return Pointer to the allocated `api_ctx_t` (void*).
 */
void *app_api_create_ctx(httpd_req_t *req);

/*! \brief Finalizes the request and sends the response.
 *
 * Generates a JSON error object if an error occurred, or sends 204 No Content on success. Used by the HTTPS server
 * middleware as the `handle_response` callback.
 *
 * \param req       The HTTP request handle.
 * \param void_ctx  The context created by `app_api_create_ctx`.
 * \param res       The result returned by the request handler logic.
 *
 * \return The final error code (usually passed to logging/callbacks).
 */
esp_err_t app_api_handle_response(httpd_req_t *req, void *void_ctx, esp_err_t res);

/*! \brief Retrieves the application-specific API endpoints.
 *
 * This includes things like "password change" (POST) and "control" (WS).
 *
 * \return Pointer to the static array of `https_endpoint_t`.
 */
const https_endpoint_t *app_api_get_endpoints(void);

/*! \brief Returns the number of application endpoints.
 *
 * Helper to know how big the array from `app_api_get_endpoints` is.
 *
 * \return Count of endpoints.
 */
size_t app_api_get_endpoints_count(void);

/*! \brief Internal helper for APP_API_RETURN_ERR.
 *
 * Populates the context with error details but returns ESP_OK.
 */
esp_err_t _app_api_return_err_helper(httpd_req_t *req, int status, esp_err_t code, const char *name_str);

/*! \brief Sets the context to success state (HTTP 204).
 *
 * Should be used at the end of a successful handler.
 *
 * \param req The HTTP request handle.
 * \return Always ESP_OK.
 */
esp_err_t app_api_return_ok(httpd_req_t *req);

/*! \brief Flags that the handler will send its own response.
 *
 * Useful for file downloads or custom data streams where the default JSON response is not desired.
 *
 * \param req The HTTP request handle.
 */
void app_api_set_manual_response(httpd_req_t *req);

#endif // APP_API_H_
