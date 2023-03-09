CommandResponse executeShowSeed(String positionStr) {
  if (global.authenticated == false) {
    return {"Enter password!", "8 numbers/letters"};
  }

  int position = positionStr.toInt();
  if (!positionStr || position == 0) {
    positionStr = "1";
    position = 1;
  }
  if (position > 24) {
    return {"Show Seed", "Done"};
  }

  String word = getWordAtPosition(global.mnemonic, position - 1);
  serialSendCommand(COMMAND_SEED, positionStr + " " + word);
  printMnemonicWord(positionStr, word);

  EventData event = awaitEvent();
  if (event.type == EVENT_BUTTON_ACTION) {
    event = handleButtonDownEvent(event, position);
  }
  return {"", "", 0, event};
}

EventData handleButtonDownEvent(EventData buttonEvent, int position) {
  String buttonState = getWordAtPosition(buttonEvent.data, 1);
  if (buttonState == "1") {
    // on button up, just show the word again
    return {EVENT_INTERNAL_COMMAND, COMMAND_SEED + " " + String(position)};
  }

  String buttonNumber = getWordAtPosition(buttonEvent.data, 0);
  if (buttonNumber == "1")
    return {EVENT_INTERNAL_COMMAND, COMMAND_SEED + " " + String(position - 1)};
  return {EVENT_INTERNAL_COMMAND, COMMAND_SEED + " " + String(position + 1) };
}
