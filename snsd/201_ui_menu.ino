void menu() {
  global.onLogo = false;
  setDisplay(true);

  // Define menu items and their associated actions
  struct MenuItem {
    String name;
    void (*action)();
  };

  MenuItem menuItems[] = {
    {"Show Npub QR Code", displayShowNpubQRCodeScreen},
    {"Switch Key", displaySwitchKeyScreen},
    {"Generate Key", menuNewKey},
    {"Remove Key", displayRemoveKeyScreen},
    {"Lock Device", displayLoginScreen},
    {global.pinCode == "00000000" ? "Set PIN" : "Change PIN", displaySetPinScreen},
    {"Change Color", displayColorSelectScreen},
    {global.darkMode ? "Light Mode" : "Dark Mode", displayToggleDarkMode},
    {"Wipe Device", displayWipeDeviceScreen},
    {"Show Nsec QR Code", displayShowNsecQRCodeScreen},
    {"Back", menuReturn}
  };

  int menuSize = sizeof(menuItems) / sizeof(MenuItem);
  int selectedIndex = 0;

  // Dynamically calculate the number of items that can be displayed
  int itemHeight = 20;
  int maxVisibleItems = REAL_SCREEN_HEIGHT / itemHeight;

  int startIndex = 0;
  int endIndex = maxVisibleItems - 1;

  unsigned long lastButton1Press = 0;
  unsigned long lastButton2Press = 0;

  // Full screen clear
  tft.fillScreen(global.backgroundColor);

  while (true) {
    // Draw the menu
    tft.setTextColor(global.foregroundColor, global.backgroundColor);
    tft.setTextSize(2);
    tft.setCursor(0, 10);

    for (int i = startIndex; i <= endIndex && i < menuSize; i++) {
      tft.setCursor(0, (i - startIndex) * itemHeight + 10); // Adjust Y position based on visible range
      if (i == selectedIndex) {
        tft.setTextColor(global.accentColor, global.backgroundColor);
        tft.print("> ");
      } else {
        tft.setTextColor(global.foregroundColor, global.backgroundColor);
        tft.print("  ");
      }
      tft.println(padRightWithSpaces(menuItems[i].name, 18));
    }

    // Wait for button event
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
              endIndex = maxVisibleItems - 1;
            } else if (selectedIndex > endIndex) {
              // Shift the visible window down by one
              startIndex++;
              endIndex++;

              // If we somehow go beyond the last menu item, wrap back to top
              if (endIndex >= menuSize) {
                startIndex = 0;
                endIndex = maxVisibleItems - 1;
              }
            }

            // Ensure we always show exactly maxVisibleItems
            if (endIndex - startIndex + 1 > maxVisibleItems) {
              endIndex = startIndex + maxVisibleItems - 1;
            }
          }
        } else if (buttonNumber == "1") { // Select the current option
          if (currentMillis - lastButton1Press > global.debounceDelay) {
            lastButton1Press = currentMillis;
            menuItems[selectedIndex].action(); // Trigger the associated action
            return; // Exit menu after action is performed
          }
        }
      }
    }
  }
}

// Menu helpers
void menuNewKey() {
  executeNewKey("");
}

void menuReturn() {
  return;
}