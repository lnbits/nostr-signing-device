CommandResponse executeWhipeHww(String password) {
  if (password == "") {
    return { "Enter new password", "8 numbers/letters"};
  }

  showMessage("Resetting...", "");
  delay(2000);

  HwwInitData data = initHww(password, "", "");
  delay(DELAY_MS);
  global.authenticated = data.success;

  serialSendCommand(COMMAND_WIPE,  String(global.authenticated));

  if (global.authenticated == true) {
    global.passwordHash = data.passwordHash;
    global.mnemonic = data.mnemonic;
    global.passphrase = "";
    return { "Successfully wiped!",  "Every new beginning comes from some other beginning's end."};
  }
  return {"Error, try again"  "8 numbers/letters"};
}
