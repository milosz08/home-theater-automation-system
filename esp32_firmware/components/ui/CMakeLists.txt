idf_component_register(
  SRCS
    "ui_icon.c"
    "ui_manager.c"
    "ui.c"
  
  INCLUDE_DIRS
    "include"

  PRIV_INCLUDE_DIRS
    "."

  REQUIRES
    esp_timer
    lcd_driver
    log
    env_sensor
    eth_w5500
    helper
)
