/**
   @brief Show info about the existing commands.

   @param commandData: String.  Not used.
   @return CommandResponse
*/
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
  tft.println("'/sign' sign a random message");
  tft.println("'/shared-secret' build a shared secret using an external public key");
  tft.println("'/seed' will show the seed on the device display");

  logInfo("Commands");
  logInfo("'/help' show available commands");
  logInfo("'/sign-message' sign a random message");
  logInfo("'/shared-secret' build a shared secret using an external public key");
  logInfo("'/seed' will show the seed on the device display");

  delay(10000);
}
