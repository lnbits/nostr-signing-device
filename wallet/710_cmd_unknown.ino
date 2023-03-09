CommandResponse executeUnknown(String details) {
  logSerial("Unknown command: "+ details);
  return {"Unknown command",  details};
}