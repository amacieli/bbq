#pragma once
// Waveshare 4.2" e-paper (SSD1683) -> ESP32-S3 DevKitC. Wiring carried over from the working Zephyr build.
#define EPD_MOSI 14   // DIN
#define EPD_SCK  13   // CLK
#define EPD_CS   15
#define EPD_DC    4
#define EPD_RST   3   // GPIO3 is a strapping pin; fine as an output after boot
#define EPD_BUSY  5
