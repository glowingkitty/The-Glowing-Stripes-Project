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

boolean wifi_is_nearby(String wifi_ssid, int number_of_nearby_wifis){
    for (int i = 0; i < number_of_nearby_wifis; ++i) {
        // check for every possible wifi in known_wifi_ssids
        if (WiFi.SSID(i)==wifi_ssid){
            Serial.println("Known wifi found: "+wifi_ssid);
            WiFi.scanDelete();
            return true;
        }
    }
    return false;
}

boolean connect_to_ssid(String ssid,String password){
    delay(2000);

    WiFi.begin(ssid.c_str(), password.c_str());
    delay(500);

    // if that fails, return false (causing a restart)
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to wifi '"+ssid+"'. Restarting...");
        ESP.restart();
    }
    Serial.println("");
    Serial.print("Connected LED strip to Wifi: ");
    Serial.print(ssid);
    Serial.println("");
    return true;
}

boolean connect_to_wifi(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || connect_to_wifi()");
    Serial.println("");

    // load wifi credentials, see if any wifi is online - if so, connect with it
    Serial.println("Check if any known wifi is online...");
    
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("no networks found. connect_to_wifi() failed.");
        WiFi.scanDelete();
        return false;
    } else {
        // try to connect to one of the known wifis in wifi_credentials.json
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

                    wifi_credentials_file.close();

                    // check if wifi_credentials is a single wifi network
                    if (wifi_credentials.containsKey("0") && wifi_credentials.containsKey("1")){
                        // check if wifi is online
                        if (wifi_is_nearby(wifi_credentials["0"],n)){
                            // if wifi is online, connect to it
                            return connect_to_ssid(wifi_credentials["0"].as<String>(),wifi_credentials["1"].as<String>());
                        }
                    }

                    // else check for every single wifi in wifi_credentials, if its online & connect to it
                    for (int i = 0; i < wifi_credentials.size();i++){
                        if (wifi_is_nearby(wifi_credentials[i]["0"],n)){
                            // if wifi is online, connect to it
                            return connect_to_ssid(wifi_credentials[i]["0"].as<String>(),wifi_credentials[i]["1"].as<String>());
                        }
                    }
                }
            }
        }

        // else check if default glowing_stripes_ssid is online
        if (wifi_is_nearby(glowing_stripes_ssid,n)){
            // if wifi is online, connect to it
            return connect_to_ssid(glowing_stripes_ssid,glowing_stripes_password);
        } else {
            Serial.println("No known wifi is nearby.");
            return false;
        }
    }
}

void start_wifi(){
    // if wifi from wifi credentials is available, connect to it
    // else connect to glow.local network
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_wifi()");
    Serial.println("");

    if (!connect_to_wifi()){
        Serial.println("Failed to connect to any wifi. Restarting...");
        ESP.restart();
    }
    
    // Info: as long as "start_wifi" is executed before leds start - this should cause LED flickering
    // Only when OTA is used to update LED strip code, this will cause flickering
}