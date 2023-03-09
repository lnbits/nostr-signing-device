//========================================================================//
//================================COMMANDS================================//
//========================================================================//

CommandResponse cmdRes = {"Welcome", "Row, row, row your boat"};


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
  if (isEncryptedCommand(c.cmd) && isNotInternalCommand(event.type)) {
    c = decryptAndExtractCommand(data);
  }
  // Do not remove this log line. Flushes stale data from buffer.
  logSerial("received command: " + c.cmd);
  cmdRes = executeCommand(c);

  delay(DELAY_MS);
}

bool isEncryptedCommand(String cmd) {
  return cmd != COMMAND_PAIR &&
         cmd != COMMAND_CHECK_PAIRING &&
         cmd != COMMAND_PING;
}

bool isNotInternalCommand(String type) {
  return type != EVENT_INTERNAL_COMMAND;
}

bool isNotCommandEvent(String type) {
  return type != EVENT_SERIAL_DATA && type != EVENT_INTERNAL_COMMAND;
}


CommandResponse executeCommand(Command c) {
  if (c.cmd == COMMAND_PING)
    return executePing(c.data);

  if (c.cmd == COMMAND_CHECK_PAIRING)
    return executeCheckPairing(c.data);

  if (c.cmd == COMMAND_PAIR)
    return executePair(c.data);

  if (c.cmd == COMMAND_HELP)
    return executeHelp(c.data);

  if (c.cmd == COMMAND_WIPE)
    return executeWhipeHww(c.data);

  if (c.cmd == COMMAND_PASSWORD)
    return executePasswordCheck(c.data);

  if (c.cmd == COMMAND_PASSWORD_CLEAR)
    return executePasswordClear(c.data);

  if (c.cmd == COMMAND_ADDRESS)
    return executeShowAddress(c.data);

  if (c.cmd == COMMAND_SEED)
    return executeShowSeed(c.data);

  if (c.cmd == COMMAND_SEND_PSBT)
    return executeSignPsbt(c.data);

  if (c.cmd == COMMAND_RESTORE)
    return executeRestore(c.data);

  if (c.cmd == COMMAND_XPUB)
    return executeXpub(c.data);

  return executeUnknown(c.cmd + ": " + c.data);

}

unsigned long lastTickTime = 0;
int counter = 10;

int button1State = HIGH;
int button2State = HIGH;


EventData awaitEvent() {
  unsigned long  waitTime = millis();
  bool idle = true;
  while (Serial.available() == 0) {
    // check if ok for pairing or if idle
    if (idle == true) {
      if  ((millis() - waitTime) > 60 * 1000) {
        idle = false;
        logo(0);
      } else if  (counter > 0 && ((millis() - lastTickTime) > 1000)) {
          counter--;
          lastTickTime = millis();
          logo(counter);
        } else if (counter == 0) {
          logo(counter);
          counter--;
        }
    }

    EventData buttonEvent = checkButtonsState();
    if (buttonEvent.type == EVENT_BUTTON_ACTION) return buttonEvent;

  }
  counter = -1;
  String data = Serial.readStringUntil('\n');
  return { EVENT_SERIAL_DATA, data };
}

EventData checkButtonsState() {
  int button1NewState = digitalRead(global.button1Pin);
  if (button1NewState != button1State) {
    logSerial("button 1: " + String(button1NewState));
    button1State = button1NewState;
    return { EVENT_BUTTON_ACTION, "1 " + String(button1NewState)};
  }

  int button2NewState = digitalRead(global.button2Pin);
  if (button2NewState != button2State) {
    logSerial("button 2: " + String(button2NewState));
    button2State = button2NewState;
    return { EVENT_BUTTON_ACTION, "2 " + String(button2NewState)};
  }
  return {"", ""};
}

HwwInitData initHww(String password, String mnemonic, String passphrase) {
  if (isValidPassword(password) == false)
    return {"", "", false};

  deleteFile(SPIFFS, global.mnemonicFileName.c_str());
  deleteFile(SPIFFS, global.passwordFileName.c_str());
  if (mnemonic == "") {
    mnemonic = generateStrongerMnemonic(24); // todo: allow 12 also
  }

  String passwordHash  = hashStringData(password);
  writeFile(SPIFFS, global.passwordFileName.c_str(), passwordHash);

  int byteSize =  passwordHash.length() / 2;
  byte encryptionKey[byteSize];
  fromHex(passwordHash, encryptionKey, byteSize);


  writeFile(SPIFFS, global.mnemonicFileName.c_str(), encryptDataWithIv(encryptionKey, mnemonic));

  return {passwordHash, mnemonic, true};
}

void serialSendCommand(String command, String commandData) {
  serialPrintlnSecure(command + " " + commandData);
}

void serialPrintlnSecure(String msg) {
  String encryptedHex = encryptDataWithIv(global.dhe_shared_secret, msg);
  Serial.println(encryptedHex);
}

Command decryptAndExtractCommand(String ecryptedData) {
  String data = decryptDataWithIv(global.dhe_shared_secret, ecryptedData);
  return extractCommand(data);
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
