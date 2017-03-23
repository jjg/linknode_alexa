#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

#define SWITCH_1        14   // top-left
#define SWITCH_2        12   // top-right
#define SWITCH_3        13   // bottom-left
#define SWITCH_4        16   // bottom-right

#define SERIAL_BAUDRATE 115200

// Wifi credentials REMOVE BEFORE COMMITTING TO GITHUB!!!
#define WIFI_SSID "theupsidedown"
#define WIFI_PASS "Sevencome11"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void setup() {
  
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // Wifi
    wifiSetup();

    // Relays
    pinMode(SWITCH_1, OUTPUT);
    pinMode(SWITCH_2, OUTPUT);
    pinMode(SWITCH_3, OUTPUT);
    pinMode(SWITCH_4, OUTPUT);

    // Fauxmo
    fauxmo.addDevice("master bedroom lights");
    fauxmo.addDevice("master bedroom fan");
    fauxmo.addDevice("master bedroom humidifier");
    fauxmo.addDevice("master bedroom other");
    
    // fauxmoESP 2.0.0 has changed the callback signature to add the device_id, this WARRANTY
    // it's easier to match devices to action without having to compare strings.
    fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");

        switch(device_id){
          case 0: 
            digitalWrite(SWITCH_1, state);
            break;
          case 1:
            digitalWrite(SWITCH_2, state);
            break;
          case 2:
            digitalWrite(SWITCH_3, state);
            break;
          case 3:
            digitalWrite(SWITCH_4, state);
            break;
          default:
            Serial.printf("Unhandled device #%d\n", device_id);
            break;
        }
    });
}

void loop() {

    fauxmo.handle();

    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }
}
