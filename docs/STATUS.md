# BBQ controller - status (updated 2026-10-13)

## Done / verified on hardware
- Toolchain: PlatformIO + Arduino-ESP32 (Zephyr abandoned; its Wi-Fi/WPA3 and display stack were blockers). Linux only, no WSL.
- Wi-Fi joins UniFi LAN (modem sleep disabled so unit is reachable). Unit at http://<ip>/ (was 192.168.1.74, DHCP).
- Web UI: live temp/setpoint/fan via SSE (/events; needs `Accept: text/event-stream`), setpoint +/- via POST /api/setpoint?delta=N.
- E-paper (Waveshare 4.2", GDEY042T81/SSD1683) landscape 400x300, 92px temperature (U8g2 logisoso92), partial refresh every 5 s,
  full refresh ~10 min. Version stamp `vYYYYMMDD-HH-MM` on screen, serial, heartbeat (standing rule: keep until app is finished).
- Display needs HSPI (see docs/hardware.md gotchas). Wiring confirmed, see docs/hardware.md.
- PSRAM enabled (8MB). Repo: github.com/amacieli/bbq (main).

## Simulated / not built yet
- Temperature is SIMULATED. No fan output, no PID, no settings storage, no Wi-Fi provisioning, no chart, no alarm. See docs/Specifications.txt.

## Hardware decisions
- Pit probe + fan reused from user's BBQ Guru DigiQ DX3 (Pit Viper fan). Probe cable is 3.5mm MONO -> single NTC thermistor.
  OEM spare: BBQ Guru Premium 6ft Pit Probe. Priority = pit temperature; food probes are a later bonus (more ADC channels).
- ADC: ADS1115 16-bit I2C module (user's chosen listing: 2-pack, Amazon B0DZSP4CG8, 4 channels, 2.0-5.5V). Power at 3.3V.
- Fan driver, depends on fan type (UNRESOLVED - the Pit Viper may be an AC fan):
  - DC fan: dual-MOSFET trigger module (user's candidate: Amazon B0FMJH3DML, 6-pack; logic 3.3-20V OK from ESP32, load 5-36V DC, 15A) + flyback diode (1N4007/SS14) across the fan. ESP32 GND must be common with the load supply.
  - AC fan: MOSFET module is NOT suitable; would need relay or SSR (mains wiring, enclosure/safety considerations).

## Waiting on user (measurements)
1. Pit probe resistance at room temp, ice water (0C), boiling (~100C) -> Steinhart-Hart + reference resistor value.
2. Fan voltage/current (label of fan or DigiQ adapter), connector wiring.

## Research to-do
- **USB-C powered fan option**: find a fan that runs from a typical USB-C power adapter (5V direct, or 9/12V via a USB-C PD trigger/decoy module).
  Would remove mains from the controller entirely (safer, simpler enclosure) and pairs with the MOSFET module. Need: suitable airflow for a
  Big Green Egg intake (compare vs Pit Viper), current draw vs adapter rating, mounting/adapter to the egg's draft door, noise, cost.
  Compare against reusing the Pit Viper (AC vs DC TBD from its label).

## Next steps
1. Make temperature source swappable (sim vs thermistor); add PID (time-proportioned, ~10s window) + fan output (GPIO TBD).
2. ADS1115 + thermistor code once measurements are in.
3. RAM ring buffer + chart, settings in NVS, Wi-Fi provisioning portal, alarm, local setpoint buttons/encoder (hardware TBD).
