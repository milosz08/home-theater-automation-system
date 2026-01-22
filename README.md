# Home theater automation system

A specialized ESP32-based home theater controller designed for reliable, hardwired environments where 
Wi-Fi is impractical (server racks). The system utilizes Ethernet connectivity to ensure low-latency 
control via TLS-encrypted WebSockets. It features a native Android application for daily operation and a 
Python-based provisioning tool for streamlined deployment.

The firmware includes dedicated serial protocol implementations for the **Optoma UHD 38x** projector 
(via RS232) and the **Suprema Polaris Pro** projection screen (via RS485).

## Table of content

* [Hardware](#hardware)
* [Software](#software)
* [RS485 data transmission example](#rs485-data-transmission-example)
* [Hardware and software stack](#hardware-and-software-stack)
* [Author](#author)
* [License](#license)

## Hardware

[Click to open schematic in PDF format](.github/schematic/esp32-driver.pdf)

![](.github/schematic/esp32-driver.svg)

The system is powered centrally via an LM2596S switching regulator (5V/3A). This primary rail is
distributed to three independent sections, each powered by a separate AMS1117 linear regulator stepping
voltage down to 3.3V, ensuring noise isolation (5V components like the buzzer and LCD display are powered
directly from the 5V rail). Key communication modules: Ethernet W5500, MAX3232, and MAX3485 - feature
dedicated decoupling circuits consisting of a ceramic capacitor (for high-frequency filtering) paired with
a Low-ESR capacitor. For the Ethernet module, the electrolytic capacitor size was increased to 220uF to
eliminate intermittent connection drops observed with standard 47uF units.

A PCF8574 expander manages I/O for the user interface (buttons, LEDs, buzzer, LCD backlight). Due to the
expander's limited current sourcing capability, most peripherals are ground-controlled (active low). The
control panel supports functions for RESTART, RESET (factory defaults), MENU navigation and screen
backlight toggling (LCD_BLK). The 1602 LCD (white on black) is driven by a modified FC-113 I2C converter
operating at 3.3V logic levels (compatible with the ESP32). Backlight control (anode) is handled via a
MOSFET transistor triggered by pin P3 of the I2C expander.

The RS485 bus, driven by the XY-017 (MAX3485), includes a 122om termination resistor (R10) and a bias
network with 330om resistors pulling line A+ to 3.3V (R8) and line B- to ground (R9). These values were
selected to ensure the idle differential voltage exceeds the +-200mV threshold recommended by RS485/modbus
standards. Both RS232 and RS485 interfaces are protected by TVS diodes (line-to-ground and differential),
safeguarding against voltage spikes induced by equipment such as the projection screen motor. The buzzer
circuit (5V), controlled by 3.3V logic, utilizes a cascaded transistor arrangement with a BC327 (T1) and
BC337 (T2); a diode (D5) is placed in reverse parallel across the buzzer coil to protect the driver
transistor from inductive kickback voltage spikes. Additionally, a passive circuit based on an SN74HC02 NOR
gate allows generating a STOP signal by simultaneously pressing two buttons on a wall-mounted shutter
switch. The RS485 differential signals (A+ and B-) are mapped to pins 4 and 5 of the RJ45 connector to
utilize a twisted pair within the UTP cable, significantly reducing electromagnetic interference
(common-mode noise rejection).

### Prototype (DUT) with test bench

The test setup utilizes a Rigol DHO804 oscilloscope equipped with x10 probes and set to DC coupling.
Channels 1 and 2 capture the differential RS485 signals (A+ and B-) transmitting the `cmd_screen_down`
command to the projection screen, while the RS232 probes (CH3 and CH4) remained disabled for this specific
test sequence.

![](.github/assets/esp32-driver-prototype.jpg)

The bench configuration features a redundant set of converters to allow simultaneous monitoring via the
oscilloscope and PC connection. The RS485 interface uses a standalone USB adapter, whereas the TTL-to-RS232
conversion is handled by a modified MAX3232 module; the stock DB9 connector was manually removed and
replaced with goldpins (mapping pins 5, 3, and 2 to GND, TXD, and RXD respectively) to accommodate the
final design's RJ45 interface. Additionally, to prevent ground loops and double-powering issues - since the
system is already driven by a 9V 3A supply via an LM2596S DC-DC converter - mini USB cable used for ESP
programming and debugging was modified by severing the +5V line, leaving only the ground and data lines
connected. A short top-down video demonstrating the active system is provided below.

https://github.com/user-attachments/assets/9b0b8d48-a2f2-4973-bb6c-5a5791618fee

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

![](.github/assets/esp32-provisioning-tool-demo.gif)

### Android client

[TBD]

## RS485 data transmission example

The screen control process is initiated at the application layer, where the user issues the command
`{"action":"cmd_screen_down"}` via the websocket interface. The ESP32 microcontroller receives, 
interprets, and queues the message, eventually passing it to the transport layer, which transmits the raw 
hexadecimal sequence via UART to the MAX3485 transceiver; the correct formatting of this data packet is 
clearly visible in the HTerm serial monitor.

![](.github/assets/rs485-wss.png)

Declared HEX command sequence directly send via RS485:

```c
static const uint8_t CMD_SCREEN_DOWN[] = {
  0xFF, 0xEE, 0xEE, 0xEE, 0xEE, 0xFF, 0xEE, 0xEE, 0xEE, 0xEE, 0xFF, 0xEE, 0xEE,
  0xEE, 0xEE, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x28, 0xF4, 0x12, 0x00, 0x00
};
```

Received HEX command sequence from MAX3485 module, directly after TVS diode and terminator with bias
(MAX3485 -> terminator with bias -> TVS -> RS485 to USB -> HTerm):

![](.github/assets/rs485-binary.png)

The transceiver converts this logic signal into a differential transmission, verified via oscilloscope 
measurements. As referenced in the hardware section, the use of bias resistors ensures that the 
differential signal (A-B) maintains a stable idle state above +200mV (logic "1"), while reliably dropping 
below the -200mV threshold (to approximately -440mV) during active transmission, ensuring error-free frame 
interpretation by the screen controller.

![](.github/assets/rs485-osci.jpg)

## Hardware and software stack

* ESP32 WROOM DevKit v1,
* MAX3232 (TTL <-> RS232), MAX3485 (TTL <-> RS485), W5500 (LAN),
* I2C, PCF8574, FC-113, LCD 1602 HD44780 (FFSTN),
* LM2596S DC-DC Step-Down 12V -> 5V, AMS1117 DC-DC Step-Down 5V -> 3.3V (power section),
* AHT20+BMP280, BMT1206UXLF,
* 2N7000 (MOSFET), BC337 (NPN), BC327 (PNP), 1N4007, SB2100, P6KE15CA,
* SN74HC02 (NOR),
* ESP-IDF, ESP-LittleFS, C, CMake, Make,
* PKI with CA (MITM attacks protection), basic auth,
* Python (esptool, littlefs-python, cryptography, pyserial, qrcode),
* Android (Kotlin).

## Author

Created by Miłosz Gilga. If you have any questions about this project, send
message: [miloszgilga@gmail.com](mailto:miloszgilga@gmail.com).

## License

This project is licensed under the GNU General Public License v3.0.
