idf_component_register(
  SRCS
    "https_server.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    esp_https_server
    esp_eth
    esp_netif
    esp_event
    log
    helper
    nvs_manager
)
