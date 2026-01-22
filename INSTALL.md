# Installation and provisioning

1. Go to the GitHub Releases page and download the `esp32-provisioning-tool` version appropriate for your operating
system.

> [!NOTE]
If a pre-compiled binary for your OS is not available, please refer to [CONTRIBUTING.md](./CONTRIBUTING.md) for build
instructions. This is a GUI application and requires a desktop environment to run.

2. Connect the controller to your computer and verify which COM port has been assigned to it. Launch the provisioning
tool.

3. Download the latest ESP32 firmware (the `.bin` file) from GitHub Releases. In the provisioning tool, select the file
path for the downloaded firmware and check the option to flash the ESP32.

4. Enter your Ethernet connection parameters and the device password in the tool, then click RUN PROVISIONING. The
software will automatically:
* create a LittleFS partition,
* populate it with configuration data and generate cryptographic keys,
* flash the firmware onto the ESP32.

5. Once the process is complete, a QR code will appear on your screen (you can save this as an image). Scan this code
using the Android app to pair the controller with your mobile device.

6. Successful pairing is confirmed by three quick beeps from the buzzer and a notification on the controller's LCD
screen. You can now fully manage the controller via the app.

### Important notes:

* The controller can be paired with only one app instance at a time. You must remove the previous pairing before
connecting a new device.
* If disconnected, the app will attempt to reconnect. The wait time between attempts increases with each failed retry
(backoff strategy).
* The app remembers the last paired controller. If it cannot connect upon launch, the status will show as
**DISCONNECTED**.
* Communication uses a fully encrypted channel. By utilizing TOFU (Trust On First Use) during the initial pairing, the
connection is immune to Man-In-The-Middle (MITM) attacks from the very first connection.
