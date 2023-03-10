/**
   @brief Unenrypted `ping` command.
   It will respond with a ping command over `Serial`.
   @param data: String (optional). Some identification token from the client.
   @return CommandResponse
*/
CommandResponse executePing(String data) {
  if (data == "") return {"", ""};
  
  Serial.println(COMMAND_PING + " 0 " + global.deviceId);
  return {"Contacted by", data};
}
