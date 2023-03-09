//========================================================================//
//================================HELPERS=================================//
//========================================================================//

int getMnemonicBytes(String menmonicSentence) {
  int wc = wordCount(menmonicSentence);
  switch (wc)
  {
    case 12:
      return 16;
    case 15:
      return 20;
    case 18:
      return 24;
    case 21:
      return 28;
    case 24:
      return 32;
    default:
      return 0;
  }
}

int wordCount(String s) {
  int count = 1;
  for (int i = 0; i < s.length(); i++)
    if (s[i] == ' ') count++;
  return count;
}


bool isValidPassword(String instr) {
  if (instr.length() < 8) {
    return false;
  }
  return true;
}

String int64ToString(uint64_t value) {
  String s = "";
  while (value != 0) {
    s = String((int)(value % 10)) + s;
    value = (uint64_t) value / 10;
  }
  return s;
}

String getWordAtPosition(String str, int position) {
  String s = str.substring(0);
  int spacePos = 0;
  int i = 0;
  while (spacePos != -1) {
    spacePos = s.indexOf(" ");
    if (i == position) {
      if (spacePos == -1) return s;
      return s.substring(0, spacePos);
    }
    s = s.substring(spacePos + 1);
    i++;
  }

  return "";
}

String generateExtraEtropy() {
  bootloader_random_enable();

  String uBitcoinEntropy = generateMnemonic(24);

  byte espEntropy[32];
  esp_fill_random(espEntropy, 32);
  String espHexEntropy = toHex(espEntropy, 32);

  String clientEntropy = toHex(global.dhe_shared_secret, 32);

  bootloader_random_disable();

  return uBitcoinEntropy + espHexEntropy + clientEntropy + global.passwordHash;
}

String generateStrongerMnemonic(int wordCount) {
  String extraEtropy = generateExtraEtropy();
  return generateMnemonic(wordCount, extraEtropy);
}

Command extractCommand(String s) {
  int spacePos = s.indexOf(" ");
  String command = s.substring(0, spacePos);
  if (spacePos == -1) {
    return {command, ""};
  }
  String commandData = s.substring(spacePos + 1, s.length());
  return {command, commandData};
}
