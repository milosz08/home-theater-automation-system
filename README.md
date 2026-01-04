# Home theater automation system

[TBD]

## Table of content

* [Hardware and software stack](#hardware-and-software-stack)
* [Author](#author)
* [License](#license)

## Hardware

[TBD]

## Software

### ESP32 firmware

[TBD]

### Provisioning tool (certgen and ESP32 flashing)

[TBD]

### Android client

[TBD]

## Hardware and software stack

* ESP32 WROOM DevKit,
* ESP-IDF, ESP-LittleFS, C, CMake, Make,
* MAX3232 (TTL <-> RS232), MAX3485 (TTL <-> RS485), W5500 (LAN),
* I2C, PCF8574, LCD 1602 HD44780 (FFSTN),
* PKI with CA (MITM attacks protection), basic auth,
* Python (esptool, littlefs-python, cryptography, pyserial, qrcode),
* Android (Kotlin).

## Author

Created by Miłosz Gilga. If you have any questions about this application, send
message: [miloszgilga@gmail.com](mailto:miloszgilga@gmail.com).

## License

This project is licensed under the GNU General Public License v3.0.
