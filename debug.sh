if [ -z "$1" ]; then
    echo "Usage: ./debug.sh /dev/ttyACM0 esp32"
    exit 1
fi
sh build.sh $2 && \
arduino-cli upload --input-dir build --fqbn esp32:esp32:ttgo-lora32 -p $1 && \
arduino-cli monitor -p $1 -c baudrate=115200
