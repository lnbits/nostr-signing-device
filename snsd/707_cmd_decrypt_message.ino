CommandResponse executeDecryptMessageNip04(String data) {
  if (data == "") return {"", ""};
  
  // Get the shared secret as the 3rd party public key
  String thirdPartyPublicKey = getTokenAtPosition(data, " ", 0);

  thirdPartyPublicKey.trim();

  thirdPartyPublicKey.toLowerCase();
  if(!isValidHexKey(thirdPartyPublicKey)) {
    showMessage("Decrypt Message", "Invalid 3rd party public key.");
    return {"", ""};
  }

  String sharedSecretHex = generateSharedSecret(thirdPartyPublicKey);
  
  // Get the content as everything after the first space
  String encryptedContent = getTokenAtPosition(data, " ", 1);
  encryptedContent.trim();

  String decryptedMessage = decryptDirectMessageContentNip04(sharedSecretHex, encryptedContent);
  sendCommandOutput(COMMAND_DECRYPT_MESSAGE_NIP04, decryptedMessage);

  showMessage("Decrypt Message", decryptedMessage.substring(0, 16) + "...");
  
  return {"Decrypt Message", data};
}

CommandResponse executeDecryptMessageNip44(String data) {
    logInfo("Full command data length: " + String(data.length()));
    
    if (data == "") return {"", ""};
    
    // Get the shared secret as the 3rd party public key
    String thirdPartyPublicKey = getTokenAtPosition(data, " ", 0);
    logInfo("Public key length: " + String(thirdPartyPublicKey.length()));
    
    thirdPartyPublicKey.trim();
    thirdPartyPublicKey.toLowerCase();
    
    if(!isValidHexKey(thirdPartyPublicKey)) {
      showMessage("Decrypt Message", "Invalid 3rd party public key.");
      return {"", ""};
    }

    String sharedSecretHex = generateSharedSecret(thirdPartyPublicKey);
    
    // Get the encrypted content
    String encryptedContent = getTokenAtPosition(data, " ", 1);
    encryptedContent.trim();
    
    logInfo("Encrypted content: " + encryptedContent);
    logInfo("Encrypted content length: " + String(encryptedContent.length()));

    String decryptedMessage = decryptMessageNip44(encryptedContent, sharedSecretHex);
    sendCommandOutput(COMMAND_DECRYPT_MESSAGE_NIP44, decryptedMessage);

    showMessage("Decrypt NIP-44", decryptedMessage.substring(0, 16) + "...");
    
    return {"Decrypt NIP-44", data};
}