idf_component_register(
  SRCS
    "ws_dispatcher.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    esp_http_server
    log
    ws_queue
)
