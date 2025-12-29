PROJECT_NAME := esp32_signal_server

PORT ?= COM3 # communication port for ESP32 board
BAUD ?= 921600 # flashing speed

.PHONY: all build rebuild flash flash-app monitor run clean erase menuconfig help

all: build

# normal building
build:
	idf.py build

# full rebuild (change .env file or CMakeLists.txt)
rebuild:
	idf.py reconfigure
	idf.py build

# flash with high speed
flash: build
	idf.py -p $(PORT) -b $(BAUD) flash

# flash only app
flash-app: build
	idf.py -p $(PORT) -b $(BAUD) app-flash

# monitor with decoder
monitor:
	idf.py -p $(PORT) monitor

# flash and run
run: flash monitor

# clean output files
clean:
	idf.py fullclean

# clean flash (remove NVS)
erase:
	idf.py -p $(PORT) erase-flash

menuconfig:
	idf.py menuconfig
