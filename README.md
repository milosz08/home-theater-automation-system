# Home theater automation system

[TBD]

## Table of content

* [Hardware](#hardware)
* [Software](#software)
* [Hardware and software stack](#hardware-and-software-stack)
* [Author](#author)
* [License](#license)

## Hardware

[TBD]

## Software

### ESP32 firmware

The embedded firmware layer for the controller was developed directly within the ESP-IDF framework,
utilizing FreeRTOS, while the Arduino ecosystem was bypassed to ensure complete control over system
resources and performance optimization. A modular CMake architecture was adopted, wherein hardware
drivers remain strictly separated from business logic, facilitating communication exclusively through
callback mechanisms. Regarding network connectivity, a hybrid approach was implemented in which REST
APIs manage critical configuration updates and authorization, while WebSockets are utilized for
real-time control to eliminate input lag. Server-Sent Events (SSE) were explicitly rejected due to the
limitations imposed by the single-threaded nature of the HTTP server, which poses significant concurrency
challenges, as well as to avoid the cumulative latency inherent in standard REST-based interactions.
Furthermore, to prevent slower bus operations, such as those involving UART or I2C interfaces, from
blocking the network thread, asynchronous processing is enforced via queues utilizing a Producer-Consumer
model.

### Provisioning tool (certgen and ESP32 flashing)

This desktop tool handles the configuration and flashing of the ESP32 Home Theater firmware. It generates
the necessary SSL certificates and private keys for the HTTPS server, and produces a QR code that allows
the Android client to connect instantly without manual setup. The application can flash the device via UART
by combining the base firmware binary (available in Releases tab) with a LittleFS storage partition that is
generated on the fly based on your input and the partition table (located in `esp32_firmware/partitions.csv`).

![](.github/img/esp32-provisioning-tool-demo.gif)

### Android client

[TBD]

## Hardware and software stack

* ESP32 WROOM DevKit,
* MAX3232 (TTL <-> RS232), MAX3485 (TTL <-> RS485), W5500 (LAN),
* I2C, PCF8574, LCD 1602 HD44780 (FFSTN),
* LM2596S DC-DC Step-Down 12V -> 5V, AMS1117 DC-DC Step-Down 5V -> 3.3V (power section),
* AHT20 + BMP280, BMT1206UXLF,
* 2N7000 (diode), 1N4007 (transistor),
* ESP-IDF, ESP-LittleFS, C, CMake, Make,
* PKI with CA (MITM attacks protection), basic auth,
* Python (esptool, littlefs-python, cryptography, pyserial, qrcode),
* Android (Kotlin).

## Author

Created by Miłosz Gilga. If you have any questions about this application, send
message: [miloszgilga@gmail.com](mailto:miloszgilga@gmail.com).

## License

This project is licensed under the GNU General Public License v3.0.
