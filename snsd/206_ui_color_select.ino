struct ColorOption {
    String name;
    uint16_t value;
};

ColorOption colorOptions[] = {
    {"Lnbits Purple", TFT_LNBITS_PURPLE},
    {"Navy", TFT_NAVY},
    {"Dark Green", TFT_DARKGREEN},
    {"Dark Cyan", TFT_DARKCYAN},
    {"Maroon", TFT_MAROON},
    {"Purple", TFT_PURPLE},
    {"Olive", TFT_OLIVE},
    {"Light Grey", TFT_LIGHTGREY},
    {"Dark Grey", TFT_DARKGREY},
    {"Blue", TFT_BLUE},
    {"Green", TFT_GREEN},
    {"Cyan", TFT_CYAN},
    {"Red", TFT_RED},
    {"Magenta", TFT_MAGENTA},
    {"Yellow", TFT_YELLOW},
    {"Orange", TFT_ORANGE},
    {"Green Yellow", TFT_GREENYELLOW},
    {"Pink", TFT_PINK}
};

void displayColorSelectScreen() {
    global.onLogo = false;
    setDisplay(true);

    int totalColors = sizeof(colorOptions) / sizeof(ColorOption);
    int selectedIndex = 0;
    
    // Find current color index
    for (int i = 0; i < totalColors; i++) {
        if (colorOptions[i].value == global.accentColor) {
            selectedIndex = i;
            break;
        }
    }

    unsigned long lastButton1Press = 0;

    // Full screen clear
    tft.fillScreen(global.backgroundColor);

    while (true) {
        tft.setTextColor(global.foregroundColor, global.backgroundColor);
        tft.setTextSize(2);
        tft.setCursor(0, 10);
        tft.println("Select Color:");
        tft.println("");
        
        // Show color name in its own color
        tft.setTextColor(colorOptions[selectedIndex].value, global.backgroundColor);
        tft.println(padRightWithSpaces(colorOptions[selectedIndex].name, 20));
        tft.println("");

        tft.setTextColor(global.foregroundColor, global.backgroundColor);
        tft.setTextSize(2);
        tft.setCursor(0, 100 * global.scaleFactor);
        tft.println("Top to switch");
        tft.println("Bottom to confirm");

        EventData event = awaitEvent();

        if (event.type == EVENT_BUTTON_ACTION) {
            String buttonNumber = getWordAtPosition(event.data, 0);
            String buttonState = getWordAtPosition(event.data, 1);

            if (buttonState == "1") {
                if (buttonNumber == "2") {
                    unsigned long currentMillis = millis();
                    if (currentMillis - lastButton1Press > global.debounceDelay) {
                        lastButton1Press = currentMillis;
                        selectedIndex = (selectedIndex + 1) % totalColors;
                    }
                } else if (buttonNumber == "1") {
                    global.accentColor = colorOptions[selectedIndex].value;
                    saveAccentColor();
                    showMessage("Color Selected", colorOptions[selectedIndex].name);
                    return;
                }
            }
        } else if (event.type == EVENT_SCREEN_IDLE) {
            displayLogoScreen();
            return;
        }
    }
}

void saveAccentColor() {
    writeFile(SPIFFS, global.accentColorFileName.c_str(), String(global.accentColor));
}

void loadAccentColor() {
    FileData colorFile = readFile(SPIFFS, global.accentColorFileName.c_str());
    if (colorFile.success) {
        global.accentColor = colorFile.data.toInt();
    }
} 