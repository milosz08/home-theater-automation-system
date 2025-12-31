cmake_minimum_required(VERSION 3.5)

# include esp-idf toolkit
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# project name
project(esp32_signal_server)

spiffs_create_partition_image(storage data FLASH_IN_PROJECT)
