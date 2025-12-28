idf_component_register(SRCS "main.c" "eth_w5500.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_eth esp_netif esp_event driver nvs_flash)
