//========================================================================//
//===============================UI STUFF=================================//
//========================================================================//

void displayLogoScreen() {
  // Check if we're already on the displayLogoScreen (avoids flicker when idle)
  if(global.onLogo == true) return;

  global.onLogo = true;

  tft.fillScreen(TFT_BLACK);

  int centerX = 240 / 2;
  int centerY = 135 / 2;
  int circleRadius = 50;

  // Draw the purple circle
  tft.fillCircle(centerX, centerY, circleRadius, TFT_LNBITS_PURPLE);

  // Draw the lightning bolt
  int xOffset = centerX - 15;
  int yOffset = centerY - 35;

  tft.fillTriangle(xOffset + 0, yOffset + 0, 
                   xOffset + 30, yOffset + 0, 
                   xOffset + 0, yOffset + 40, TFT_WHITE);

  tft.fillTriangle(xOffset + 10, yOffset + 25, 
                   xOffset + 30, yOffset + 25, 
                   xOffset + 10, yOffset + 70, TFT_WHITE);

  tft.fillTriangle(xOffset + 30, yOffset + 0, 
                   xOffset + 13, yOffset + 40, 
                   xOffset + 0, yOffset + 40, TFT_WHITE);

  // Draw rotated "LNbits" text on the left side
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextDatum(MC_DATUM);

  tft.setRotation(0);

  // Draw "LN" in bold by overlaying text slightly offset
  tft.setTextColor(TFT_WHITE);
  
  tft.setCursor(12, 2);
  tft.print("L");
  tft.setCursor(12, 0);
  tft.print("L");
  tft.setCursor(14, 0);
  tft.print("L");
  tft.setCursor(14, 2);
  tft.print("L");

  tft.setCursor(35, 2);
  tft.print("N");
  tft.setCursor(35, 0);
  tft.print("N");
  tft.setCursor(37, 0);
  tft.print("N");
  tft.setCursor(37, 2);
  tft.print("N");

  // Draw "bits" normal text next to "LN"
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(60, 2);
  tft.print("bits");

  tft.setRotation(1);

  // Draw build version at bottom-right
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  int textWidth = tft.textWidth(env.version); // Get text width
  int textX = 240 - textWidth - 2; 
  int textY = 135 - 12;

  tft.setCursor(textX, textY);
  tft.print(env.version);
}

void showMessage(String message, String additional)
{
  global.onLogo = false;
  setDisplay(true);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 30);
  tft.println(message);

  tft.setTextColor(TFT_LNBITS_PURPLE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 80);
  tft.println(additional);

  delay(2000);
}

void toggleDisplay() {
  if (global.isDisplayOn) {
    tft.writecommand(TFT_DISPOFF);
    digitalWrite(global.backlightPin, LOW);
  } else {
    tft.writecommand(TFT_DISPON);
    digitalWrite(global.backlightPin, HIGH);
  }

  global.isDisplayOn = !global.isDisplayOn;
}

void setDisplay(bool state) {
  if (!state) {
    tft.writecommand(TFT_DISPOFF);
    digitalWrite(global.backlightPin, LOW);
  } else {
    tft.writecommand(TFT_DISPON);
    digitalWrite(global.backlightPin, HIGH);
  }

  global.isDisplayOn = state;
}

