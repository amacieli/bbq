# Hardware

ESP32-S3 DevKitC (rev v0.2, 16MB flash, 8MB PSRAM) + Waveshare 4.2" e-paper (SSD1683, 400x300).

| E-paper | ESP32-S3 GPIO |
|---|---|
| DIN (MOSI) | 14 |
| CLK | 13 |
| CS | 15 |
| DC | 4 |
| RST | 3 |
| BUSY | 5 |
| VCC / GND | 3V3 / GND |

These are the pins used by the earlier Zephyr build (display worked). The old `pinout-mappings.md` listed DC/RST/BUSY as 27/26/25 - those
don't apply to the S3 (GPIO26+ are flash/PSRAM) and were discarded.

Temperature probe (3.5mm) and fan driver: not yet specified.
