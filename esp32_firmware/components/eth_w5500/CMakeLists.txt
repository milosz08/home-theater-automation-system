idf_component_register(
  SRCS
    "eth_w5500.c"

  INCLUDE_DIRS
    "include"

  REQUIRES
    driver
    esp_eth
    esp_event
    esp_netif
)
