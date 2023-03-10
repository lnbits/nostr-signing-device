/**
   @brief Build a shared secret (using Diffie-Hellman) between the private key on the device and an external public key.

   @param publicKeyHex: String. The public key of the other party.
   @return CommandResponse
*/
CommandResponse executeSharedSecret(String publicKeyHex) {
  // pubkey hex: c1415f950a1e3431de2bc5ee35144639e2f514cf158279abff9ed77d50118796404466b433cd01fb26757a9cf65b1ba4407ef3d293feebd4a0658060d9eb491f
  // python: #### shared_secret ca0653175f9069b104c5d929866a7cccd4002c7a9a21e1cb053a42fd311fb2db
  // arduimo: bbe91483b639859885ccbd67aa307a21dea00cc075618be118e3416b407444a2
  logInfo("executeSharedSecret!! 100");
  String privateKeyHex = "4c89c4e0db793a7fac4881f7b6f209ab2c039d88ca64600537f3a94e855478ab";
  int byteSize =  32;
  byte privateKeyBytes[byteSize];
  fromHex(privateKeyHex, privateKeyBytes, byteSize);
  PrivateKey privateKey(privateKeyBytes);
  // PublicKey dhPublicKey = privateKey.publicKey();

  logInfo("executeSharedSecret!! 110");

  byte sharedSecret[32];

  byte publicKeyBin[64];
  fromHex("02" + publicKeyHex, publicKeyBin, 64);
  PublicKey otherPublicKey(publicKeyBin, true);
  privateKey.ecdh(otherPublicKey, sharedSecret, false);

  logInfo("executeSharedSecret!! 120");

  String sharedSecretHex = toHex(sharedSecret, 32);
  logInfo("sharedSecretHex: " + sharedSecretHex);
  sendCommandOutput(COMMAND_SHARED_SECRET, sharedSecretHex);

  logInfo("executeSharedSecret!! 130");

  // logInfo("xxxx: " + toHex(dhPublicKey.point, sizeof(dhPublicKey.point)));
  // todo: wait for button click
  delay(10000);

  return {"Shared Secret", "done"};
}


// CommandResponse pair(String publicKeyHex, String button1Pin, String button2Pin) {
//   String tempMnemonic = generateStrongerMnemonic(24);
//   byte dhe_secret[32];
//   mnemonicToEntropy(tempMnemonic, dhe_secret, sizeof(dhe_secret));

//   PrivateKey dhPrivateKey(dhe_secret);
//   PublicKey dhPublicKey = dhPrivateKey.publicKey();

//   byte publicKeyBin[64];
//   fromHex(publicKeyHex, publicKeyBin, 64);
//   PublicKey otherDhPublicKey(publicKeyBin, false);
//   dhPrivateKey.ecdh(otherDhPublicKey, global.dhe_shared_secret, false);

//   Serial.println(COMMAND_PAIR + " 0 " + toHex(dhPublicKey.point, sizeof(dhPublicKey.point)));

//   String sharedSecretHex =  toHex(global.dhe_shared_secret, sizeof(global.dhe_shared_secret));
//   deleteFile(SPIFFS, global.sharedSecretFileName.c_str());
//   writeFile(SPIFFS, global.sharedSecretFileName.c_str(), sharedSecretHex);

//   updateDeviceConfig(button1Pin, button2Pin);

//   String fingerprint = hashStringData(sharedSecretHex).substring(0, 5);
//   fingerprint.toUpperCase();
//   return {"Confirm", "Code: " + fingerprint };
// }