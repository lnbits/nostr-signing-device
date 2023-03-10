/**
   @brief Sign a random message.

   @param messageHex: String. The message to be signed in hex format.
   @return CommandResponse
*/
CommandResponse executeSignMessage(String messageHex) {
  showMessage("Please wait", "Signing message...");

  logInfo("Preparing to sign messsge: " + messageHex);

  FileData nostrFile = readFile(SPIFFS, global.nostrFileName.c_str()); // todo: check success
  String privateKeyHex = nostrFile.data;
  int byteSize =  32;
  byte privateKeyBytes[byteSize];
  fromHex(privateKeyHex, privateKeyBytes, byteSize);
  PrivateKey privateKey(privateKeyBytes);
  logInfo("privateKey address: " + privateKey.address());


  byte messageBytes[byteSize];
  fromHex(messageHex, messageBytes, byteSize);
  SchnorrSignature signature = privateKey.schnorr_sign(messageBytes);

  String signatureHex = String(signature);
  logInfo("Schnorr Signature: " + signatureHex);  
  sendCommandOutput(COMMAND_SIGN_MESSAGE, signatureHex);

  // todo: wait for button click
  showMessage("Signed Message", signatureHex.substring(0, 16) + "...");
  delay(10000);

  return {"Signed Message", signatureHex.substring(0, 16) + "..."};
}
