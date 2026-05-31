# Wireless Controller Projects

This repository contains two related projects for wireless controller development:

- `esp32_controle_wireless`: ESP32 firmware using PlatformIO that implements a wireless controller and UDP communication.
- `pygame_and_udp_keyboard`: A Python-based input/visualization project that sends or receives controller data over UDP.

## Repository Structure

- `esp32_controle_wireless/`
  - `platformio.ini` - PlatformIO project configuration.
  - `include/` - Project header files.
  - `lib/` - Libraries used by the firmware (EEPROM, globals, UDP, webserver, etc.).
  - `src/main.cpp` - Firmware entry point.

- `pygame_and_udp_keyboard/`
  - `main.py` - Python entry point for the desktop controller or visualizer.
  - `requeriments.txt` - Python dependencies (note: filename has a typo in the repository).
  - `udp_keyboard.py`, `assets.py`, `general_pad.py` - helper modules.

## Prerequisites

- For ESP32 firmware:
  - PlatformIO installed (VS Code PlatformIO extension or `pip install platformio`).
  - ESP32 toolchain supported by PlatformIO.

- For the Python project:
  - Python 3.8+ installed.
  - A virtual environment is recommended.

## Build and Upload (ESP32 firmware)

1. Open the `esp32_controle_wireless` folder in VS Code with PlatformIO.
2. Build the project:

```bash
cd esp32_controle_wireless
pio run
```

3. Upload to a connected ESP32:

```bash
pio run -t upload
```

If you prefer PlatformIO's GUI in VS Code, use the Build and Upload buttons in the PlatformIO toolbar.

## Run the Python controller/visualizer

1. Create and activate a virtual environment:

```bash
cd pygame_and_udp_keyboard
python -m venv .venv
# Windows
.\.venv\Scripts\activate
# Unix/macOS
source .venv/bin/activate
```

2. Install dependencies (repository file is `requeriments.txt`):

```bash
pip install -r requeriments.txt
```

3. Run the application:

```bash
python main.py
```

Adjust configuration inside `config.py` or `udp.py` if you need to change target IP/ports or behavior.

## Configuration

- Firmware settings and network parameters are located in the firmware sources under `esp32_controle_wireless/include` and `src` files.
- Python-side configuration is under `pygame_and_udp_keyboard/core/config.py` and `pygame_and_udp_keyboard/udp.py`.

## Notes

- The Python requirements file is named `requeriments.txt` in the repository. If you rename it to `requirements.txt`, update commands accordingly.
- The ESP32 firmware uses PlatformIO; ensure your board and upload port are configured in `platformio.ini` before uploading.
