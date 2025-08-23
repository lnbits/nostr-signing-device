#define FORMAT_ON_FAIL true

void loadDarkMode() {
  FileData darkModeFile = readFile(SPIFFS, FILE_DARKMODE);
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
  Serial.begin(115200);
  Serial.print("Welcome to the LNbits Nostr signing device!");
  Serial.println(" (" + String(VERSION) + ")");

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
  if(SPIFFS.exists(FILE_LEGACY_NOSTR_SECRET)) {
    migrateKey();
  }

  // Load saved keys
  loadKeys();
  loadActiveKeyIndex();
  loadAccentColor();

  logInfo("NSD: waiting for commands");

  pinMode(BTN_1_PIN, INPUT_PULLUP);
  if (BTN_1_PIN != BTN_2_PIN) {
    pinMode(BTN_2_PIN, INPUT_PULLUP);
  }

  // Screen start up sequence
  loadPIN();
  loadDarkMode();
  displayLoginScreen();
}
