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
    {"Set/Change PIN", displaySetPinScreen},
    {"Wipe Device", displayWipeDeviceScreen},
    {"Show Nsec QR Code", displayShowNsecQRCodeScreen},
    {"Back", menuReturn}
  };

  int menuSize = sizeof(menuItems) / sizeof(MenuItem);
  int selectedIndex = 0;

  // Show exactly 6 items at a time (indices 0 through 5)
  int startIndex = 0;
  int endIndex = 5;

  unsigned long lastButton1Press = 0;
  unsigned long lastButton2Press = 0;

  // Full screen clear
  tft.fillScreen(TFT_BLACK);

  while (true) {
    // Draw the menu
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 10);

    for (int i = startIndex; i <= endIndex && i < menuSize; i++) {
      tft.setCursor(0, (i - startIndex) * 20 + 10); // Adjust Y position based on visible range
      if (i == selectedIndex) {
        tft.setTextColor(TFT_LNBITS_PURPLE, TFT_BLACK);
        tft.print("> ");
      } else {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
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
              endIndex = 5; 
            } else if (selectedIndex > endIndex) {
              // Shift the visible window down by one
              startIndex++;
              endIndex++;

              // If we somehow go beyond the last menu item, wrap back to top
              if (endIndex >= menuSize) {
                startIndex = 0;
                endIndex = 5;
              }
            }

            // Ensure we always show exactly 6 items
            if (endIndex - startIndex + 1 > 6) {
              endIndex = startIndex + 5;
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