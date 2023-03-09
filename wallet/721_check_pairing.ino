CommandResponse executeCheckPairing(String encryptedData) {
  String sharedSecret = toHex(global.dhe_shared_secret, sizeof(global.dhe_shared_secret));

  if (sharedSecret.equals("0000000000000000000000000000000000000000000000000000000000000000")) {
    return {"Ready", "For encrypted connection"};
  }
  String data = decryptDataWithIv(global.dhe_shared_secret, encryptedData);
  if (data == PAIRING_CONTROL_TEXT) {
    Serial.println(COMMAND_CHECK_PAIRING + " 0 " + encryptDataWithIv(global.dhe_shared_secret, PAIRING_CONTROL_TEXT));
    return {"Paired", "Encrypted connection"};
  }
  Serial.println(COMMAND_CHECK_PAIRING + " 1 must_repair");
  return {"Must re-pair", "Restart device"};
}