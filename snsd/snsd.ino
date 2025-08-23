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
#include <Bitcoin.h>
#include <QRCodeGenerator.h>
#include <bootloader_random.h>
#include <vector>
#include <gmp-ino.h>
#include <mbedtls/base64.h>
#include <mbedtls/aes.h>
#include <map>

fs::SPIFFSFS &FlashFS = SPIFFS;

SHA256 h;
TFT_eSPI tft = TFT_eSPI();

#define DEBUG true

// Pin configuration
#ifdef TDISPLAY
  // TTGO T-Display
  #define BTN_1_PIN 0
  #define BTN_2_PIN 35
  #define BACKLIGHT_PIN 4
  #define REAL_SCREEN_WIDTH 240
  #define REAL_SCREEN_HEIGHT 135
#endif
#ifdef TDISPLAY_S3
  // Lilygo T-Display-S3
  #define BTN_1_PIN 0
  #define BTN_2_PIN 14
  #define BACKLIGHT_PIN 38
  #define REAL_SCREEN_WIDTH 320
  #define REAL_SCREEN_HEIGHT 170
#endif

// Used for UI scaling (do not amend)
#define BASE_SCREEN_WIDTH 240
#define BASE_SCREEN_HEIGHT 135

// Colours
#define TFT_LNBITS_PURPLE 0x63BA

//////////////////////////////// Define and initialize the Global State ////////////////////////////////

struct GlobalState {
  String deviceId;
  String passwordHash;
  unsigned long startTime;
  byte dhe_shared_secret[32];
  bool hasCommandsFile;
  String commands;
  std::vector<String> privateKeys;
  std::map<String, String> keyNames;
  int activeKeyIndex;
  String pinCode;
  int pinAttempts;
  bool unlocked;
  bool onLogo;
  bool isDisplayOn;
  int screenTimeout;
  int debounceDelay;
  float scaleFactor;
  uint16_t accentColor;
  bool darkMode;
  uint16_t foregroundColor;
  uint16_t backgroundColor;
};

// Note: this is not an endorsment for One World Goverment
GlobalState global = {
  "",
  "",
  millis(),
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  false,
  "",
  {},
  {},
  0,
  "00000000",
  0,
  false,
  false,
  true,
  10,
  50,
  0,
  TFT_LNBITS_PURPLE,
  true,
  TFT_WHITE,
  TFT_BLACK,
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
  if(DEBUG){
    Serial.println("/log " + msg);
  }
}
