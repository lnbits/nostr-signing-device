/**
   @brief Show info about the existing commands.

   @param commandData: String.  Not used.
   @return CommandResponse
*/
CommandResponse executeHelp(String commandData) {
  logInfo("Commands");
  logInfo("'/help' show available commands");
  logInfo("'/ping' checks the connection with the device");
  logInfo("'/public-key' returns the public key in hex format");
  logInfo("'/sign-message' sign a message using the private key");
  logInfo("'/shared-secret' build a shared secret using an external public key");
  logInfo("'/add-key' add a new private key to the wallet");
  logInfo("'/remove-key' remove an existing private key by index");
  logInfo("'/list-keys' list all stored private keys");
  logInfo("'/new-key' generate a new private key");
  return {"More info at:", "github.com/lnbits/nostr-signing-device"};
}
