#ifndef APP_API_H_
#define APP_API_H_

#include "https_server.h"

#include <stddef.h>

const https_endpoint_t *app_api_get_endpoints(void);
size_t app_api_get_endpoints_count(void);

#endif
