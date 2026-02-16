idf_component_register(
  SRCS
    "ws_cmd_router.c"
    "ws_cmd_handlers.c"
  
  INCLUDE_DIRS
    "include"

  PRIV_INCLUDE_DIRS
    "."

  REQUIRES
    esp_app_format
    log
    json
    peripheral_control
    ws_queue
)
