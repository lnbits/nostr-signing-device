# DIY Nostr signing device (powered by <a href="https://www.arduino.cc/reference/en/libraries/ubitcoin/">uBitcoin</a>)

## Flash here https://lnbits.github.io/nostr-signing-device

<img style="width:500px;" src="https://i.imgur.com/wlV5isa.png">

For use with [https://github.com/fiatjaf/horse](https://github.com/fiatjaf/horse). Chrome Extension [here](https://chrome.google.com/webstore/detail/horse/ogdjeglchjlenflecdcoonkngmmipcoe)

Join us <a href="https://t.me/nostr_protocol">t.me/nostr_protocol</a>, <a href="https://t.me/lnbits">t.me/lnbits</a>, <a href="https://t.me/makerbits">t.me/makerbits</a>

This very cheap off the shelf nostr signing device is designed to work with Lilygos <a href="https://www.aliexpress.com/item/33048962331.html">Tdisplay</a>, but you can easily make work with any ESP32.

Data is sent to/from the **Signing Device** over webdev Serial, not the most secure data transmission method, but x1000 better than storing the privatekey on a computer. Clients could support sending to the device directly, but using is Nos2X is far more convenient.

## Install instructions
- Flash the hardware-wallet firmware directly from the browser using the [installer](https://lnbits.github.io/nostr-device)
## Build instructions

- Buy a Lilygo <a href="https://www.aliexpress.com/item/33048962331.html">Tdisplay</a> (although with a little tinkering any ESP32 will do) 
- Install <a href="https://www.arduino.cc/en/software">Arduino IDE 1.8.19</a>
- Install ESP32 boards, using <a href="https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-boards-manager">boards manager</a>
- Download this repo
- Copy these <a href="libraries">libraries</a> into your Arduino install "libraries" folder
- Open this <a href="snsd/snsd.ino">snsd.ino</a> file in the Arduino IDE
- Select "TTGO-LoRa32-OLED-V1" from tools>board
- Upload to device

## How to use
// Guide to go here

> _Note: If using MacOS, you will need the CP210x USB to UART Bridge VCP Drivers available here https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers_
> If you are using **MacOS Big Sur or an Mac with M1 chip**, you might encounter the issue `A fatal error occurred: Failed to write to target RAM (result was 0107)`, this is related to the chipset used by TTGO, you can find the correct driver and more info in this <a href="https://github.com/Xinyuan-LilyGO/LilyGo-T-Call-SIM800/issues/139#issuecomment-904390716">GitHub issue</a>
