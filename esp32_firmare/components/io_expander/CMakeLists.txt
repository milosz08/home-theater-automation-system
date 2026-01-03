idf_component_register(
  SRCS
    "io_expander.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    driver
    freertos
    i2c_bus
)
