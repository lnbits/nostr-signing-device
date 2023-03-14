/**
   @brief Executed when the command is unkonwn.
    Does nothing
   @param details: String. The name of the unknown command.
   @return CommandResponse
*/
CommandResponse executeUnknown(String details) {
  logInfo("Unknown command: " + details);
  logo();
  return {"",  ""};
}
