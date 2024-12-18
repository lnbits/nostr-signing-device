//========================================================================//
//================================HELPERS=================================//
//========================================================================//

String getWordAtPosition(String str, int position) {
  return getTokenAtPosition(str, " ", position);
}

String getLineAtPosition(String str, int position) {
  return getTokenAtPosition(str, "\n", position);
}

String getTokenAtPosition(String str, String separator, int position) {
  String s = str.substring(0);
  int separatorPos = 0;
  int i = 0;
  while (separatorPos != -1) {
    separatorPos = s.indexOf(separator);
    if (i == position) {
      if (separatorPos == -1) return s;
      return s.substring(0, separatorPos);
    }
    s = s.substring(separatorPos + 1);
    i++;
  }

  return "";
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

void saveKeys() {
  String keysData = "";
  for (String key : global.privateKeys) {
    keysData += key + "\n"; // Each key on a new line
  }
  writeFile(SPIFFS, global.privateKeysFileName.c_str(), keysData);
}

int countLines(String data, char delimiter = '\n') {
  int count = 0;
  for (int i = 0; i < data.length(); i++) {
    if (data[i] == delimiter) {
      count++;
    }
  }
  // Add 1 to include the last line if it doesn't end with a delimiter
  return data.endsWith(String(delimiter)) ? count : count + 1;
}

void loadKeys() {
  FileData keysFile = readFile(SPIFFS, global.privateKeysFileName.c_str());
  if (keysFile.success) {
    global.privateKeys.clear(); // Ensure no duplicate keys
    String keysData = keysFile.data;
    int lineCount = countLines(keysData, '\n'); // Count the number of lines
    for (int i = 0; i < lineCount; i++) {
      String key = getLineAtPosition(keysData, i);
      if (!key.isEmpty()) {
        global.privateKeys.push_back(key);
      }
    }
    logInfo("Keys loaded: " + String(global.privateKeys.size()));
  } else {
    logInfo("No keys file found, starting fresh");
  }
}

String getPublicKey(String privateKeyHex) {
  int byteSize =  32;
  byte privateKeyBytes[byteSize];
  fromHex(privateKeyHex, privateKeyBytes, byteSize);
  PrivateKey privateKey(privateKeyBytes);
  PublicKey publicKey = privateKey.publicKey();

  return toHex(publicKey.point, byteSize);
}

void generateNewKeyHex(char *buffer) {
  for (size_t i = 0; i < 32; i++) {
    uint8_t randomByte = esp_random() & 0xFF;
    sprintf(&buffer[i * 2], "%02x", randomByte);
  }
  buffer[64] = '\0';
}

void generateRandomIV(uint8_t *iv, int length) {
    for (int i = 0; i < length; i++) {
        iv[i] = random(0, 256);
    }
}

String previewString(const String &input) {
    if (input.length() <= 19) {
        return input;
    }
    String firstPart = input.substring(0, 8);
    String lastPart = input.substring(input.length() - 8);
    return firstPart + "..." + lastPart;
}

// Used to allow us to do partial screen refreshes to avoid flickering
String padRightWithSpaces(String input, int length) {
    while (input.length() < length) {
        input += ' '; // Append space
    }
    return input;
}

// Function to check if a string is 64 characters long and contains only lowercase hex characters
bool isValidHexKey(const String& input) {
    String trimmed = input;
    trimmed.trim();

    // Check if the length is 64
    if (trimmed.length() != 64) {
        return false;
    }

    // Check if all characters are valid lowercase hex
    for (size_t i = 0; i < trimmed.length(); i++) {
        char c = trimmed.charAt(i);
        if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f')) {
            return false;
        }
    }

    return true;
}

// Function to check if a string is any length and contains only lowercase hex characters
bool isValidHex(const String& input) {
    String trimmed = input;
    trimmed.trim();

    // Check if all characters are valid lowercase hex
    for (size_t i = 0; i < trimmed.length(); i++) {
        char c = trimmed.charAt(i);
        if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f')) {
            return false;
        }
    }

    return true;
}

// Function to check if a string is valid bech32 with specific prefix
bool isValidBech32Key(const String& input, bool isPrivate) {
    const String prefix = isPrivate ? "nsec1" : "npub1";
    const String bech32Charset = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

    String trimmed = input;
    trimmed.trim();

    // Check if the string starts with the required prefix
    if (!trimmed.startsWith(prefix)) {
        return false;
    }

    // Verify the rest of the string after the prefix
    for (size_t i = prefix.length(); i < trimmed.length(); i++) {
        char c = trimmed.charAt(i);
        if (bech32Charset.indexOf(c) == -1) {
            return false;
        }
    }

    return true;
}

void migrateKey() {
  logInfo("Migrating private key..");
  FileData nostrFile = readFile(SPIFFS, global.legacyNostrSecretFileName.c_str());
  writeFile(SPIFFS, global.privateKeysFileName.c_str(), nostrFile.data + "\n");
  deleteFile(SPIFFS, global.legacyNostrSecretFileName.c_str());
  logInfo("Private key migrated.");
  ESP.restart();
}