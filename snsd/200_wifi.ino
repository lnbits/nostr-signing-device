#include <WiFi.h>

void setupWifi() {
    WiFi.begin(config_ssid.c_str(), config_password.c_str());
    Serial.print("Connecting to WiFi.");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
        digitalWrite(2, HIGH);
        Serial.print(".");
        delay(500);
        digitalWrite(2, LOW);
    }
    Serial.println();
    Serial.println("WiFi connection etablished!");
    printTFT("WiFi connected!", 21, 69);
}

void loopWifi() {
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected!");
        delay(500);
    }
}
