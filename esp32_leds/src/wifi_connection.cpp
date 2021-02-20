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

boolean wifi_from_credentials_is_online(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || wifi_from_credentials_is_online()");
    Serial.println("");

    Serial.println("Check if host wifi is online...");
    
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("no networks found");
        WiFi.scanDelete();
        return false;
    } else {
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
            } else {
                wifi_ssid = glowing_stripes_ssid;
            }
            
            for (int i = 0; i < n; ++i) {
                // Print SSID and RSSI for each network found
                if (WiFi.SSID(i)==wifi_ssid){
                    Serial.println("Host wifi found!");
                    return true;
                }
            }
    }
    WiFi.scanDelete();
    Serial.println("Host wifi NOT found!");
    return false;
}

boolean default_host_wifi_is_online(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || default_host_wifi_is_online()");
    Serial.println("");

    Serial.println("Check if default host wifi is online...");
    
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("no networks found");
        WiFi.scanDelete();
        return false;
    } else {
            wifi_ssid = glowing_stripes_ssid;
            for (int i = 0; i < n; ++i) {
                // Print SSID and RSSI for each network found
                if (WiFi.SSID(i)==wifi_ssid){
                    Serial.println("Host wifi found!");
                    return true;
                }
            }
    }
    WiFi.scanDelete();
    Serial.println("Default host wifi NOT found!");
    return false;
}

boolean connect_to_wifi_from_credentials(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || connect_to_wifi_from_credentials()");
    Serial.println("");
    // if credentials for a wifi exist, connect to wifi, else connect to TheGlowingStripes hotspot
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

boolean connect_to_default_host_wifi(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || connect_to_default_host_wifi()");
    Serial.println("");

    wifi_ssid = glowing_stripes_ssid;
    wifi_password = glowing_stripes_password;
    
    Serial.println("");
    Serial.print("Connecting to wifi: ");
    Serial.print(wifi_ssid);
    Serial.println("");
    delay(2000);

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
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_wifi()");
    Serial.println("");
    // see if host wifi is nearby (the one defined in credentials, or TheGlowingStripes wifi)
    if (wifi_from_credentials_is_online()){
        if (connect_to_wifi_from_credentials()==false){
            // if that failed, restart ESP and try again
            Serial.println("Failed to connect to wifi from credentials. Restarting...");
            ESP.restart();
        }
    } else {
        // else try connecting to TheGlowingStripes network
        if (default_host_wifi_is_online()){
            if (connect_to_default_host_wifi()==false){
                // if that failed, restart ESP and try again
                Serial.println("Failed to connect to default host wifi. Restarting...");
                ESP.restart();
            }
        }
    }
    
    // Info: as long as "start_wifi" is executed before leds start - this should cause LED flickering
    // Only when OTA is used to update LED strip code, this will cause flickering
}