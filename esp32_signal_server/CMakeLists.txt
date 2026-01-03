cmake_minimum_required(VERSION 3.5)

# include esp-idf toolkit
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# project name
project(esp32_signal_server)

littlefs_create_partition_image(littlefs data FLASH_IN_PROJECT)
