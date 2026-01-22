# Contributing

> [!TIP]
Make sure you have `make` installed in your system. For windows environments you can use mingw version (`mingw32-make`)
or install natively via Chocolatey via `choco install make`.

## Prepare development environment

1. Create a `.env` file in the root project directory based on `example.env`.
2. Fill it with the appropriate values:

```properties
ESP_IP_ADDRESS=<IP address of ESP32 driver>
ESP_GATEWAY=<gateway address>
ESP_NETMASK=<network mask, ex. for /24: 255.255.255.0>
ESP_DNS=<by default Cloudflare DNS server 1.1.1.1, with you own DNS server supports named domain>
ESP_PORT=<HTTPS and WSS port>
ESP_DEFAULT_PASSWORD=<init password for communication with ESP32 driver by Android app>
```

3. Generate the necessary data files for `esp32_firmware` and the Android app:

```bash
$ make setup
```

### ESP32 firmware development setup

1. Install ESP-IDF v5.5 environment at your machine.
2. Navigate to the firmware directory:

```bash
$ cd esp32_firmware
```

3. Select the target chip:

```bash
$ make set-chip
```

4. To compile the code, flash it to the board, and open the serial monitor in one go:

```bash
$ make run
```

Other commands you will find inside `esp32_firmware/Makefile` file.

>[!TIP]
If you only changed configuration files (LittleFS) and not the code, you can skip the full rebuild via
`$ make flash-data`.

4. If you need to completely wipe the ESP32 flash memory (nvs, data, app), type:

```bash
$ make erase
```

### Prepare Python provisioning tool environment

1. Make sure you have installed at least Python 3.12.
2. Navigate to the tool directory:

```bash
$ cd esp32_provisioning_tool
```

3. Create virtual environment and install dependencies via:

```bash
$ make install
```

4. Run program directly from command via:

```bash
$ make gui
```

4. To create a standalone `.exe` (Windows) or binary file (Linux/macOS):

```bash
$ make build
```

The output file will be located in the `dist/` directory.
