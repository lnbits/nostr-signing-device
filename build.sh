#!/bin/sh
if [ -z "$1" ]; then
    echo "Usage: sh build.sh <device>"
    echo "esp32 is the default device"
    exit 1
fi
device_name=$(echo $1 | tr '[:lower:]' '[:upper:]')
tft_config_file="tft_config_$1.txt"
if [ -f "$tft_config_file" ]; then
    echo "Using TFT configuration file $tft_config_file."
    user_tft_config=$(cat $tft_config_file | tr '\n' ' ' | sed -e "s/\ /\ -D/g" -e "s/-D$//")
    tft_font="-DLOAD_GLCD=1 -DLOAD_FONT2=1 -DLOAD_FONT4=1 -DLOAD_FONT6=1 -DLOAD_FONT7=1 -DLOAD_FONT8=1 -DLOAD_GFXFF=1 -DSMOOTH_FONT=1"
    tft_config=" -DTFT=1 -DUSER_SETUP_LOADED=1 -D${user_tft_config} ${tft_font} -DSPI_FREQUENCY=27000000 -DSPI_READ_FREQUENCY=20000000"
fi
arduino-cli compile \
    --build-property "build.partitions=min_spiffs" \
    --build-property "upload.maximum_size=1966080" \
    --build-property "build.extra_flags.esp32=-D${device_name}${tft_config}" \
    --build-path build \
    --fqbn esp32:esp32:ttgo-lora32 snsd
