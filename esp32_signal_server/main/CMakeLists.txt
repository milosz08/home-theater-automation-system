idf_component_register(SRCS "main.c"
                            "eth_w5500.c"
                            "rest_server.c"
                            "rest_handlers.c"
                            "auth_nvs.c"
                            "reset_button.c"
                            "utils.c"
                            "led_ind.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_eth esp_netif esp_event driver nvs_flash esp_https_server mbedtls)

target_add_binary_data(${COMPONENT_TARGET} "certs/cacert.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/prvtkey.pem" TEXT)

set(ENV_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../.env")

if(EXISTS "${ENV_FILE}")
     file(STRINGS "${ENV_FILE}" lines)
     foreach(line ${lines})
          string(STRIP "${line}" line)
          if(line MATCHES "^#" OR line STREQUAL "")
               continue()
          endif()
          if(line MATCHES "^([^=]+)=(.*)$")
               set(KEY "${CMAKE_MATCH_1}")
               set(VALUE "${CMAKE_MATCH_2}")
               string(STRIP "${KEY}" KEY)
               string(STRIP "${VALUE}" VALUE)
               string(REGEX REPLACE "^[\"']|[\"']$" "" VALUE "${VALUE}")
               target_compile_definitions(${COMPONENT_LIB} PRIVATE ${KEY}="${VALUE}")
          endif()
     endforeach()
endif()
