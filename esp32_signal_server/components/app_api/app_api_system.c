#include "app_api_handlers.h"
#include "https_server.h"

// public api ----------------------------------------------------------------------------------------------------------

// GET /api/info
esp_err_t app_api_system_health(httpd_req_t *req)
{
  return https_server_silent_response(req);
}
