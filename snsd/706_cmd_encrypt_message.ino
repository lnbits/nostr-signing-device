CommandResponse executeEncryptMessage(String data) {
    if (data == "") return {"", ""};
    
    // Get the shared secret as the 3rd party public key
    String thirdPartyPublicKey = getTokenAtPosition(data, " ", 0);
    
    thirdPartyPublicKey.trim();

    thirdPartyPublicKey.toLowerCase();
    if(!isValidHexKey(thirdPartyPublicKey)) {
      showMessage("Encrypt Message", "Invalid 3rd party public key.");
      return {"", ""};
    }

    String sharedSecretHex = generateSharedSecret(thirdPartyPublicKey);
    
    // Get the content as everything after the first space
    int firstSpaceIndex = data.indexOf(" ");
    String content = (firstSpaceIndex != -1) ? data.substring(firstSpaceIndex + 1) : "";
    content.trim();
    
    String encryptedMessage = encryptDirectMessageContent(sharedSecretHex, content);
    sendCommandOutput(COMMAND_ENCRYPT_MESSAGE, encryptedMessage);

    showMessage("Encrypt Message", content.substring(0, 16) + "...");
    
    return {"Encrypt Message", data};
}