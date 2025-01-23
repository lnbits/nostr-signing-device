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

void loadBLEMode() {
  FileData bleModeFile = readFile(SPIFFS, global.bleModeFileName.c_str());
  if (bleModeFile.success) {
    global.bleMode = (bleModeFile.data == "1");
  } else {
    global.bleMode = false; // Default to USB mode
  }
}

void loadTapToSignMode() {
  FileData tapToSignFile = readFile(SPIFFS, global.tapToSignFileName.c_str());
  if (tapToSignFile.success) {
    global.tapToSign = (tapToSignFile.data == "1");
  } else {
    global.tapToSign = true; // Default to Enabled
  }
}

void loadColorSwap() {
  FileData colorSwapFile = readFile(SPIFFS, global.colorSwapFileName.c_str());
  if (colorSwapFile.success) {
    global.colorSwap = (colorSwapFile.data == "1");
  } else {
    global.colorSwap = false; // Default to RGB
  }
}

void setup() {
  randomSeed(esp_random());

  // Handle bottom button as wake from sleep
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK((gpio_num_t)BTN_1_PIN), (esp_sleep_ext1_wakeup_mode_t)0);

  // Set LCD Power On PIN to high to enable running on battery
  if(LCD_POWER_ON_PIN >= 0) {
    pinMode(LCD_POWER_ON_PIN, OUTPUT);
    digitalWrite(LCD_POWER_ON_PIN, HIGH);
  }

  // Calculate scale factor for UI elements
  float scaleWidth = REAL_SCREEN_WIDTH / (float)BASE_SCREEN_WIDTH;
  float scaleHeight = REAL_SCREEN_HEIGHT / (float)BASE_SCREEN_HEIGHT;
  global.scaleFactor = min(scaleWidth, scaleHeight);

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

  // Load UI settings
  loadColorSwap();
  loadAccentColor();
  loadDarkMode();

  logInfo("NSD: waiting for commands");

  pinMode(global.button1Pin, INPUT_PULLUP);
  if (global.button1Pin != global.button2Pin) {
    pinMode(global.button2Pin, INPUT_PULLUP);
  }

  // Load device settings
  loadBLEMode();

  if(global.bleMode) {
    setupBLE();
  } else {
    Serial.begin(9600);
    Serial.setRxBufferSize(SERIAL_RX_BUFFER_SIZE);
    Serial.setTxBufferSize(SERIAL_TX_BUFFER_SIZE);
  }

  loadTapToSignMode();

  // Display login screen if PIN is set, or go to logo
  loadPIN();
  displayLoginScreen();
}