#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

fs::SPIFFSFS &FlashFS = SPIFFS;
#define FORMAT_ON_FAIL true
#define PARAM_FILE "/elements.json"

///////////CHANGE////////////////
         
int portalPin = 4;

/////////////////////////////////

// Access point variables
String payloadStr;
String password;
String serverFull;
String lnbitsServer;
String ssid;
String wifiPassword;
String deviceId;
String highPin;
String timePin;
String pinFlip;
String dataId;
String lnurl;

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
      Serial.println("Launch portal");
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

  if (triggerConfig == false){
    WiFi.begin(ssid.c_str(), wifiPassword.c_str());
    while (WiFi.status() != WL_CONNECTED && timer < 20000) {
      delay(500);
      digitalWrite(2, HIGH);
      Serial.print(".");
      timer = timer + 1000;
      if(timer > 19000){
        triggerConfig = true;
      }
      delay(500);
      digitalWrite(2, LOW);
    }
  }

  if (triggerConfig == true)
  {
    digitalWrite(2, HIGH);
    Serial.println("USB Config triggered");
    configOverSerialPort();
  }

  pinMode(highPin.toInt(), OUTPUT);
  onOff();
  Serial.println(lnbitsServer + "/api/v1/ws/" + deviceId);
  webSocket.beginSSL(lnbitsServer, 443, "/api/v1/ws/" + deviceId);
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  while(WiFi.status() != WL_CONNECTED){
    Serial.println("Failed to connect");
    delay(500);
  }
  digitalWrite(2, LOW);
  paid = false;
  while(paid == false){
    webSocket.loop();
    if(paid){
      Serial.println(payloadStr);
      highPin = getValue(payloadStr, '-', 0);
      Serial.println(highPin);
      timePin = getValue(payloadStr, '-', 1);
      Serial.println(timePin);
      onOff();
    }
  }
  Serial.println("Paid");
}

//////////////////HELPERS///////////////////

void onOff()
{ 
  pinMode (highPin.toInt(), OUTPUT);
  if(pinFlip == "true"){
    digitalWrite(highPin.toInt(), LOW);
    delay(timePin.toInt());
    digitalWrite(highPin.toInt(), HIGH); 
    delay(2000);
  }
  else{
    digitalWrite(highPin.toInt(), HIGH);
    delay(timePin.toInt());
    digitalWrite(highPin.toInt(), LOW); 
    delay(2000);
  }
}

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
    StaticJsonDocument<1500> doc;
    DeserializationError error = deserializeJson(doc, paramFile.readString());

    const JsonObject maRoot0 = doc[0];
    const char *maRoot0Char = maRoot0["value"];
    password = maRoot0Char;
    Serial.println(password);

    const JsonObject maRoot1 = doc[1];
    const char *maRoot1Char = maRoot1["value"];
    ssid = maRoot1Char;
    Serial.println(ssid);

    const JsonObject maRoot2 = doc[2];
    const char *maRoot2Char = maRoot2["value"];
    wifiPassword = maRoot2Char;
    Serial.println(wifiPassword);

    const JsonObject maRoot3 = doc[3];
    const char *maRoot3Char = maRoot3["value"];
    serverFull = maRoot3Char;
    lnbitsServer = serverFull.substring(5, serverFull.length() - 33);
    deviceId = serverFull.substring(serverFull.length() - 22);

    const JsonObject maRoot4 = doc[4];
    const char *maRoot4Char = maRoot4["value"];
    lnurl = maRoot4Char;
    Serial.println(lnurl);
  }
  paramFile.close();
}

//////////////////NODE CALLS///////////////////

void checkConnection(){
  WiFiClientSecure client;
  client.setInsecure();
  const char* lnbitsserver = lnbitsServer.c_str();
  if (!client.connect(lnbitsserver, 443)){
    down = true;
    return;   
  }
}

//////////////////WEBSOCKET///////////////////

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);
                

			    // send message to server when Connected
				webSocket.sendTXT("Connected");
            }
            break;
        case WStype_TEXT:
            payloadStr = (char*)payload;
            paid = true;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    }
}
