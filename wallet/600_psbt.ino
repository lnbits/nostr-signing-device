
PSBT parseBase64Psbt(String psbtBase64) {
  PSBT psbt;
  psbt.parseBase64(psbtBase64);
  return psbt;
}

void printOutputDetails(PSBT psbt, HDPrivateKey hd, int index, const Network * network) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.println("Output " + String(index));
  tft.setTextSize(1);
  tft.println("");
  bool isChange = isChangeAddress(hd, psbt.txOutsMeta[index], psbt.tx.txOuts[index]);
  if (isChange == true) tft.print("Change ");
  tft.println("Address:");
  tft.println("");

  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(psbt.tx.txOuts[index].address(network));
  tft.setTextSize(1);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("");
  tft.println("Amount:");
  tft.println("");
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  String sats = int64ToString(psbt.tx.txOuts[index].amount);
  printSats(sats, 2);
  tft.println(" sat");
}

void printFeeDetails(uint64_t fee) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0, 30);
  tft.setTextSize(2);
  tft.print("Fee: ");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  String sats = int64ToString(fee);
  printSats(sats, 2);
  tft.println(" sat");
}

bool isChangeAddress(HDPrivateKey hd, PSBTOutputMetadata txOutMeta, TxOut txOut) {
  logSerial("### isChangeAddress IN " );
  if (txOutMeta.derivationsLen > 0) { // there is derivation path
    logSerial("### isChangeAddress address1 " + txOut.address());
    logSerial("### isChangeAddress derivationsLen " + String(txOutMeta.derivationsLen));
    // considering only single key for simplicity
    PSBTDerivation der = txOutMeta.derivations[0];
    // logSerial("### isChangeAddress der " + String(der.derivation));
    HDPublicKey pub = hd.derive(der.derivation, der.derivationLen).xpub();
    logSerial("### isChangeAddress pub " + pub.xpub());
    logSerial("### isChangeAddress address 2" +  pub.address() + " " + txOut.address() );
    return pub.address() == txOut.address();
  }
  logSerial("### isChangeAddress OUT " );
  return false;
}

void printSats(String sats, int textSize) {
  int len = sats.length();
  int offest = len % 3;
  for (int i = 0; i < len; i++) {
    tft.print(sats[i]);
    if ((i + 1 - offest) % 3 == 0) {
      tft.setTextSize(1);
      tft.print(" ");
      tft.setTextSize(textSize);
    }
  }
}
