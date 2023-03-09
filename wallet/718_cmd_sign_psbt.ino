CommandResponse executeSignPsbt(String commandData) {
  if (global.authenticated == false) {
    return { "Enter password!", "8 numbers/letters"};
  }

  showMessage("Please wait", "Parsing PSBT...");
  int spacePos = commandData.indexOf(" ");
  String networkName = commandData.substring(0, spacePos);
  String psbtBase64 = commandData.substring(spacePos + 1, commandData.length() );

  const Network * network;
  if (networkName == "Mainnet") {
    network = &Mainnet;
  } else if (networkName == "Testnet") {
    network = &Testnet;
  } else {
    return { "Unknown Network", "Must be Mainent or Testnet"};
  }

  PSBT psbt = parseBase64Psbt(psbtBase64);
  if (!psbt) {
    logSerial("Failed psbt: " + psbtBase64);
    serialSendCommand(COMMAND_SEND_PSBT, "psbt_parse_failed");
    return {"Failed parsing",  "Send PSBT again"};
  }

  HDPrivateKey hd(global.mnemonic, global.passphrase, network);
  // check if it is valid
  if (!hd) {
    serialSendCommand(COMMAND_SEND_PSBT, "invalid_mnemonic'");
    return {"Invalid Mnemonic", ""};
  }

  serialSendCommand(COMMAND_SEND_PSBT, "1");

  for (int i = 0; i < psbt.tx.outputsNumber; i++) {
    printOutputDetails(psbt, hd, i, network);

    EventData event = awaitEvent();
    if (event.type != EVENT_SERIAL_DATA) {
      return {"Operation Canceled", "button pressed" };
    };
    String data = event.data;

    Command c = decryptAndExtractCommand(data);
    if (c.cmd == COMMAND_CANCEL) {
      return {"Operation Canceled", "`/help` for details" };
    }
    if (c.cmd != COMMAND_CONFIRM_NEXT) {
      return executeUnknown("Expected: " + COMMAND_CONFIRM_NEXT);
    }
  }

  printFeeDetails(psbt.fee());

  EventData event = awaitEvent();
  if (event.type != EVENT_SERIAL_DATA) {
    return {"Operation Canceled", "`/help` for details" };
  }
  String data = event.data;

  Command c = decryptAndExtractCommand(data);
  if (c.cmd == COMMAND_SIGN_PSBT) {
    showMessage("Confirm", "Press button to confirm");
    EventData event = awaitEvent();
    if (event.type != EVENT_BUTTON_ACTION) {
      return {"Operation Canceled", "" };
    };

    showMessage("Please wait", "Signing PSBT...");

    uint8_t signedInputCount = psbt.sign(hd);

    serialSendCommand(COMMAND_SIGN_PSBT,  String(signedInputCount) + " " + psbt.toBase64());
    return { "Signed inputs:", String(signedInputCount) };
  }
  if (c.cmd = COMMAND_CANCEL) {
    return { "Operation Canceled",  "`/help` for details" };
  }
  return  executeUnknown("Expected: " + COMMAND_SIGN_PSBT);
}
