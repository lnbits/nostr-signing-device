CommandResponse executePing(String data) {
  if (data == "") return {"", ""};
  
  Serial.println(COMMAND_PING + " 0 " + global.deviceId);
  return {"Contacted by", data};
}