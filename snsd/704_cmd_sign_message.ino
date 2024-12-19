/**
   @brief Sign a random message.

   @param messageHex: String. The message to be signed in hex format.
   @return CommandResponse
*/
CommandResponse executeSignMessage(String messageHex) {
  global.onLogo = false;
  setDisplay(true);
  
  if (global.privateKeys.empty()) {
    showMessage("No keys available", "Add a key first");
    return {"Error", "No keys available"};
  }

  int selectedIndex = global.activeKeyIndex;
  String privateKeyHex = global.privateKeys[selectedIndex];
  String keyPreview = padRightWithSpaces(global.keyNames.count(privateKeyHex) ? global.keyNames[privateKeyHex] : previewString(hexToNostr(getPublicKey(privateKeyHex), "npub")), 20).substring(0,20);

  unsigned long lastButton2Press = 0;

  while (true) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 10);
    tft.println("Sign Request:");
    tft.println("");
    tft.setTextColor(TFT_LNBITS_PURPLE, TFT_BLACK);
    tft.println("Key: " + keyPreview);
    tft.println("");

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, 90);
    tft.println("Top to accept");
    tft.println("Bottom to reject");

    EventData event = awaitEvent();

    if (event.type == EVENT_BUTTON_ACTION) {
      String buttonNumber = getWordAtPosition(event.data, 0);
      String buttonState = getWordAtPosition(event.data, 1);

      if (buttonState == "1") {
        if (buttonNumber == "2") {
          unsigned long currentMillis = millis();
          if (currentMillis - lastButton2Press > global.debounceDelay) {
            lastButton2Press = currentMillis;

            // Accept
            showMessage("Please wait", "Signing message...");

            String privateKeyHex = global.privateKeys[selectedIndex];
            int byteSize = 32;
            byte privateKeyBytes[byteSize];
            fromHex(privateKeyHex, privateKeyBytes, byteSize);
            PrivateKey privateKey(privateKeyBytes);

            byte messageBytes[byteSize];
            fromHex(messageHex, messageBytes, byteSize);
            SchnorrSignature signature = privateKey.schnorr_sign(messageBytes);

            String signatureHex = String(signature);
            sendCommandOutput(COMMAND_SIGN_MESSAGE, signatureHex);
            showMessage("Signed Message", signatureHex.substring(0, 16) + "...");

            return {"Signed Message", signatureHex.substring(0, 16) + "..."};
          }
        } else if (buttonNumber == "1") {
          // Reject
          sendCommandOutput(COMMAND_SIGN_MESSAGE, "Rejected"); // Notify rejection
          showMessage("Request Rejected", "Message signing aborted.");

          return {"Rejected", "Message signing aborted"};
        }
      }
    }
  }
}