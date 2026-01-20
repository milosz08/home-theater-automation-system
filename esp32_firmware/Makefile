PROJECT_NAME := esp32_firmware

PORT ?= COM3 # communication port for ESP32 board
BAUD ?= 921600 # flashing speed

.PHONY: all build rebuild flash flash-app flash-data monitor run clean erase menuconfig help

all: build

# normal building
build:
	idf.py build

# full rebuild (CMakeLists.txt or partition structure)
rebuild:
	idf.py reconfigure
	idf.py build

# flash with high speed (app, bootloader, partition table, storage)
flash: build
	idf.py -p $(PORT) -b $(BAUD) flash

# flash only app (without SPIFFS)
flash-app: build
	idf.py -p $(PORT) -b $(BAUD) app-flash

# change some content inside data directory
flash-data:
	idf.py -p $(PORT) -b $(BAUD) storage-flash

# monitor with decoder
monitor:
	idf.py -p $(PORT) monitor

# flash and run
run: flash monitor

# fast refresh
run-data: flash-data monitor

# clean output files
clean:
	idf.py fullclean

# clean flash (remove NVS and SPIFFS)
erase:
	idf.py -p $(PORT) erase-flash

menuconfig:
	idf.py menuconfig

# generate docs via doxygen (only for .c and .h sources inside components and main directory)
docs:
	doxygen
