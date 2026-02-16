idf_component_register(
  SRCS
    "ui_manager.c"
  
  INCLUDE_DIRS
    "include"

  REQUIRES
    esp_timer
    log
    env_sensor
    eth_w5500
    helper
    ntp_service
    ui
)
