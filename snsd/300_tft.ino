#ifdef TDISPLAY
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
void setupTFT() {
  tft.init();
  Serial.println("TFT: " + String(TFT_WIDTH) + "x" + String(TFT_HEIGHT));
  Serial.println("TFT pin MISO: " + String(TFT_MISO));
  Serial.println("TFT pin CS: " + String(TFT_CS));
  Serial.println("TFT pin MOSI: " + String(TFT_MOSI));
  Serial.println("TFT pin SCLK: " + String(TFT_SCLK));
  Serial.println("TFT pin DC: " + String(TFT_DC));
  Serial.println("TFT pin RST: " + String(TFT_RST));
  Serial.println("TFT pin BL: " + String(TFT_BL));
  tft.setRotation(1);
  tft.invertDisplay(true);
}
void printTFT(String message, int x, int y) {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(x, y);
  tft.println(message);
}
void clearTFT() {
  tft.fillScreen(TFT_BLACK);
}
#else
void printTFT(String message, int x, int y) {}
void clearTFT() {}
#endif
