#!/bin/sh
if [ -z "$1" ]; then
    echo "Usage: sh build.sh <device>"
    echo "tdisplay is the default device"
    exit 1
fi
uppercase=$(echo $1 | tr '[:lower:]' '[:upper:]')
tft_config=$(sh ./tft_build_flags.sh)
arduino-cli compile \
    --build-property "build.partitions=min_spiffs" \
    --build-property "upload.maximum_size=1966080" \
    --build-property "build.extra_flags.esp32=${tft_config} -D${uppercase}" \
    --library ./libraries/QRCode \
    --build-path build \
    --fqbn esp32:esp32:ttgo-lora32 snsd
    # - source-path: ./libraries/tiny-AES-c
