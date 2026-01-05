idf_component_register(
  SRCS
    "env_sensor.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    driver
    esp_timer
    i2c_bus
)
