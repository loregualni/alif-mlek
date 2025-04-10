#!/usr/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: $0 <use case> <tflite model>"
    exit 1
fi

SCRIPT_FILE="$0"
USE_CASE="$1"
MODEL_FILE="$2"

SCRIPT_DIR="$(dirname $SCRIPT_FILE)"
MLEK_DIR="$SCRIPT_DIR/../../../.."
BUILD_DIR="$MLEK_DIR/builds/ensemble-hp"

# create building scripts
set -x
cmake -B "$BUILD_DIR" -G "Ninja" \
    -DUSE_CASE_BUILD="$USE_CASE" \
    -D"$USE_CASE"_MODEL_TFLITE_PATH="$MODEL_FILE" \
    -DTARGET_PLATFORM="ensemble" \
    -DTARGET_SUBSYSTEM="RTSS-HP" \
    -DTARGET_BOARD="DevKit" \
    -DTARGET_REVISION="B" \
    -DLINKER_SCRIPT_NAME="ensemble-RTSS-HP-infrun" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCONSOLE_UART="4" \
    -DVERIFY_TEST_OUTPUT="ON" \
    -DGLCD_UI="OFF"

# build project
cmake --build "$BUILD_DIR" -j$(nproc)
set +x
