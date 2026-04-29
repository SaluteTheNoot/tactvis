# TactVis

Real-time 3D tactical situational awareness renderer built in C++ and OpenGL. Ingests live UDP sensor streams and visualizes IFF-classified entities (aircraft, ground vehicles) over procedurally generated terrain with a heads-up display overlay.

![TactVis](https://img.shields.io/badge/C%2B%2B-17-blue) ![OpenGL](https://img.shields.io/badge/OpenGL-4.1-green) ![Platform](https://img.shields.io/badge/platform-macOS-lightgrey)

## Features

- **Multi-threaded pipeline** — dedicated threads for UDP ingestion, entity simulation, and rendering at 60 FPS
- **Dead-reckoning interpolation** — smooth entity movement between 10 Hz sensor packets
- **Custom GLSL shaders** — fBm procedural terrain with elevation tinting + tactical grid, IFF-colored entity billboards, pulsing threat radius effect
- **Dual camera modes** — top-down orthographic tactical map and 3D perspective view (toggle with `V`)
- **Live HUD** — Dear ImGui overlay with entity table, IFF counts, FPS, and camera mode
- **Sensor simulator** — Python UDP broadcaster streaming 8 entities at 10 Hz for testing

## Tech Stack

C++17, OpenGL 4.1, GLSL, GLFW, GLM, Dear ImGui, Python, CMake

## Setup

Requires macOS with Homebrew.

```bash
git clone https://github.com/SaluteTheNoot/tactvis.git
cd tactvis
./setup.sh
```

`setup.sh` installs dependencies (GLFW, GLM), fetches GLAD and Dear ImGui, and runs CMake.

## Build & Run

```bash
# Build
cmake --build build -j$(nproc)

# Terminal 1 — start sensor simulator
python3 sim/sensor_sim.py

# Terminal 2 — launch renderer
./build/tactvis
```

## Controls

| Key | Action |
|---|---|
| `WASD` / Arrow keys | Pan (top-down) / Move (3D) |
| Scroll wheel | Zoom (top-down) / FOV (3D) |
| Left-drag | Pan / Rotate camera |
| `V` | Toggle top-down ↔ 3D |
| `Esc` | Quit |

## Sensor Packet Format

The renderer listens on UDP port 5005 for JSON packets:

```json
{
  "id": 1,
  "callsign": "EAGLE-1",
  "iff": "friendly",
  "type": "aircraft",
  "x": 100.0,
  "y": -200.0,
  "alt": 800.0,
  "heading": 45.0,
  "speed": 90.0
}
```

`iff` — `friendly` | `hostile` | `unknown`  
`type` — `aircraft` | `vehicle` | `waypoint`
