#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

fs::SPIFFSFS &FlashFS = SPIFFS;
#define FORMAT_ON_FAIL true
#define PARAM_FILE "/elements.json"

///////////CHANGE////////////////
         
int portalPin = 4;

/////////////////////////////////

// Access point variables
String payloadStr;
String timePin;
String pinFlip;
String dataId;
String privatekey;

bool paid;
bool down = false;
bool triggerConfig = false; 

WebSocketsClient webSocket;

struct KeyValue {
  String key;
  String value;
};

void setup()
{
  Serial.begin(115200);

  int timer = 0;
  pinMode (2, OUTPUT);

  while (timer < 2000)
  {
    digitalWrite(2, LOW);
    Serial.println(touchRead(portalPin));
    if(touchRead(portalPin) < 40){
      Serial.println("Launch config");
      triggerConfig = true;
      timer = 5000;
    }
    delay(150);
    digitalWrite(2, HIGH);
    timer = timer + 300;
    delay(150);
  }

  timer = 0;

  FlashFS.begin(FORMAT_ON_FAIL);

  // get the saved details and store in global variables
  readFiles();

  if (triggerConfig == true)
  {
    digitalWrite(2, HIGH);
    Serial.println("USB Config triggered");
    configOverSerialPort();
  }
}

void loop() {

  // Wait for an event to sign

}

//////////////////HELPERS///////////////////


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void readFiles()
{
  File paramFile = FlashFS.open(PARAM_FILE, "r");
  if (paramFile)
  {
    StaticJsonDocument<500> doc;
    DeserializationError error = deserializeJson(doc, paramFile.readString());

    const JsonObject maRoot0 = doc[0];
    const char *maRoot0Char = maRoot0["value"];
    privatekey = maRoot0Char;
    Serial.println(privatekey);

  }
  paramFile.close();
}