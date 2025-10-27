#!/usr/bin/env bash
# --------- Configuration ---------
PROJECT_DIR=~/Projects/BLDC
BUILD_DIR="$PROJECT_DIR/build"
UF2_FILE="$BUILD_DIR/BLDC_testApp.uf2"
SERIAL_BAUD=115200

# Find Pico serial device
SERIAL_PORT=$(ls /dev/tty.usb* 2>/dev/null | head -n 1)

# --------- Step 1: Build project ---------
echo "Building project..."
cd "$BUILD_DIR" || exit

# Capture make output
BUILD_OUTPUT=$(mktemp)
cmake .. >"$BUILD_OUTPUT" 2>&1
make -j$(nproc) || { echo "Build failed, aborting."; exit 1; }
# --------- Step 2: Flash Pico ---------
if [ ! -f "$UF2_FILE" ]; then
    echo "UF2 file not found: $UF2_FILE"
    exit 1
fi

echo "Flashing Pico..."
picotool load -f  "$UF2_FILE"

# --------- Step 3: Open serial monitor ---------
# Give Pico a second to reboot
sleep 5

# Try to detect the serial port again
SERIAL_PORT=$(ls /dev/tty.usb* 2>/dev/null | head -n 1)

if [ -z "$SERIAL_PORT" ]; then
    echo "No Pico serial device found."
    exit 1
fi

echo "Opening serial monitor on $SERIAL_PORT..."
picocom  "$SERIAL_PORT" "-b" "$SERIAL_BAUD"