void displayLoginScreen() {
  global.onLogo = false;
  global.unlocked = false;
  setDisplay(true);

  if (global.pinCode == "00000000") {
    // Unset PIN, skip login screen
    global.unlocked = true;
    displayLogoScreen();
    return;
  }

  const String correctPin = global.pinCode; // The correct pin stored as a String
  const int pinLength = 8;
  String enteredPin = "";

  int currentDigit = 0;
  unsigned long lastButtonPressTime = 0;

  // Initialize failed attempt count
  const int maxAttempts = 3;

  // Full screen clear
  tft.fillScreen(TFT_BLACK);

  while (true) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 10);
    tft.println("Enter PIN:");
    tft.println("");

    // Display entered PIN with masking except for the current digit
    String displayPin = "";
    for (int i = 0; i < pinLength; i++) {
      if (i < enteredPin.length()) {
        displayPin += "*"; // Mask confirmed digits
      } else if (i == enteredPin.length()) {
        displayPin += String(currentDigit); // Show current digit being changed
      } else {
        displayPin += "*"; // Mask remaining digits
      }
    }

    tft.setTextSize(3);
    tft.setTextColor(TFT_LNBITS_PURPLE, TFT_BLACK);
    tft.println(displayPin);
    tft.println("");

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 90);
    tft.println("Top to cycle digit");
    tft.println("Bottom to confirm");

    EventData event = awaitEvent();
    unsigned long currentMillis = millis();

    if (event.type == EVENT_BUTTON_ACTION) {
      String buttonNumber = getWordAtPosition(event.data, 0);
      String buttonState = getWordAtPosition(event.data, 1);

      if (buttonNumber == "2" && buttonState == "1") { // Cycle digit button pressed
        if (currentMillis - lastButtonPressTime > global.debounceDelay) {
          lastButtonPressTime = currentMillis;
          currentDigit = (currentDigit + 1) % 10; // Increment the current digit
        }
      } else if (buttonNumber == "1" && buttonState == "1") { // Confirm digit or submit
        if (enteredPin.length() < pinLength) {
          enteredPin += String(currentDigit); // Confirm the current digit

          if (enteredPin.length() == pinLength) {
            if (enteredPin == correctPin) {
              showMessage("PIN Accepted", "Access Granted");
              global.unlocked = true;
              global.pinAttempts = 0; // Reset failed attempts on success
              writeFile(SPIFFS, global.pinAttemptsFileName.c_str(), "0");
              return; // Exit the function upon successful PIN entry
            } else {
              global.pinAttempts++;
              writeFile(SPIFFS, global.pinAttemptsFileName.c_str(), String(global.pinAttempts)); // Log failed attempt

              if (global.pinAttempts >= maxAttempts) {
                SPIFFS.format(); // Wipe device
                ESP.restart(); // Restart after max attempts
              } else {
                int attemptsLeft = maxAttempts - global.pinAttempts;
                showMessage("PIN Denied", String(attemptsLeft) + " Attempts Left");
                enteredPin = ""; // Reset the entered PIN
                currentDigit = 0;

                // Full screen clear
                tft.fillScreen(TFT_BLACK);
              }
            }
          }
        }
      }
    }
  }
}

void displaySetPinScreen() {
  global.onLogo = false;
  setDisplay(true);

  const int pinLength = 8;
  String newPin = "";
  int currentDigit = 0;

  unsigned long lastButtonPressTime = 0;

  // Full screen clear
  tft.fillScreen(TFT_BLACK);

  while (true) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 10);
    tft.println("Set New PIN:");
    tft.println("");

    // Display the entered PIN with remaining digits as underscores
    String displayPin = "";
    for (int i = 0; i < pinLength; i++) {
      if (i < newPin.length()) {
        displayPin += newPin[i]; // Show confirmed digits
      } else if (i == newPin.length()) {
        displayPin += String(currentDigit); // Show the current digit being adjusted
      } else {
        displayPin += "_"; // Show remaining digits as underscores
      }
    }

    tft.setTextSize(3);
    tft.setTextColor(TFT_LNBITS_PURPLE, TFT_BLACK);
    tft.println(displayPin);
    tft.println("");

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 90);
    tft.println("Top to cycle digit");
    tft.println("Bottom to confirm");

    EventData event = awaitEvent();
    unsigned long currentMillis = millis();

    if (event.type == EVENT_BUTTON_ACTION) {
      String buttonNumber = getWordAtPosition(event.data, 0);
      String buttonState = getWordAtPosition(event.data, 1);

      if (buttonNumber == "2" && buttonState == "1") { // Cycle digit button pressed
        if (currentMillis - lastButtonPressTime > global.debounceDelay) {
          lastButtonPressTime = currentMillis;
          currentDigit = (currentDigit + 1) % 10; // Increment the current digit
        }
      } else if (buttonNumber == "1" && buttonState == "1") { // Confirm digit or complete PIN
        if (newPin.length() < pinLength) {
          newPin += String(currentDigit); // Add the current digit to the PIN

          if (newPin.length() == pinLength) {
            // PIN entry completed
            savePIN(newPin);
            global.pinCode = newPin;
            showMessage("PIN Set", "Your new PIN is set.");
            return; // Exit the function
          }
        }
      }
    }
  }
}