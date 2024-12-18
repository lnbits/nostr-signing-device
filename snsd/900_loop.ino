void loop() {
  if (!global.unlocked) {
    displayLoginScreen();
  } else {
    listenForCommands();
  }
}
