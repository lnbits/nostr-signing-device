//========================================================================//
//===============================UI STUFF=================================//
//========================================================================//

void logo() {
  String title = "NSD";
  String subTitle = "Nostr Signing Device";

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(4);
  tft.setCursor(0, 20);
  tft.print(title);
  tft.setTextSize(2);
  tft.setCursor(0, 70);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print(subTitle);
  tft.setTextSize(1);
  tft.setCursor(0, 150);
  tft.print("version: " + env.version);
}

void showMessage(String message, String additional)
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 30);
  tft.println(message);
  tft.setCursor(0, 80);
  tft.setTextSize(2);
  tft.println(additional);
  logInfo(message);
  logInfo(additional);
}



void printMnemonicWord(String position, String word) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 10);
  tft.println("Word " + position + ":");
  tft.println("");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(3);
  tft.println(word);
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

void showQRCode(String s) {
  int version = qrVersionFromStringLength(s.length());
  int px = squareSizeFromStringLength(s.length());
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];
  qrcode_initText(&qrcode, qrcodeData, version, 0, s.c_str());

  tft.fillScreen(TFT_BLACK);

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      bool full = qrcode_getModule(&qrcode, x, y);

      int color = full ? TFT_WHITE : TFT_BLACK;
      tft.fillRect((x + 2) * px, (y + 1) * px, px, px, color);
    }
  }
}
