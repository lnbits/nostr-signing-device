#define FORMAT_ON_FAIL true

void setup() {
  Serial.begin(9600);

  // load screen
  tft.init();
  tft.setRotation(1);
  tft.invertDisplay(true);

  h.begin();
  FlashFS.begin(FORMAT_ON_FAIL);
  SPIFFS.begin(true);

  logInfo("SNSD: waiting for commands");

  if (loadFiles() == false)
    showMessage("Failed to open files",  "Reset or 'help'");
  updateDeviceConfig();
}

bool loadFiles() {
  FileData pwdFile = readFile(SPIFFS, global.passwordFileName.c_str());
  String passwordHash = pwdFile.data;

  int byteSize =  passwordHash.length() / 2;
  byte passwordHashBin[byteSize];
  fromHex(passwordHash, passwordHashBin, byteSize);

  FileData mnFile = readFile(SPIFFS, global.mnemonicFileName.c_str());
  global.mnemonic = decryptDataWithIv(passwordHashBin, mnFile.data);
  global.passwordHash = passwordHash;

  FileData sharedSecretFile = readFile(SPIFFS, global.sharedSecretFileName.c_str());
  if (sharedSecretFile.success) {
    fromHex(sharedSecretFile.data, global.dhe_shared_secret, sizeof(global.dhe_shared_secret));
  }

  return mnFile.success && pwdFile.success;
}

void updateDeviceConfig() {
  FileData deviceMetaFile = readFile(SPIFFS, global.deviceMetaFileName.c_str());

  if (deviceMetaFile.success) {
    global.deviceId = getWordAtPosition(deviceMetaFile.data, 0);

    String button1PinStr = getWordAtPosition(deviceMetaFile.data, 1);
    if (button1PinStr && button1PinStr != "") {
      global.button1Pin = button1PinStr.toInt();
    }

    String button2PinStr = getWordAtPosition(deviceMetaFile.data, 2);
    if (button2PinStr && button2PinStr != "") {
      global.button2Pin = button2PinStr.toInt();
    }
  } else {
    // create random unique ID
    int uuidSize = 32;
    uint8_t uuid[uuidSize];
    String tempMnemonic = generateStrongerMnemonic(24);
    mnemonicToEntropy(tempMnemonic, uuid, uuidSize);
    global.deviceId = toHex(uuid, uuidSize);
    writeFile(SPIFFS, global.deviceMetaFileName.c_str(), global.deviceId);
  }

  pinMode(global.button1Pin, INPUT_PULLUP);
  if (global.button1Pin != global.button2Pin) {
    pinMode(global.button2Pin, INPUT_PULLUP);
  }
}
