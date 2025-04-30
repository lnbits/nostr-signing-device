// Forward declarations
struct MenuItem;
void displayMenu(MenuItem* menuItems, int menuSize, String title);

// Define menu item structure
struct MenuItem {
  String name;
  void (*action)();
  MenuItem* subMenu;
  int subMenuSize;
};

// Define menu sizes
const int MAIN_MENU_SIZE = 5;
const int KEY_MANAGEMENT_MENU_SIZE = 6;
const int UI_CONFIG_MENU_SIZE = 4;
const int DEVICE_CONFIG_MENU_SIZE = 6;

// Sub-menu definitions
MenuItem keyManagementMenu[KEY_MANAGEMENT_MENU_SIZE] = {
  {"Show Npub QR Code", displayShowNpubQRCodeScreen, nullptr, 0},
  {"Switch Key", displaySwitchKeyScreen, nullptr, 0},
  {"Generate Key", menuNewKey, nullptr, 0},
  {"Remove Key", displayRemoveKeyScreen, nullptr, 0},
  {"Show Nsec QR Code", displayShowNsecQRCodeScreen, nullptr, 0},
  {"Back", menuReturn, nullptr, 0}
};

MenuItem uiConfigMenu[UI_CONFIG_MENU_SIZE] = {
  {"Change Color", displayColorSelectScreen, nullptr, 0},
  {"", displayToggleDarkMode, nullptr, 0}, // Name will be set dynamically
  {"", displayToggleColorSwap, nullptr, 0}, // Name will be set dynamically
  {"Back", menuReturn, nullptr, 0}
};

MenuItem deviceConfigMenu[DEVICE_CONFIG_MENU_SIZE] = {
  {"", displayToggleBLEMode, nullptr, 0}, // Name will be set dynamically
  {"Lock Device", displayLoginScreen, nullptr, 0},
  {"", displaySetPinScreen, nullptr, 0}, // Name will be set dynamically
  {"Wipe Device", displayWipeDeviceScreen, nullptr, 0},
  {"", displayToggleTapToSign, nullptr, 0}, // Name will be set dynamically
  {"Back", menuReturn, nullptr, 0}
};

// Main menu definition
MenuItem mainMenu[MAIN_MENU_SIZE] = {
  {"Key Management", nullptr, keyManagementMenu, KEY_MANAGEMENT_MENU_SIZE},
  {"UI Config", nullptr, uiConfigMenu, UI_CONFIG_MENU_SIZE},
  {"Device Config", nullptr, deviceConfigMenu, DEVICE_CONFIG_MENU_SIZE},
  {"Sleep", esp_deep_sleep_start, nullptr, 0},
  {"Back", menuReturn, nullptr, 0}
};

// Helper function to display a menu
void displayMenu(MenuItem* menuItems, int menuSize, String title) {
  int selectedIndex = 0;
  int itemHeight = 20;
  int maxVisibleItems = REAL_SCREEN_HEIGHT / itemHeight;
  int startIndex = 0;
  int endIndex = maxVisibleItems - 2;
  unsigned long lastButton1Press = 0;
  unsigned long lastButton2Press = 0;

  // Update dynamic menu items
  if (menuItems == uiConfigMenu) {
    menuItems[1].name = global.darkMode ? "Set Light Mode" : "Set Dark Mode";
    menuItems[2].name = global.colorSwap ? "Set RGB Mode" : "Set BGR Mode";
  } else if (menuItems == deviceConfigMenu) {
    menuItems[0].name = global.bleMode ? "Enable USB" : "Enable BLE";
    menuItems[2].name = global.pinCode == "00000000" ? "Set PIN" : "Change PIN";
    menuItems[4].name = global.tapToSign ? "Disable Tap to Sign" : "Enable Tap to Sign";
  }

  tft.fillScreen(global.backgroundColor);

  while (true) {
    // Draw the menu title
    tft.setTextColor(global.accentColor, global.backgroundColor);
    tft.setTextSize(2);
    tft.setCursor(0, 10);
    tft.println(title);

    // Draw menu items
    for (int i = startIndex; i <= endIndex && i < menuSize; i++) {
      tft.setCursor(0, (i - startIndex) * itemHeight + 35); // Increased Y offset for title
      if (i == selectedIndex) {
        tft.setTextColor(global.accentColor, global.backgroundColor);
        tft.print("> ");
      } else {
        tft.setTextColor(global.foregroundColor, global.backgroundColor);
        tft.print("  ");
      }
      tft.println(padRightWithSpaces(menuItems[i].name, 18));
    }

    EventData event = awaitEvent();

    if (event.type == EVENT_BUTTON_ACTION) {
      String buttonNumber = getWordAtPosition(event.data, 0);
      String buttonState = getWordAtPosition(event.data, 1);

      if (buttonState == "1") {
        unsigned long currentMillis = millis();

        if (buttonNumber == "2") { // Scroll down through options
          if (currentMillis - lastButton2Press > global.debounceDelay) {
            lastButton2Press = currentMillis;

            // Move selection down and wrap using modulo
            selectedIndex = (selectedIndex + 1) % menuSize;

            // Check if we wrapped around to top
            if (selectedIndex == 0) {
              // We reached beyond the last item and wrapped to top
              startIndex = 0;
              endIndex = maxVisibleItems - 2;
            } else if (selectedIndex > endIndex) {
              // Shift the visible window down by one
              startIndex++;
              endIndex++;

              // If we somehow go beyond the last menu item, wrap back to top
              if (endIndex >= menuSize) {
                startIndex = 0;
                endIndex = maxVisibleItems - 2;
              }
            }

            // Ensure we always show exactly maxVisibleItems (adjusting for header)
            if (endIndex - startIndex + 2 > maxVisibleItems) {
              endIndex = startIndex + maxVisibleItems - 2;
            }
          }
        } else if (buttonNumber == "1") { // Select the current option
          if (currentMillis - lastButton1Press > global.debounceDelay) {
            lastButton1Press = currentMillis;
            MenuItem selectedItem = menuItems[selectedIndex];
            
            if (selectedItem.subMenu != nullptr) {
              displayMenu(selectedItem.subMenu, selectedItem.subMenuSize, selectedItem.name);
              tft.fillScreen(global.backgroundColor); // Clear screen after returning
            } else {
              selectedItem.action();
              if (selectedItem.action != menuReturn) {
                return; // Return to previous menu after action
              }
              return; // Exit this menu level
            }
          }
        }
      }
    } else if (event.type == EVENT_SCREEN_IDLE) {
      return;
    }
  }
}

// Main menu function
void menu() {
  global.onLogo = false;
  setDisplay(true);
  displayMenu(mainMenu, MAIN_MENU_SIZE, "Main Menu");
}

// Menu helpers
void menuNewKey() {
  executeNewKey("");
}

void menuReturn() {
  return;
}