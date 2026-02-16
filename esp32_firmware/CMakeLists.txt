cmake_minimum_required(VERSION 3.5)
set(PROJECT_VER "${CMD_APP_VER}")

# include esp-idf toolkit
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# project name
project(esp32_firmware)

littlefs_create_partition_image(littlefs data FLASH_IN_PROJECT)
