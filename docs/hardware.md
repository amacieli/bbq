# Hardware

ESP32-S3 DevKitC (rev v0.2, 16MB flash, 8MB PSRAM) + Waveshare 4.2" e-paper (SSD1683, 400x300).

Confirmed wiring (user-verified 2026-10-13; unchanged since the Zephyr build). Format: display pin / ESP32-S3 pin.

| Display | ESP32-S3 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| DIN | GPIO 14 |
| CLK | GPIO 13 |
| CS | GPIO 15 |
| DC | GPIO 4 |
| RST | GPIO 3 |
| BUSY | GPIO 5 |

The old `pinout-mappings.md` listed DC/RST/BUSY as 27/26/25 - those don't exist/usable on the S3 and were discarded.

## Gotchas
- Must use **HSPI (SPI3)** for the panel. The default `SPI` (FSPI) never reached the panel on this board
  (BUSY never asserted, refreshes "finished" in ~6 ms, screen blank). Bit-banged and HSPI both work.
- Pass ss=-1 to `begin()`; GxEPD2 drives CS itself.
- Healthy timings in the log: `_Update_Full` ~1.7 s, `_Update_Part` ~0.38 s (values are microseconds). ~6 ms = panel not responding.
- Board is connected via the UART (CH343) USB port -> Serial is UART0.

Temperature probe (3.5mm) and fan driver: not yet specified.
