#include "app_api.h"
#include "app_api_handlers.h"

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
};

// public api ----------------------------------------------------------------------------------------------------------

const https_endpoint_t *app_api_get_endpoints(void)
{
  return s_endpoints;
}

size_t app_api_get_endpoints_count(void)
{
  return sizeof(s_endpoints) / sizeof(s_endpoints[0]);
}
