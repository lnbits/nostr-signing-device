CommandResponse executeShowAddress(String addressData) {
  if (global.authenticated == false) {
    return {"Enter password!", "8 numbers/letters"};
  }

  String networkName = getWordAtPosition(addressData, 0);
  String path = getWordAtPosition(addressData, 1);
  String address = getWordAtPosition(addressData, 2);

  const Network * network;
  if (networkName == "Mainnet") {
    network = &Mainnet;
  } else if (networkName == "Testnet") {
    network = &Testnet;
  } else {
    return {"Unknown Network",  "Must be Mainnet or Testnet"};
  }

  if (!path) {
    return {"Derivation path missing!", "Address cannot be derived"};
  }

  HDPrivateKey hd(global.mnemonic, global.passphrase, network);

  HDPrivateKey pK = hd.derive(path);
  String derivedAddress = pK.address();
  if (!derivedAddress.equals(address)) {
    return {"Danger! Address missmatch!", "Derived address different thant the UI address"};
  }

  EventData event = toggleDatanAndQR(derivedAddress, false);

  return {"", "", 0, event};
}
