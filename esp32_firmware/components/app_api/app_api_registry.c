#include "app_api.h"
#include "app_api_handlers.h"
#include "helper.h"

// private api ---------------------------------------------------------------------------------------------------------

static const https_endpoint_t s_endpoints[] = {
  // auth
  {
    .uri        = "/api/change-password",
    .method     = HTTP_POST,
    .handler    = app_api_auth_change_password,
    .is_public  = false,
    .name       = "Change password"
  },
  {
    .uri        = "/api/reset-password",
    .method     = HTTP_DELETE,
    .handler    = app_api_auth_reset_password,
    .is_public  = false,
    .name       = "Reset password"
  },
  // control (ws)
  {
    .uri        = "/ws/control",
    .method     = HTTP_GET,
    .handler    = app_api_ws,
    .is_public  = false,
    .is_ws      = true
  },
  // ota update
  {
    .uri        = "/api/update",
    .method     = HTTP_POST,
    .handler    = app_api_ota_update,
    .is_public  = false,
    .name       = "Firmware update"
  },
};

// public api ----------------------------------------------------------------------------------------------------------

const https_endpoint_t *app_api_get_endpoints(void)
{
  return s_endpoints;
}

size_t app_api_get_endpoints_count(void)
{
  return ARRAY_SIZE(s_endpoints);
}
