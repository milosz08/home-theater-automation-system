idf_component_register(
  SRCS
    "ws_cmd_router.c"
    "ws_cmd_handlers.c"
  
  INCLUDE_DIRS
    "include"

  PRIV_INCLUDE_DIRS
    "."

  REQUIRES
    log
    json
    peripheral_control
)
