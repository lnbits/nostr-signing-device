//========================================================================//
//================================COMMANDS================================//
//========================================================================//

CommandResponse cmdRes = {"Nostr", "Signing Device"};

void listenForCommands() {
  if (cmdRes.message != "" || cmdRes.subMessage != "")
    displayLogoScreen();

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

  if (c.cmd == COMMAND_PING)
    return executePing(c.data);

  if (c.cmd == COMMAND_HELP)
    return executeHelp(c.data);

  if (c.cmd == COMMAND_RESTORE)
    return executeRestore(c.data);

  if (c.cmd == COMMAND_ADD_KEY)
    return executeAddKey(c.data);

  if (c.cmd == COMMAND_REMOVE_KEY)
    return executeRemoveKey(c.data);

  if (c.cmd == COMMAND_LIST_KEYS)
    return executeListKeys(c.data);

  if (c.cmd == COMMAND_SWITCH_KEY)
    return executeSwitchKey(c.data);

  if (c.cmd == COMMAND_NEW_KEY)
    return executeNewKey(c.data);

  if (c.cmd == COMMAND_NAME_KEY)
    return executeNameKey(c.data);

  if (c.cmd == COMMAND_ENCRYPT_MESSAGE)
    return executeEncryptMessage(c.data);

  if (c.cmd == COMMAND_DECRYPT_MESSAGE)
    return executeDecryptMessage(c.data);

  if (c.cmd == COMMAND_REBOOT)
    return executeReboot(c.data);

  if (c.cmd == COMMAND_WIPE)
    return executeWipe(c.data);

  return executeUnknown(c.cmd + ": " + c.data);
}

void sendCommandOutput(String command, String commandData) {
  Serial.println(command + " " + commandData);
}

void savePIN(String pinCode) {
  writeFile(SPIFFS, FILE_PIN, pinCode);
}

void loadPIN() {
  // Load the expected PIN number
  FileData pinFile = readFile(SPIFFS, FILE_PIN);
  if (pinFile.success) {
    global.pinCode = pinFile.data;
  } else {
    // Default to not set
    global.pinCode = "00000000";
  }

  // Load the PIN attempts
  FileData attemptsFile = readFile(SPIFFS, FILE_PIN);
  if (attemptsFile.success) {
    global.pinAttempts = attemptsFile.data.toInt();
  } else {
    // Default to zero
    global.pinAttempts = 0;
  }
}
