PROJECT_NAME := esp32_signal_server
PORT ?= COM3 # communication port for ESP32 board

.PHONY: all build flash monitor clean menuconfig help

all: build

build:
	idf.py build

flash: build
	idf.py -p $(PORT) flash

monitor:
	idf.py -p $(PORT) monitor

run: flash
	idf.py -p $(PORT) monitor

clean:
	idf.py fullclean

erase:
	idf.py -p $(PORT) erase-flash

menuconfig:
	idf.py menuconfig
