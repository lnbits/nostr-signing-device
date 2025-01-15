/**
   @brief Get the public key corresponsing to the stored private key

   @return CommandResponse
*/
CommandResponse executePublicKey(String data) {
  String privateKeyHex = global.privateKeys[global.activeKeyIndex];
  int byteSize =  32;
  byte privateKeyBytes[byteSize];
  fromHex(privateKeyHex, privateKeyBytes, byteSize);
  PrivateKey privateKey(privateKeyBytes);
  PublicKey publicKey = privateKey.publicKey();

  String publicKeyHex = toHex(publicKey.point, sizeof(publicKey.point));
  sendCommandOutput(COMMAND_PUBLIC_KEY, publicKeyHex.substring(0, 64));

  showMessage("Public key", publicKeyHex.substring(0, 16) + "...");
  
  return {"Public Key", publicKeyHex.substring(0, 16) + "..."};
}
