# waveshare-esp32-touch-lcd-4.3b-platformio

This is a proof of concept to get a selected set of the features of the
[Waveshare ESP32-S3-Touch-LCD-4.3B][1] board working in a [PlatformIO][6] project.

A lot of the examples and sample projects online copy a lot of code from the
Waveshare examples and dependencies into the the repo.  Support for the board
has improved so the aim here is to use standard published libraries only.

I'm using the LVGL template files unchanged so instead of copying them to the
`src` directory, the `copy_templates.py` `pre:` script copies them to a
dependency directory.

## Features
The working features are:
* WiFi
* LVGL
* Logging
* Development and Production Builds

The features still to add are:
* WiFi
* Touchscreen
* Backlight adjustment
* Clock
* Remote Debugging
* HTTP Requests
* JSON Parsing
* [Squareline Studio][2] UI

The excluded features are:
* Bluetooth
* Working with the SD Card

# References

* [ESP32_Display_Panel supported Waveshare boards page][3]
* [ESP32_Display_Panel LVGL example code][4]
* [PlatformIO Registry][5]

[1]: https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B
[2]: https://squareline.io
[3]: https://github.com/esp-arduino-libs/ESP32_Display_Panel/blob/master/docs/board/board_waveshare.md
[4]: https://github.com/esp-arduino-libs/ESP32_Display_Panel/blob/master/examples/platformio/lvgl_v8_port/src/main.cpp
[5]: https://registry.platformio.org
[6]: https://platformio.org