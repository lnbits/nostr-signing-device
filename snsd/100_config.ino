// do not change the version manually or move the #define it will be replaced during release
#define VERSION "v0.0.0"

// uncomment if you dont want to use the configuration file
//#define HARDCODED

// time in seconds for configurator to send /delete-file
// so the device boots in configuration mode
#define BOOTUP_TIMEOUT 3
#define CONFIG_FILE "/elements.json"

// device specific configuration / defaults
#define CONFIG_SSID "my_wifi_ssid"
#define CONFIG_PASSWORD "my_wifi_password"

#ifdef HARDCODED
void setupConfig(){
    Serial.println("Setting hardcoded values...");
    config_ssid = CONFIG_SSID;
    Serial.println("SSID: " + config_ssid);
    config_password = CONFIG_PASSWORD;
    Serial.println("SSID password: " + config_password);
}
#else
#include <FS.h>
#include <SPIFFS.h>

void setupConfig(){
    SPIFFS.begin(true);
    // first give the installer a chance to delete configuration file
    executeConfigBoot();
    String fileContent = readConfig();
    // file does not exist, so we will enter endless config mode
    if (fileContent == "") {
        Serial.println("Config file does not exist.");
        executeConfigForever();
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, fileContent);
    if(error){
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
    }

    config_ssid = getJsonValue(doc, "config_ssid", CONFIG_SSID);
    config_password = getJsonValue(doc, "config_password", CONFIG_PASSWORD);
}

String readConfig() {
    File paramFile = SPIFFS.open(CONFIG_FILE, FILE_READ);
    if (!paramFile) {
        return "";
    }
    String fileContent = paramFile.readString();
    if (fileContent == "") {
        return "";
    }
    paramFile.close();
    return fileContent;
}

String getJsonValue(JsonDocument &doc, const char* name, String defaultValue)
{
    String value = defaultValue;
    for (JsonObject elem : doc.as<JsonArray>()) {
        if (strcmp(elem["name"], name) == 0) {
            value = elem["value"].as<String>();
            Serial.println(String(name) + ": " + value);
            return value;
        }
    }
    Serial.println(String(name) + " (using default): " + value);
    return defaultValue;
}

void executeConfigBoot() {
    Serial.println("Entering boot mode. Waiting for " + String(BOOTUP_TIMEOUT) + " seconds.");
    clearTFT();
    printTFT("BOOT MODE", 21, 21);
    int counter = BOOTUP_TIMEOUT + 1;
    while (counter-- > 0) {
        if (Serial.available() == 0) {
            delay(1000);
            continue;
        }
        Serial.println();
        // if we get serial data in the first 5 seconds, we will enter config mode
        counter = 0;
        executeConfigForever();
    }
    Serial.println("Exiting boot mode.");
    Serial.print("Welcome to the LNbits SNSD!");
    Serial.println(" (" + String(VERSION) + ")");
    clearTFT();
    printTFT("GENERIC", 21, 21);
    printTFT(String(VERSION), 21, 42);
}

void executeConfigForever() {
    Serial.println("Entering config mode. until we receive /config-done.");
    clearTFT();
    printTFT("CONFIG", 21, 21);
    bool done = false;
    while (true) {
        done = executeConfig();
        if (done) {
            Serial.println("Exiting config mode.");
            return;
        }
    }
}

bool executeConfig() {
  if (Serial.available() == 0) return false;
  String data = Serial.readStringUntil('\n');
  Serial.println("received serial data: " + data);
  if (data == "/config-done") {
    delay(1000);
    return true;
  }
  if (data == "/file-remove") {
    SPIFFS.remove(CONFIG_FILE);
  }
  if (data.startsWith("/file-append")) {
    File file = SPIFFS.open(CONFIG_FILE, FILE_APPEND);
    if (!file) {
      file = SPIFFS.open(CONFIG_FILE, FILE_WRITE);
    }
    if (!file) {
      Serial.println("Failed to open file for writing.");
    }
    if (file) {
      int pos = data.indexOf(" ");
      String jsondata = data.substring(pos + 1);
      file.println(jsondata);
      file.close();
    }
  }
  if (data.startsWith("/file-read")) {
    File file = SPIFFS.open(CONFIG_FILE, "r");
    if (file) {
      while (file.available()) {
        String line = file.readStringUntil('\n');
        Serial.println("/file-send " + line);
      }
      file.close();
      Serial.println("/file-done");
    }
    return false;
  }
  return false;
}
#endif
