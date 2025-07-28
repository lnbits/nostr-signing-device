int button1State = HIGH;
int button2State = HIGH;
int lineNumber = 0;


EventData awaitEvent() {
  if (global.hasCommandsFile == true) {
    return awaitFileEvent();
  }
  return awaitSerialEvent();
}

EventData awaitFileEvent() {
  String line = getNextFileCommand();
  delay(500);
  return { EVENT_INTERNAL_COMMAND, line };
}

/**
   @brief Get the next command from file.
   It will enter an infinite loop when it reaches the end of the file.
   It skips empty lines and commented lines (that start with `#`).
   @return String
*/
String getNextFileCommand() {
  String line = "";
  do {
    line = getLineAtPosition(global.commands, lineNumber);
    lineNumber++;
  } while (line.startsWith("#") || line == "");
  return line;
}

EventData awaitSerialEvent() {
  unsigned long waitTime = millis();
  while (Serial.available() == 0) {
    if  ((millis() - waitTime) > global.screenTimeout * 1000) {
      waitTime = millis();
      if(global.unlocked) {
        return {EVENT_SCREEN_IDLE,""};
      }
    }

    EventData buttonEvent = checkButtonsState();

    // Handle button presses on the menu screen
    if (buttonEvent.type == EVENT_BUTTON_ACTION && global.onLogo) {
      String buttonNumber = getWordAtPosition(buttonEvent.data, 0);
      String buttonState = getWordAtPosition(buttonEvent.data, 1);

      if (buttonState == "1") {
        if (buttonNumber == "2") {
          menu();
          displayLogoScreen();
        } else if (buttonNumber == "1") {
          toggleDisplay();
        }
      }
    } else if (buttonEvent.type == EVENT_BUTTON_ACTION) {
      setDisplay(true);
      // Return other button presses
      return buttonEvent;
    }

  }
  String data = Serial.readStringUntil('\n');
  return { EVENT_SERIAL_DATA, data };
}

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;

EventData checkButtonsState() {
  int button1NewState = digitalRead(BTN_1_PIN);
  int button2NewState = digitalRead(BTN_2_PIN);
  unsigned long currentTime = millis();

  if (button1NewState != button1State && (currentTime - lastDebounceTime1) > global.debounceDelay) {
    lastDebounceTime1 = currentTime;
    logInfo("button 1: " + String(button1NewState));
    button1State = button1NewState;
    return { EVENT_BUTTON_ACTION, "1 " + String(button1NewState) };
  }

  if (button2NewState != button2State && (currentTime - lastDebounceTime2) > global.debounceDelay) {
    lastDebounceTime2 = currentTime;
    logInfo("button 2: " + String(button2NewState));
    button2State = button2NewState;
    return { EVENT_BUTTON_ACTION, "2 " + String(button2NewState) };
  }

  return { "", "" };
}
