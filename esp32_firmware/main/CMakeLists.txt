idf_component_register(
  SRCS
    "main.c"

  INCLUDE_DIRS
    "."

  REQUIRES
    esp_netif
    esp_event
    esp_eth
    driver
    log
    freertos
    app_api
    env_sensor
    eth_w5500
    https_server
    i2c_bus
    io_expander
    io_input
    led_ind
    nvs_manager
    storage
    uart_bus
    ui
)
