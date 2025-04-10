#!/usr/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: $0 <use case> <usb port>"
    exit 1
fi

SCRIPT_FILE="$0"
USE_CASE="$1"
PORT_NAME="$2"

BIN_NAME="m55_hp_$USE_CASE"
SCRIPT_DIR="$(dirname $SCRIPT_FILE)"
MLEK_DIR="$SCRIPT_DIR/../../../.."
INFRUN_JSON="$SCRIPT_DIR/config/$BIN_NAME.json"

BUILD_DIR="$MLEK_DIR/builds/ensemble-hp"
INFRUN_BIN="$BUILD_DIR/bin/sectors/$USE_CASE/mram.bin"

SETOOLS_DIR="$HOME/app-release-exec-linux"
SETOOLS_IMAGE="$SETOOLS_DIR/build/images/$BIN_NAME.bin"
SETOOLS_CONFIG="$SETOOLS_DIR/build/config/$BIN_NAME.json"

# copy binary and configuration to setools directory
set -x
cp "$INFRUN_BIN" "$SETOOLS_IMAGE"
cp "$INFRUN_JSON" "$SETOOLS_CONFIG"
cd "$SETOOLS_DIR"

# generate application image
./app-gen-toc -f "$SETOOLS_CONFIG"

# erase applications from mram
echo "$PORT_NAME" | ./app-write-mram -e app -d

# write application to mram
echo "$PORT_NAME" | ./app-write-mram -p -d
set +x
