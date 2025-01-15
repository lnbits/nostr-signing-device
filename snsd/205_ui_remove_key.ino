void displayRemoveKeyScreen() {
    global.onLogo = false;
    setDisplay(true);

    if (global.privateKeys.empty()) {
        showMessage("No keys found", "Add a key first");
        return;
    }

    int totalKeys = global.privateKeys.size();
    int selectedIndex = global.activeKeyIndex;
    String privateKeyHex = global.privateKeys[selectedIndex];
    String keyPreview = padRightWithSpaces(global.keyNames.count(privateKeyHex) ? global.keyNames[privateKeyHex] : previewString(hexToNostr(getPublicKey(privateKeyHex), "npub")), 20);
    unsigned long lastButton1Press = 0;

    // Full screen clear
    tft.fillScreen(global.backgroundColor);

    while (true) {
        tft.setTextColor(global.foregroundColor, global.backgroundColor);
        tft.setTextSize(2);
        tft.setCursor(0, 10);
        tft.println("Remove Key:");
        tft.println("");
        tft.setTextColor(global.accentColor, global.backgroundColor);
        tft.println(keyPreview);
        tft.println("");

        tft.setTextColor(global.foregroundColor, global.backgroundColor);
        tft.setTextSize(2);
        tft.setCursor(0, 100 * global.scaleFactor);
        tft.println("Top to switch");
        tft.println("Bottom to delete");

        EventData event = awaitEvent();

        if (event.type == EVENT_BUTTON_ACTION) {
            String buttonNumber = getWordAtPosition(event.data, 0);
            String buttonState = getWordAtPosition(event.data, 1);

            if (buttonState == "1") {
                if (buttonNumber == "2") {
                    unsigned long currentMillis = millis();
                    if (currentMillis - lastButton1Press > global.debounceDelay) {
                        lastButton1Press = currentMillis;
                        selectedIndex = (selectedIndex + 1) % totalKeys;
                        privateKeyHex = global.privateKeys[selectedIndex];
                        keyPreview = padRightWithSpaces(global.keyNames.count(privateKeyHex) ? global.keyNames[privateKeyHex] : previewString(hexToNostr(getPublicKey(privateKeyHex), "npub")), 20);
                    }
                } else if (buttonNumber == "1") {
                    CommandResponse response = executeRemoveKey(String(selectedIndex));
                    showMessage("Key Removed", "Total: " + String(global.privateKeys.size()));

                    if (global.privateKeys.empty()) {
                        showMessage("No keys left", "Add a key first");
                        return;
                    }
                    selectedIndex = global.activeKeyIndex;
                    return;
                }
            }
        } else if (event.type == EVENT_SCREEN_IDLE) {
          displayLogoScreen();
          return;
        }
    }
}