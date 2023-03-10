/**
   Very cheap little bitcoin HWW for use with lilygo TDisplay
   although with little tinkering any ESP32 will work

   Join us!
   https://t.me/lnbits
   https://t.me/makerbits

*/

#include <FS.h>
#include <SPIFFS.h>

#include <Wire.h>
#include <TFT_eSPI.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "Bitcoin.h"
#include "PSBT.h"
#include "qrcoded.h"

#include <aes.h>

#include <FS.h>
#include <SPIFFS.h>

#include "bootloader_random.h"

#include "SD.h"
#include "SPI.h"


fs::SPIFFSFS &FlashFS = SPIFFS;

SHA256 h;
TFT_eSPI tft = TFT_eSPI();


// SD Cars
#define SD_MISO     2
#define SD_MOSI     15
#define SD_SCLK     17
#define SD_CS       13


// Buttons
#define button1PinNumber 0
#define button2PinNumber 35


//////////////////////////////// Define and initialize the Global State ////////////////////////////////

struct GlobalState {
  String deviceId;
  bool authenticated;
  bool persistSecrets;
  String passwordHash;
  String mnemonic;
  String passphrase;
  unsigned long startTime;
  const String passwordFileName;
  const String mnemonicFileName;
  const String nostrFileName;
  const String sharedSecretFileName;
  const String deviceMetaFileName;
  int button1Pin;
  int button2Pin;
  byte dhe_shared_secret[32];
  // sd card
  bool hasCommandsFile;
  String commands;
  const String commandsInFileName;
  const String commandsOutFileName;
  const String commandsLogFileName;
};

// Note: this is not an endorsment for One World Goverment
GlobalState global = {
  "",
  false,
  true,
  "",
  "",
  "",
  millis(),
  "/hash.txt",
  "/mn.txt",
  "/nostr-secret.txt",
  "/shared_secret.txt",
  "/device_meta.txt",
  button1PinNumber,
  button2PinNumber,
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  false,
  "",
  "/commands.in.txt",
  "/commands.out.txt",
  "/commands.log.txt",
};

////////////////////////////////           Global State End            ////////////////////////////////

//////////////////////////////// Define and initialize Environament Variables ////////////////////////////////
struct EnvironmentVarialbes {
  String version;
};

EnvironmentVarialbes env = {
  "0.1",
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

struct HwwInitData {
  String passwordHash;
  String mnemonic;
  bool success;
};

// do not move/remove, arduino IDE bug
// at least one function definition is require after `struct` declaration
void logInfo(const String msg) {
  logInfoFile(msg);
  logInfoSerial(msg);
}

void logInfoFile(const String msg) {
  if (global.hasCommandsFile == true) {
    Serial.println("/log logInfoFile: " + global.commandsLogFileName + " msg: " + msg);
    appendFile(SD, global.commandsLogFileName.c_str(), msg + "\n");
  }
}

void logInfoSerial(const String msg) {
  Serial.println("/log " + msg);
}
