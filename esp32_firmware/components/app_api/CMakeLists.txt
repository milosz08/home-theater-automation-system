idf_component_register(
  SRCS 
    "app_api_auth.c"
    "app_api_ota_update.c"
    "app_api_registry.c"
    "app_api_ws.c"
    "app_api.c"

  INCLUDE_DIRS
    "include"

  PRIV_INCLUDE_DIRS
    "."

  REQUIRES
    app_update
    esp_http_server
    esp_partition
    log
    helper
    https_server
    nvs_manager
    ui_manager
    ui
    ws_cmd_router
    ws_dispatcher
    ws_queue
)
