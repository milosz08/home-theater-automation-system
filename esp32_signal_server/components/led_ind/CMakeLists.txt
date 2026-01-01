idf_component_register(
  SRCS
    "led_ind.c"

  INCLUDE_DIRS
    "include"

  REQUIRES 
    driver
    io_expander
)
