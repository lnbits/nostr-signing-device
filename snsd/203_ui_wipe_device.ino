void displayWipeDeviceScreen() {
  global.onLogo = false;
  setDisplay(true);

  // Full screen clear
  tft.fillScreen(TFT_BLACK);

  while (true) {
    // Draw the confirmation screen
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 10);
    tft.println("Do you want to wipe this device? All keys will be removed.");

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 90);
    tft.println("Top to confirm");
    tft.println("Bottom to cancel");

    EventData event = awaitEvent();

    if (event.type == EVENT_BUTTON_ACTION) {
      String buttonNumber = getWordAtPosition(event.data, 0);
      String buttonState = getWordAtPosition(event.data, 1);

      if (buttonState == "1") {
        if (buttonNumber == "2") { // Confirm
          // Perform the reset operation
          showMessage("Wiping", "Please wait...");
          SPIFFS.format();

          showMessage("Wipe Complete", "Device has been wiped.");
          ESP.restart();
          return;
        } else if (buttonNumber == "1") { // Cancel
          showMessage("Wipe Canceled", "Returning to menu.");
          return;
        }
      }
    } else if (event.type == EVENT_SCREEN_IDLE) {
      displayLogoScreen();
      return;
    }
  }
}