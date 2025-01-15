CommandResponse executeReboot(String data) {
  ESP.restart();
  showMessage("Reset", "Device is rebooting.");
  return {"Reset", "Device is rebooting."};
}

CommandResponse executeWipe(String data) {
  SPIFFS.format();
  ESP.restart();
  showMessage("Wipe", "Wiping device of all data.");
  return {"Wipe", "Wiping device of all data."};
}