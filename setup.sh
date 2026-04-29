#!/usr/bin/env bash
# One-shot setup: installs deps via Homebrew, clones vendors, and configures CMake.
set -e

echo "=== TactVis Setup ==="

# --- Homebrew deps ---
echo "[1/4] Installing Homebrew packages..."
brew install cmake glfw glm

# --- GLAD (OpenGL loader) ---
echo "[2/4] Fetching GLAD..."
GLAD_DIR="vendor/glad"
if [ ! -d "$GLAD_DIR" ]; then
    mkdir -p "$GLAD_DIR/include/glad" "$GLAD_DIR/include/KHR" "$GLAD_DIR/src"
    curl -sL "https://raw.githubusercontent.com/Dav1dde/glad/v0.1.36/include/glad/glad.h" \
         -o "$GLAD_DIR/include/glad/glad.h"
    curl -sL "https://raw.githubusercontent.com/Dav1dde/glad/v0.1.36/include/KHR/khrplatform.h" \
         -o "$GLAD_DIR/include/KHR/khrplatform.h"
    curl -sL "https://raw.githubusercontent.com/Dav1dde/glad/v0.1.36/src/glad.c" \
         -o "$GLAD_DIR/src/glad.c"
    echo "  GLAD downloaded."
else
    echo "  GLAD already present, skipping."
fi

# --- Dear ImGui ---
echo "[3/4] Fetching Dear ImGui..."
IMGUI_DIR="vendor/imgui"
if [ ! -d "$IMGUI_DIR" ]; then
    git clone --depth 1 --branch v1.90.4 https://github.com/ocornut/imgui.git "$IMGUI_DIR"
    echo "  ImGui cloned."
else
    echo "  ImGui already present, skipping."
fi

# --- CMake configure ---
echo "[4/4] Configuring CMake build..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
echo ""
echo "=== Setup complete! ==="
echo ""
echo "To build:  cmake --build build -j\$(nproc)"
echo "To run:    ./build/tactvis"
echo ""
echo "In a separate terminal, start the simulator:"
echo "  python3 sim/sensor_sim.py"
echo ""
echo "Controls:"
echo "  WASD / Arrow keys  — pan / move camera"
echo "  Scroll wheel       — zoom (top-down) / FOV (3D)"
echo "  Left-drag          — rotate / orbit"
echo "  V                  — toggle top-down ↔ 3D"
echo "  Esc                — quit"
