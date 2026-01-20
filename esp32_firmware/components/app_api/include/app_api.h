#ifndef APP_API_H_
#define APP_API_H_

#include "https_server.h"

#include <stddef.h>

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

#endif // APP_API_H_
