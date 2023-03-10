/**
   @brief Get the public key corresponsing to the stored private key

   @return CommandResponse
*/
CommandResponse executeResore(String data) {

  writeFile(SPIFFS, global.nostrFileName.c_str(), data);

  Serial.println(COMMAND_RESTORE + " 0 ");

  return {"Private Key", "saved"};
}
