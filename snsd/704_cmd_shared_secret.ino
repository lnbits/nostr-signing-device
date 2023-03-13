/**
   @brief Build a shared secret (using Diffie-Hellman) between the private key on the device and an external public key.

   @param publicKeyHex: String. The public key of the other party.
                        The format must be 128 hex characters, 64 bytes (x,y). No `02`, `03` or `04` prefixes.
   @return CommandResponse
*/
CommandResponse executeSharedSecret(String publicKeyHex) {
  // todo: validate is hex and size
  FileData nostrFile = readFile(SPIFFS, global.nostrFileName.c_str());
  // todo: check success
  String privateKeyHex = nostrFile.data;

  int byteSize =  32;
  byte privateKeyBytes[byteSize];
  fromHex(privateKeyHex, privateKeyBytes, byteSize);
  PrivateKey privateKey(privateKeyBytes);
  PublicKey myPublicKey = privateKey.publicKey();

  byte sharedSecret[32];

  byte publicKeyBin[64];
  fromHex(publicKeyHex, publicKeyBin, 64);
  PublicKey otherPublicKey(publicKeyBin, true);
  privateKey.ecdh(otherPublicKey, sharedSecret, false);

  String sharedSecretHex = toHex(sharedSecret, sizeof(sharedSecret));
  logInfo("sharedSecretHex: " + sharedSecretHex);
  sendCommandOutput(COMMAND_SHARED_SECRET, sharedSecretHex);

  return {"Shared Secret", "sent..."};
}

