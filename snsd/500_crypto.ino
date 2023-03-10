String hashStringData(String key) {
  byte hash[64] = { 0 };
  int hashLen = 0;
  hashLen = sha256(key, hash);
  return toHex(hash, hashLen);
}

String encryptData(byte key[32], byte iv[16], String msg) {
  // String has a trailing `null` character
  // String.getBytes() can overwrite the last character with `null`.
  // Add some extra-padding for safety
  String data = msg + "        ";

  // Pad data for encryption. Length must be multiple of 16.
  while (data.length() % 16 != 0) data += " ";

  int byteSize = data.length();
  byte messageBin[byteSize];
  data.getBytes(messageBin, byteSize);

  AES_ctx ctx;
  AES_init_ctx_iv(&ctx, key, iv);

  AES_CBC_encrypt_buffer(&ctx, messageBin, sizeof(messageBin));

  return toHex(messageBin, sizeof(messageBin));
}

String encryptDataWithIv(byte key[32], String msg) {
  String data = String(msg.length()) + " " + msg;

  // create random initialization vector
  int ivSize = 16;
  uint8_t iv[ivSize];
  String tempMnemonic = generateStrongerMnemonic(24);
  mnemonicToEntropy(tempMnemonic, iv, ivSize);
  String ivHex = toHex(iv, ivSize);

  String messageHex = encryptData(key, iv, data);

  return messageHex + ivHex;
}


String decryptData(byte key[32], byte iv[16], String messageHex) {
  int byteSize =  messageHex.length() / 2;
  byte messageBin[byteSize];
  fromHex(messageHex, messageBin, byteSize);


  AES_ctx ctx;
  AES_init_ctx_iv(&ctx, key, iv);
  AES_CBC_decrypt_buffer(&ctx, messageBin, sizeof(messageBin));

  return String((char *)messageBin).substring(0, byteSize);
}

String decryptDataWithIv(byte key[32], String messageWithIvHex) {
  int ivSize = 16;
  String messageHex = messageWithIvHex.substring(0, messageWithIvHex.length() - ivSize * 2);
  String ivHex = messageWithIvHex.substring(messageWithIvHex.length() - ivSize * 2, messageWithIvHex.length());

  uint8_t iv[ivSize];
  fromHex(ivHex, iv, ivSize);
  String decryptedData = decryptData(key, iv, messageHex);

  Command c = extractCommand(decryptedData);
  int commandLength = c.cmd.toInt();
  return c.data.substring(0, commandLength);
}