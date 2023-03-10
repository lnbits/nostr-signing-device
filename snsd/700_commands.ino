//========================================================================//
//================================COMMANDS================================//
//========================================================================//

CommandResponse cmdRes = {"Welcome", ""};


void listenForCommands() {
  if (cmdRes.message != "" || cmdRes.subMessage != "")
    showMessage(cmdRes.message, cmdRes.subMessage);


  // if the command does not handle an event then it bubbles it up
  EventData event = cmdRes.event;
  if (isNotCommandEvent(event.type)) {
    event = awaitEvent();
  }

  if (isNotCommandEvent(event.type)) return;

  String data = event.data;

  Command c = extractCommand(data);

  // Do not remove this log line. Flushes stale data from buffer.
  logInfo("received command: " + c.cmd);
  cmdRes = executeCommand(c);

  delay(DELAY_MS);
}

bool isNotInternalCommand(String type) {
  return type != EVENT_INTERNAL_COMMAND;
}

bool isNotCommandEvent(String type) {
  return type != EVENT_SERIAL_DATA && type != EVENT_INTERNAL_COMMAND;
}


CommandResponse executeCommand(Command c) {
  if (c.cmd == COMMAND_SIGN_MESSAGE)
    return executeSignMessage(c.data);

  if (c.cmd == COMMAND_PUBLIC_KEY)
    return executePublicKey(c.data);

  if (c.cmd == COMMAND_SHARED_SECRET)
    return executeSharedSecret(c.data);

  if (c.cmd == COMMAND_RESTORE)
    return executeResore(c.data);


  if (c.cmd == COMMAND_SEED)
    return executeShowSeed(c.data);

  if (c.cmd == COMMAND_PING)
    return executePing(c.data);

  if (c.cmd == COMMAND_HELP)
    return executeHelp(c.data);


  return executeUnknown(c.cmd + ": " + c.data);

}


HwwInitData initHww(String password, String mnemonic, String passphrase, bool persistSecrets) {
  if (isValidPassword(password) == false)
    return {"", "", false};

  if (mnemonic == "") {
    mnemonic = generateStrongerMnemonic(24); // todo: allow 12 also
  }

  String passwordHash  = hashStringData(password);

  if (persistSecrets == true) {
    deleteFile(SPIFFS, global.mnemonicFileName.c_str());
    deleteFile(SPIFFS, global.passwordFileName.c_str());
    writeFile(SPIFFS, global.passwordFileName.c_str(), passwordHash);

    int byteSize =  passwordHash.length() / 2;
    byte encryptionKey[byteSize];
    fromHex(passwordHash, encryptionKey, byteSize);

    writeFile(SPIFFS, global.mnemonicFileName.c_str(), encryptDataWithIv(encryptionKey, mnemonic));
  }

  return {passwordHash, mnemonic, true};
}

void sendCommandOutput(String command, String commandData) {
  Serial.println(command + " " + commandData);
}


void commandOutToFile(const String msg) {
  if (global.hasCommandsFile == true) {
    appendFile(SD, global.commandsOutFileName.c_str(), msg + "\n");
  }
}


EventData toggleDatanAndQR(String data, bool showQR) {
  String dataUpper = data + "";
  dataUpper.toUpperCase();

  EventData event = {EVENT_BUTTON_ACTION, "0 1"};
  while (event.type == EVENT_BUTTON_ACTION) {
    String buttonState = getWordAtPosition(event.data, 1);

    if (buttonState == "1") {
      if (showQR == true) {
        showQRCode(dataUpper);
      } else {
        showMessage(data, "");
      }
    } else {
      showQR = !showQR;
    }
    event = awaitEvent();
  }
  return event;
}
