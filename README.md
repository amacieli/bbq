# BBQ controller

ESP32-S3 + Waveshare 4.2" e-paper + Wi-Fi web UI (live via SSE) + PID-driven fan for a Big Green Egg.
Spec: [docs/Specifications.txt](docs/Specifications.txt). Pins: [docs/hardware.md](docs/hardware.md).

## Develop (Linux)
```
uv tool install platformio        # once
cp src/secrets.h.example src/secrets.h   # edit Wi-Fi creds
pio run                           # build
pio run -t upload                 # flash
pio device monitor                # serial, 115200
```
Needs serial access: user must be in the `uucp` group.
