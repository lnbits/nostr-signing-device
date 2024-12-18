//========================================================================//
//=============================KEY MANAGEMENT=============================//
//========================================================================//
CommandResponse executeRestore(String privateKey) {
  privateKey.trim();

  privateKey.toLowerCase();
  if(!isValidHexKey(privateKey)) {
    showMessage("Restore Key", "Invalid private key hex.");
    return {"", ""};
  }
  
  // Check if the key already exists
  for (const String& existingKey : global.privateKeys) {
    if (existingKey == privateKey) {
      showMessage("Key Already Exists", "The private key is already in the list.");
      return {"", ""};
    }
  }

  // Add the key if it doesn't already exist
  global.privateKeys.push_back(privateKey);
  saveKeys();

  Serial.println(COMMAND_RESTORE + " 0 ");

  showMessage("Key Added", "Total: " + String(global.privateKeys.size()));
  return {"Key Added", "Total: " + String(global.privateKeys.size())};
}

CommandResponse executeAddKey(String privateKey) {
  // Check key is valid format and convert if needed
  privateKey.trim();

  if (isValidBech32Key(privateKey, true)) {
    privateKey = nostrToHex(privateKey);
  } else {
    privateKey.toLowerCase();
    if (!isValidHexKey(privateKey)) {
      showMessage("Add key", "Invalid private key.");
      return {"", ""};
    }
  }

  // Check if the key already exists
  for (const String& existingKey : global.privateKeys) {
    if (existingKey == privateKey) {
      showMessage("Key Already Exists", "The private key is already in the list.");
      return {"", ""};
    }
  }

  // Add the key if it doesn't already exist
  global.privateKeys.push_back(privateKey);
  saveKeys();

  Serial.println(COMMAND_ADD_KEY + " 0 ");

  showMessage("Key Added", "Total: " + String(global.privateKeys.size()));
  return {"Key Added", "Total: " + String(global.privateKeys.size())};
}

CommandResponse executeRemoveKey(String indexStr) {
  int index = indexStr.toInt();
  if (index < 0 || index >= global.privateKeys.size()) {
    return {"Error", "Invalid index"};
  }
  global.privateKeys.erase(global.privateKeys.begin() + index);
  if (global.activeKeyIndex >= global.privateKeys.size()) {
    global.activeKeyIndex = 0;
  }
  saveKeys();

  Serial.println(COMMAND_REMOVE_KEY + " 0 ");

  showMessage("Key Removed", "Total: " + String(global.privateKeys.size()));
  return {"Key Removed", "Total: " + String(global.privateKeys.size())};
}

CommandResponse executeListKeys(String data) {
  for (int i = 0; i < global.privateKeys.size(); i++) {
    String preview = hexToNostr(getPublicKey(global.privateKeys[i]), "npub");
    sendCommandOutput(COMMAND_LIST_KEYS, String(i) + ": " + preview);
  }
  return {"Listed Keys", "Check output"};
}

CommandResponse executeNewKey(String data) {
  char privateKeyHex[65];
  generateNewKeyHex(privateKeyHex);
  global.privateKeys.push_back(privateKeyHex);
  saveKeys();

  Serial.println(COMMAND_NEW_KEY + " 0 ");

  showMessage("New Key Added", "Total: " + String(global.privateKeys.size()));
  return {"New Key Added", "Total: " + String(global.privateKeys.size())};
}

void saveActiveKeyIndex() {
  writeFile(SPIFFS, global.activeKeyIndexFileName.c_str(), String(global.activeKeyIndex));
}

void loadActiveKeyIndex() {
  FileData indexFile = readFile(SPIFFS, global.activeKeyIndexFileName.c_str());
  if (indexFile.success) {
    global.activeKeyIndex = indexFile.data.toInt();
    if (global.activeKeyIndex < 0 || global.activeKeyIndex >= global.privateKeys.size()) {
      global.activeKeyIndex = 0;
    }
  } else {
    global.activeKeyIndex = 0;
  }
}