/**
   @brief Get the public key corresponsing to the stored private key

   @return CommandResponse
*/
CommandResponse executePublicKey(String data) {

  FileData nostrFile = readFile(SPIFFS, global.nostrFileName.c_str()); // todo: check success
  String privateKeyHex = nostrFile.data;
  int byteSize =  32;
  byte privateKeyBytes[byteSize];
  fromHex(privateKeyHex, privateKeyBytes, byteSize);
  PrivateKey privateKey(privateKeyBytes);
  PublicKey publicKey = privateKey.publicKey();

  String publicKeyHex = toHex(publicKey.point, sizeof(publicKey.point));
  sendCommandOutput(COMMAND_PUBLIC_KEY, publicKeyHex.substring(0, 64));



  return {"Public Key", publicKeyHex.substring(0, 16) + "..."};
}
