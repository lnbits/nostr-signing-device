CommandResponse executeHelp(String commandData) {
  help();
  return {"More info at:", "github.com/lnbits/hardware-wallet"};
}

void help()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Commands");
  tft.setCursor(0, 20);
  tft.setTextSize(1);

  tft.println("'/help' show available commands");
  tft.println("'/restore <BIP39 seed words seperated by space>' will restore from seed");
  tft.println("'/wipe' will completely wipe device and creat a new wallet");
  tft.println("'/password' login by providing a password");
  tft.println("'/password-clear' logout");
  tft.println("'/seed' will show the seed on the hww display");
  tft.println("'/psbt' will parse valid psbt and show its outputs and fee");
  tft.println("'/sign' will sign valid psbt");
  tft.println("'/xpub' will return the XPub for a provided derivation path");

  logSerial("Commands");
  logSerial("'/help' show available commands");
  logSerial("'/restore <BIP39 seed words seperated by space>' will restore from seed");
  logSerial("'/wipe' will completely wipe device and creat a new wallet");
  logSerial("'/password' login by providing a password");
  logSerial("'/password-clear' logout");
  logSerial("'/seed' will show the seed on the hww display");
  logSerial("'/psbt' will parse valid psbt and show its outputs and fee");
  logSerial("'/sign' will sign valid psbt");
  logSerial("'/xpub' will return the XPub for a provided derivation path");
  delay(10000);
}