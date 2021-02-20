#include "Arduino.h"

#include <WiFi.h>
#include "esp_wifi.h"
#include <SPIFFS.h>
#include "ArduinoJson.h"
using namespace std;

const char* wifi_ssid;
const char* wifi_password;
const char* glowing_stripes_ssid = "TheGlowingStripes";
const char* glowing_stripes_password = "letsglow";

boolean connect_to_wifi(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || connect_to_wifi()");
    Serial.println("");
    if (SPIFFS.exists("/wifi_credentials.json")){
        Serial.println("Wifi credientias found");
        StaticJsonDocument<60> wifi_credentials;
        File wifi_credentials_file = SPIFFS.open("/wifi_credentials.json");
        if(!wifi_credentials_file){
            Serial.println("Failed to open wifi_credentials for reading");
        }else {
            DeserializationError error = deserializeJson(wifi_credentials, wifi_credentials_file);
            if (error){
                Serial.println("Failed to read wifi_credentials_file.");
            } else {
                Serial.println("Loaded wifi_credentials.json");
            }
        }
        wifi_credentials_file.close();
        wifi_ssid = wifi_credentials["0"];
        wifi_password = wifi_credentials["1"];
    } else {
        wifi_ssid = glowing_stripes_ssid;
        wifi_password = glowing_stripes_password;
    }
    
    Serial.println("");
    Serial.print("Connecting to wifi: ");
    Serial.print(wifi_ssid);
    Serial.println("");
    delay(2000);

    // if credentials for a wifi exist, connect to wifi, else connect to TheGlowingStripes hotspot
    WiFi.begin(wifi_ssid, wifi_password);                  // to tell Esp32 Where to connect and trying to connect
    delay(500);

    // if that fails, return false (causing a restart)
    while (WiFi.status() != WL_CONNECTED) {                // While loop for checking Internet Connected or not
        return false;
    }
    Serial.println("");
    Serial.print("Connected LED strip to Wifi: ");
    Serial.print(wifi_ssid);
    Serial.println("");
    return true;
}

void start_wifi(){
    // if wifi from wifi credentials is available, connect to it
    // else connect to theglowingstripes.local network
    if (connect_to_wifi()==false){
        // if theglowingstripes.local also doesn't exist, restart esp
        ESP.restart();
    }
    // Info: as long as "start_wifi" is executed before leds start - this should cause LED flickering
    // Only when OTA is used to update LED strip code, this will cause flickering
}