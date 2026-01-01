idf_component_register(
  SRCS
    "io_input.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    io_expander
    i2c_bus
)
