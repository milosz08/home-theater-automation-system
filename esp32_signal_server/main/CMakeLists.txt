idf_component_register(SRCS "main.c"
                            "driver/eth_w5500.c"
                            "driver/i2c_bus.c"
                            "driver/uart_bus.c"
                            "network/auth_nvs.c"
                            "network/rest_handlers.c"
                            "network/rest_server.c"
                            "system/led_ind.c"
                            "system/reset_button.c"
                            "system/storage.c"
                            "ui/lcd_driver.c"
                            "ui/lcd_icons.c"
                            "ui/ui.c"
                            "util.c"
                       INCLUDE_DIRS "."
                                    "driver"
                                    "network"
                                    "system"
                                    "ui"
                       REQUIRES esp_eth
                                esp_netif
                                esp_event
                                driver
                                nvs_flash
                                esp_https_server
                                mbedtls
                                json
                                spiffs)
