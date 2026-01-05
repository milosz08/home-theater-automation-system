idf_component_register(
  SRCS
    "cyclic_task.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    esp_timer
    env_sensor
    ws_queue
)
