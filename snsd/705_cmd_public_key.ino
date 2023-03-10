/**
   @brief Get the public key corresponsing to the stored private key

   @return CommandResponse
*/
CommandResponse executePublicKey(String data) {

  logInfo("executePublicKey!! 100");
  String privateKeyHex = "4c89c4e0db793a7fac4881f7b6f209ab2c039d88ca64600537f3a94e855478ab";
  int byteSize =  32;
  byte privateKeyBytes[byteSize];
  fromHex(privateKeyHex, privateKeyBytes, byteSize);
  PrivateKey privateKey(privateKeyBytes);
  PublicKey publicKey = privateKey.publicKey();

  sendCommandOutput(COMMAND_PUBLIC_KEY, toHex(publicKey.point, sizeof(publicKey.point)));

  logInfo("executeSharedSecret!! 110");


  return {"Public Key", "done"};
}
