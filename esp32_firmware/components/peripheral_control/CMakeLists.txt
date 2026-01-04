idf_component_register(
  SRCS
    "peripheral_control.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    log
    uart_bus
)
