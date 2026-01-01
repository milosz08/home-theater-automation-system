idf_component_register(
  SRCS
    "storage.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    esp_common
    json
    log
    spiffs
    helper
)
