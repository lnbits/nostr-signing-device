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
#include <qrcoded.h>
#include <aes.h>
#include <bootloader_random.h>
#include <vector>
#include <gmp-ino.h>
#include <mbedtls/base64.h>
#include <map>

fs::SPIFFSFS &FlashFS = SPIFFS;

SHA256 h;
TFT_eSPI tft = TFT_eSPI();

/**
  IMPORTANT: Notes for compiling

  TTGO T-Display:
    Board in Arduino IDE = TTGO LoRa32-OLED
    TFT_eSPI = In User_Setup_Select.h uncomment - #include <User_Setups/Setup25_TTGO_T_Display.h>

  Lilygo T-Display-S3
    Board in Arduino IDE = ESP32S Dev Module
    TFT_eSPI = In User_Setup_Select.h uncomment - #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>

  IMPORTANT: Ensure that SPIFFS partition scheme is used!
*/

// Set BOARD_TYPE to 1 (TTGO T-Display) or 2 (Lilygo T-Display-S3)
#define BOARD_TYPE 1

// Pin configuration
#if BOARD_TYPE == 1
  // TTGO T-Display
  #define BTN_1_PIN 0
  #define BTN_2_PIN 35
  #define BACKLIGHT_PIN 4
  #define REAL_SCREEN_WIDTH 240
  #define REAL_SCREEN_HEIGHT 135
#elif BOARD_TYPE == 2
  // Lilygo T-Display-S3
  #define BTN_1_PIN 0
  #define BTN_2_PIN 14
  #define BACKLIGHT_PIN 38
  #define REAL_SCREEN_WIDTH 320
  #define REAL_SCREEN_HEIGHT 170
#else
  #error "Unsupported BOARD_TYPE. Please set BOARD_TYPE to 1 (TTGO T-Display) or 2 (Lilygo T-Display-S3)."
#endif

// Used for UI scaling (do not amend)
#define BASE_SCREEN_WIDTH 240
#define BASE_SCREEN_HEIGHT 135

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
  std::map<String, String> keyNames;
  String keyNamesFileName;
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
  float scaleFactor;
  uint16_t accentColor;
  String accentColorFileName;
  bool darkMode;
  uint16_t foregroundColor;
  uint16_t backgroundColor;
  String darkModeFileName;
};

// Note: this is not an endorsment for One World Goverment
GlobalState global = {
  false,
  "",
  "",
  millis(),
  BTN_1_PIN,
  BTN_2_PIN,
  BACKLIGHT_PIN,
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  false,
  "",
  {},
  {},
  "/key_names.txt",
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
  "/nostr-secret.txt",
  0,
  TFT_LNBITS_PURPLE,
  "/accent_color.txt",
  true,
  TFT_WHITE,
  TFT_BLACK,
  "/dark_mode.txt"
};

////////////////////////////////           Global State End            ////////////////////////////////

//////////////////////////////// Define and initialize Environment Variables ////////////////////////////////
struct EnvironmentVariables {
  String version;
};

EnvironmentVariables env = {
  "20250109.1219",
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
