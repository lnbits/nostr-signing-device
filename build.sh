command -v arduino-cli >/dev/null 2>&1 || { echo >&2 "arduino-cli not found. Aborting."; exit 1; }
arduino-cli config --additional-urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json init
arduino-cli core update-index
arduino-cli upgrade
# uBitcoin is broken on esp32 3.x.x
arduino-cli core install esp32:esp32@2.0.17
arduino-cli lib install ArduinoJson uBitcoin Base64 gmp-ino
arduino-cli compile \
    --library ./libraries/TFT_eSPI \
    --library ./libraries/QRCode \
    --library ./libraries/tiny-AES-c \
    --build-path build --fqbn esp32:esp32:esp32 snsd
