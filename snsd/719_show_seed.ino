/**
   @brief Show the seed.
   While in the "interactive" mode (communicating over wire with a client) only one word can be requested at a time.
   In the "command file" mode all the seed words will be returned.
   @param positionStr: String (optional). Position of the word to be displayed.
   @return CommandResponse
    - the seed word at the specified positon to the UI
    - `/seed {position} {word}` to the client. Position is the postion of the word in the list.
*/
CommandResponse executeShowSeed(String positionStr) {
  if (global.authenticated == false) {
    return {"Enter password!", "8 numbers/letters"};
  }

  return showSeed(positionStr);
}

CommandResponse showSeed(String positionStr) {
  if (global.hasCommandsFile == true) {
    return sendSeedToFile();
  }
  return showSeedWordAtPosition(positionStr);
}

CommandResponse sendSeedToFile() {
  commandOutToFile("Seed: " + global.mnemonic);
  return {"Show Seed", "Done"};
}

CommandResponse showSeedWordAtPosition(String positionStr) {
  int position = positionStr.toInt();
  if (!positionStr || position == 0) {
    positionStr = "1";
    position = 1;
  }
  if (position > 24) {
    return {"Show Seed", "Done"};
  }

  String word = getWordAtPosition(global.mnemonic, position - 1);
  sendCommandOutput(COMMAND_SEED, positionStr + " " + word);
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
