idf_component_register(
  SRCS 
    "io_button_handlers.c"
    "io_button_registry.c"

  INCLUDE_DIRS
    "include"

  PRIV_INCLUDE_DIRS
    "."

  REQUIRES
    helper
    https_server
    io_input
    nvs_manager
    sys_ind
    ui
    ui_manager
)
