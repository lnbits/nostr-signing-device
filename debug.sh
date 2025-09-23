if [ -z "$1" ]; then
    echo "Usage: ./debug.sh /dev/ttyACM0 tdisplay"
    exit 1
fi
board="esp32:esp32:ttgo-lora32"
sh build.sh $2 && \
arduino-cli upload --input-dir build --fqbn $board -p $1 && \
arduino-cli monitor -p $1 -c baudrate=115200
