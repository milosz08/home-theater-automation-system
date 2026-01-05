idf_component_register(
  SRCS 
    "app_api_auth.c"
    "app_api_registry.c"
    "app_api_ws.c"

  INCLUDE_DIRS
    "include"

  PRIV_INCLUDE_DIRS
    "."

  REQUIRES
    esp_http_server
    log
    helper
    https_server
    nvs_manager
    ws_cmd_router
    ws_dispatcher
)
