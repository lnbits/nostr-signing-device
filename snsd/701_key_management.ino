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

  Serial.print(COMMAND_RESTORE);
  Serial.println(" 0 ");

  showMessage("Key Added", "Total: " + String(global.privateKeys.size()));
  return {"Key Added", "Total: " + String(global.privateKeys.size())};
}

CommandResponse executeAddKey(String privateKey) {
  privateKey.trim();

  privateKey.toLowerCase();
  if(!isValidHexKey(privateKey)) {
    showMessage("Add Key", "Invalid private key hex.");
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

  Serial.print(COMMAND_ADD_KEY);
  Serial.println(" 0 ");

  showMessage("Key Added", "Total: " + String(global.privateKeys.size()));
  return {"Key Added", "Total: " + String(global.privateKeys.size())};
}

CommandResponse executeRemoveKey(String indexStr) {
  int index = indexStr.toInt();
  if (index < 0 || index >= global.privateKeys.size()) {
    return {"Error", "Invalid index"};
  }
  String key = global.privateKeys[index];
  global.privateKeys.erase(global.privateKeys.begin() + index);
  global.keyNames.erase(key); // Remove name if exists
  if (global.activeKeyIndex >= global.privateKeys.size()) {
    global.activeKeyIndex = 0;
  }
  saveKeys();

  Serial.print(COMMAND_REMOVE_KEY);
  Serial.println(" 0 ");

  showMessage("Key Removed", "Total: " + String(global.privateKeys.size()));
  return {"Key Removed", "Total: " + String(global.privateKeys.size())};
}

CommandResponse executeListKeys(String data) {
  for (int i = 0; i < global.privateKeys.size(); i++) {
    String key = global.privateKeys[i];
    String publicKeyHex = getPublicKey(key);
    String name = global.keyNames.count(key) ? global.keyNames[key] : "(Unnamed)";
    sendCommandOutput(COMMAND_LIST_KEYS, String(i) + ": " + publicKeyHex + " - " + name);
  }
  return {"Listed Keys", "Check output"};
}

CommandResponse executeSwitchKey(String indexStr) {
 int index = indexStr.toInt();
  if (index < 0 || index >= global.privateKeys.size()) {
    return {"Error", "Invalid index"};
  }
  global.activeKeyIndex = index;
  saveActiveKeyIndex();

  Serial.print(COMMAND_SWITCH_KEY);
  Serial.println(" 0 ");

  showMessage("Key Selected", "Index: " + String(index));
  return {"Key Selected", "Index: " + String(index)};
}

CommandResponse executeNewKey(String data) {
  char privateKeyHex[65];
  generateNewKeyHex(privateKeyHex);
  global.privateKeys.push_back(privateKeyHex);
  saveKeys();

  Serial.print(COMMAND_NEW_KEY);
  Serial.println(" 0 ");

  showMessage("New Key Added", "Total: " + String(global.privateKeys.size()));
  return {"New Key Added", "Total: " + String(global.privateKeys.size())};
}

CommandResponse executeNameKey(String data) {
  if (data == "") return {"Error", "No input provided"};

  // Get the key index and name
  String indexStr = getTokenAtPosition(data, " ", 0);

  int firstSpaceIndex = data.indexOf(" ");
  String name = (firstSpaceIndex != -1) ? data.substring(firstSpaceIndex + 1) : "";
  name.trim();

  // Parse the index
  int index = indexStr.toInt();
  if (index < 0 || index >= global.privateKeys.size()) {
    return {"Error", "Invalid index"};
  }

  String key = global.privateKeys[index];

  if (name.isEmpty()) {
    // Remove the name if the input name is blank
    if (global.keyNames.count(key)) {
      global.keyNames.erase(key); // Remove the name from the map
      saveKeys();
      showMessage("Key Name Removed", "Key at index " + indexStr + " had its name removed");
      return {"Key Name Removed", "Key at index " + indexStr + " had its name removed"};
    } else {
      return {"Error", "No name to remove for this key"};
    }
  } else {
    // Set or update the name
    global.keyNames[key] = name;
    saveKeys();
    showMessage("Key Name Set", "Key at index " + indexStr + " named '" + name + "'");
    return {"Key Name Set", "Key at index " + indexStr + " named '" + name + "'"};
  }
}

void saveActiveKeyIndex() {
  writeFile(SPIFFS, FILE_ACTIVE_KEY_INDEX, String(global.activeKeyIndex));
}

void loadActiveKeyIndex() {
  FileData indexFile = readFile(SPIFFS, FILE_ACTIVE_KEY_INDEX);
  if (indexFile.success) {
    global.activeKeyIndex = indexFile.data.toInt();
    if (global.activeKeyIndex < 0 || global.activeKeyIndex >= global.privateKeys.size()) {
      global.activeKeyIndex = 0;
    }
  } else {
    global.activeKeyIndex = 0;
  }
}
