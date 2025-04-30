#define FORMAT_ON_FAIL true

void loadDarkMode() {
  FileData darkModeFile = readFile(SPIFFS, global.darkModeFileName.c_str());
  if (darkModeFile.success) {
    global.darkMode = (darkModeFile.data == "1");
  } else {
    global.darkMode = true; // Default to dark mode
  }

  // Set colors based on mode
  if (global.darkMode) {
    global.foregroundColor = TFT_WHITE;
    global.backgroundColor = TFT_BLACK;
  } else {
    global.foregroundColor = TFT_BLACK;
    global.backgroundColor = TFT_WHITE;
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(esp_random());

  // Calculate scale factor for UI elements
  float scaleWidth = REAL_SCREEN_WIDTH / (float)BASE_SCREEN_WIDTH;
  float scaleHeight = REAL_SCREEN_HEIGHT / (float)BASE_SCREEN_HEIGHT;
  global.scaleFactor = min(scaleWidth, scaleHeight);

  // Load screen
  tft.init();
  tft.setRotation(1);
  tft.invertDisplay(true);

  showMessage("Booting...", "please wait");

  h.begin();
  FlashFS.begin(FORMAT_ON_FAIL);
  SPIFFS.begin(true);

  // Full screen clear
  tft.fillScreen(global.backgroundColor);

  // Migrate from single key to multi key
  if(SPIFFS.exists(global.legacyNostrSecretFileName.c_str())) {
    migrateKey();
  }

  // Load saved keys
  loadKeys();
  loadActiveKeyIndex();
  loadAccentColor();

  logInfo("NSD: waiting for commands");

  pinMode(global.button1Pin, INPUT_PULLUP);
  if (global.button1Pin != global.button2Pin) {
    pinMode(global.button2Pin, INPUT_PULLUP);
  }

  // Screen start up sequence
  loadPIN();
  loadDarkMode();
  displayLoginScreen();
}
