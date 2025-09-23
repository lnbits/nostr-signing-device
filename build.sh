#!/bin/sh
if [ -z "$1" ]; then
    echo "Usage: sh build.sh <device>"
    echo "tdisplay is the default device"
    exit 1
fi

board="esp32:esp32:ttgo-lora32"
arduino-cli compile \
    --build-property "build.partitions=min_spiffs" \
    --build-property "upload.maximum_size=1966080" \
    --library ./libraries/TFT_eSPI \
    --library ./libraries/QRCode \
    --library ./libraries/tiny-AES-c \
    --build-path build \
    --fqbn $board snsd
