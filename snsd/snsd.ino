/**
   Very cheap little Nostr Signing Device for use with lilygo TDisplay
   although with little tinkering any ESP32 will work

   Join us!
   https://t.me/lnbits
   https://t.me/makerbits
*/

#include <SPIFFS.h>
#include <TFT_eSPI.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "Bitcoin.h"
#include "qrcoded.h"
#include <aes.h>
#include "bootloader_random.h"
#include <vector>
#include <gmp-ino.h>
#include "mbedtls/base64.h"

fs::SPIFFSFS &FlashFS = SPIFFS;

SHA256 h;
TFT_eSPI tft = TFT_eSPI();

// Pins
#define button1PinNumber 0
#define button2PinNumber 35
#define backlightPinNumber 4

// Colours
#define TFT_LNBITS_PURPLE 0x63BA

//////////////////////////////// Define and initialize the Global State ////////////////////////////////

struct GlobalState {
  bool debug;
  String deviceId;
  String passwordHash;
  unsigned long startTime;
  int button1Pin;
  int button2Pin;
  int backlightPin;
  byte dhe_shared_secret[32];
  bool hasCommandsFile;
  String commands;
  std::vector<String> privateKeys;
  int activeKeyIndex;
  String pinCode;
  int pinAttempts;
  bool unlocked;
  bool onLogo;
  bool isDisplayOn;
  int screenTimeout;
  int debounceDelay;
  String pinFileName;
  String pinAttemptsFileName;
  String privateKeysFileName;
  String activeKeyIndexFileName;
  String legacyNostrSecretFileName;
};

// Note: this is not an endorsment for One World Goverment
GlobalState global = {
  false,
  "",
  "",
  millis(),
  button1PinNumber,
  button2PinNumber,
  backlightPinNumber,
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  false,
  "",
  {},
  0,
  "00000000",
  0,
  false,
  false,
  true,
  10,
  50,
  "/pin.txt",
  "/pinattempts.txt",
  "/private_keys.txt",
  "/active_key_index.txt",
  "/nostr-secret.txt"
};

////////////////////////////////           Global State End            ////////////////////////////////

//////////////////////////////// Define and initialize Environment Variables ////////////////////////////////
struct EnvironmentVariables {
  String version;
};

EnvironmentVariables env = {
  "20241218.1313",
};
////////////////////////////////           Env Vars End            ////////////////////////////////

struct EventData {
  String type;
  String data;
};

struct FileData {
  bool success;
  String data;
};

struct Command {
  String cmd;
  String data;
};

struct CommandResponse {
  String message;
  String subMessage;
  int statusCode;
  EventData event; // valid event that the command does not handle
};

// do not move/remove, arduino IDE bug
// at least one function definition is require after `struct` declaration
void logInfo(const String msg) {
  if(global.debug){
    Serial.println("/log " + msg);
  }
}
