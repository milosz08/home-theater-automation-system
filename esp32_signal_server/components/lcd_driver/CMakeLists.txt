idf_component_register(
  SRCS
    "lcd_driver.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    driver
    i2c_bus
)
