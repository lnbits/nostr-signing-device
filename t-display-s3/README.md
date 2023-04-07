# T-Display-S3

This fork and document is designed to track the changes needed and blockers to be resolved for support of the  nostr signing device with the Lilygo ESP32 <a href="https://www.lilygo.cc/products/t-display-s3?variant=42284559827125">T-Display-S3</a> variant.

## Status - NOT FULLY WORKING YET
- [WORKING] - Display 
- [WORKING] - Device pinouts updates
- [WORKING] - Communication with web installer
- [] - SPIFFS Storage
- [] - Sign a payload
 
## Blockers
1) SPIFFS storage w/ ESP32-S3 does not mount correctly upon boot. Firmware builds and flashes but persistent storage fails in the S3. I imagine this needs a custom partition table or better support for the S3 in future Espressif library updates and board support. 

	 ### Serial Output
	```E (552) SPIFFS: spiffs partition could not be found
	/log NSD: waiting for commands
	/log Reading file: /hash.txt
	/log Failed to open file for reading: /hash.txt
	/log Reading file: /mn.txt
	/log Failed to open file for reading: /mn.txt
	/log Reading file: /shared_secret.txt
	/log Failed to open file for reading: /shared_secret.txt
	/log Failed to open files
	/log Reset or 'help'
	/log Reading file: /device_meta.txt
	/log Failed to open file for reading: /device_meta.txt
	/log Writing file: /device_meta.txt
	/log Failed to open file for writing: /device_meta.txt
	/log Nostr
	/log Signing Device
	```
## Environment
- Arduino IDE: 2.0.4
- esp32 by Espressif: 2.0.7

## Arduino
| Setting                  | Value                            |
| :----------------------- | :------------------------------- |
| USB CDC On Boot          | Enabled                          |
| Core Debug Level         | None                             |
| USB DFU On Boot          | Disabled                         |
| Events Run On            | Core 1                           |
| JTAG Adapter             | Disabled.                        |
| Arduino Runs On          | Core 1                           |
| USB Firmware MSC On Boot | Disabled                         |
| Partition Scheme         | 16M Flash (3MB APP/9.9MB FATFS   |
| UART Mode                | UART0/Hardware CDC               |
| USB Mode                 | Hardware CDC and JTAG            |

## T-Display-S3 Pinout
![](T-DISPLAY-S3.jpg)
