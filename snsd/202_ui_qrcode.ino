void displayShowNpubQRCodeScreen () {
  global.onLogo = false;
  setDisplay(true);

  if (global.privateKeys.empty()) {
    showMessage("No keys found", "Add a key first");
    return;
  }

  showQRCode(hexToNostr(getPublicKey(global.privateKeys[global.activeKeyIndex]), "npub"), false);

  // Wait for a button press event
  while (true) {
    EventData event = awaitEvent();

    if (event.type == EVENT_BUTTON_ACTION) {
      String buttonState = getWordAtPosition(event.data, 1);

      if (buttonState == "1") {
        return;
      }
    }
  }
}

void displayShowNsecQRCodeScreen () {
  global.onLogo = false;
  setDisplay(true);

  if (global.privateKeys.empty()) {
    showMessage("No keys found", "Add a key first");
    return;
  }

  showQRCode(hexToNostr(global.privateKeys[global.activeKeyIndex], "nsec"), true);

  // Wait for a button press event
  while (true) {
    EventData event = awaitEvent();

    if (event.type == EVENT_BUTTON_ACTION) {
      String buttonState = getWordAtPosition(event.data, 1);

      if (buttonState == "1") {
        return;
      }
    }
  }
}

int qrVersionFromStringLength(int stringLength) {
  if (stringLength <= 17) return 1;
  if (stringLength <= 32) return 2;
  if (stringLength <= 53) return 3;
  if (stringLength <= 134) return 6;
  if (stringLength <= 367) return 11;
  return 28;
}

int squareSizeFromStringLength(int stringLength) {
  if (stringLength <= 53) return 4;
  return 3;
}

void showQRCode(String s, bool isPrivate) {
  int version = qrVersionFromStringLength(s.length());
  int px = squareSizeFromStringLength(s.length());
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];
  qrcode_initText(&qrcode, qrcodeData, version, 0, s.c_str());

  int colour = TFT_BLACK;
  
  if(isPrivate) {
    colour = TFT_RED;
  }

  tft.fillScreen(colour);

  // Calculate the dimensions of the QR code in pixels
  int qrPixelSize = qrcode.size * px;

  // Get the screen dimensions (replace with your screen width and height)
  int screenWidth = tft.width();
  int screenHeight = tft.height();

  // Calculate top-left corner to center the QR code
  int xOffset = (screenWidth - qrPixelSize) / 2;
  int yOffset = (screenHeight - qrPixelSize) / 2;

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      bool full = qrcode_getModule(&qrcode, x, y);

      int color = full ? TFT_WHITE : colour;
      tft.fillRect(xOffset + x * px, yOffset + y * px, px, px, color);
    }
  }
}