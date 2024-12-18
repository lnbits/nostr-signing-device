#define FORMAT_ON_FAIL true

void setup() {
  Serial.begin(9600);
  randomSeed(esp_random());

  // Load screen
  tft.init();
  tft.setRotation(1);
  tft.invertDisplay(true);

  h.begin();
  FlashFS.begin(FORMAT_ON_FAIL);
  SPIFFS.begin(true);

  // Migrate from single key to multi key
  if(SPIFFS.exists(global.legacyNostrSecretFileName.c_str())) {
    migrateKey();
  }

  // Load saved keys
  loadKeys();
  loadActiveKeyIndex();

  logInfo("NSD: waiting for commands");

  pinMode(global.button1Pin, INPUT_PULLUP);
  if (global.button1Pin != global.button2Pin) {
    pinMode(global.button2Pin, INPUT_PULLUP);
  }

  // Screen start up sequence
  loadPIN();  
  displayLoginScreen();
}
