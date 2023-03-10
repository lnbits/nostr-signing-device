/**
   @brief Sign a random message.

   @param messageHex: String. The message to be signed in hex format.
   @return CommandResponse
*/
CommandResponse executeSignMessage(String messageHex) {
  showMessage("Please wait", "Signing message...");

  logInfo("Preparing to sign messsge: " + messageHex);

  String privateKeyHex = "4c89c4e0db793a7fac4881f7b6f209ab2c039d88ca64600537f3a94e855478ab";
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
