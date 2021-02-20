#include <Arduino.h>

#include <vector>
#include <algorithm>
#include <WiFi.h>
#include "esp_wifi.h"
#include <ESPmDNS.h>
#include <SPI.h>
#include <SD.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include <string>
#include <stdio.h>
#include "strip_config.h"
#include "AsyncJson.h"
using namespace std;

const char* wifi_ssid;
const char* wifi_password;
const char* glowing_stripes_ssid = "TheGlowingStripes";
const char* glowing_stripes_password = "letsglow";
string role;

AsyncWebServer server(80);

//// stripe_config.json fields - around 70bytes when fields are filled:
// p:data_pin
// 0:id
// 1:name
// 2:num_of_leds
// 3:num_of_sections
// 4:last_animation_id
// 5:last_animation_customization
// 6:ip_address

//// led_animations.json fields - around 930bytes with default content:
// 0:id
// 1:name
// 2:neopixel_plus_function
// 3:customization
//// customization settings:
// a:colors_selected
// b:rgb_colors
// c:num_random_colors
// d:brightness
// e:timing_selected
// f:duration_ms
// g:pause_a_ms
// h:pause_b_ms
// i:sections_selected
// j:sections
// k:start
// l:possible_directions
// m:brightness_fixed
// n:max_height
// o:update_type
// p:update_progress

DynamicJsonDocument connected_led_strips(2048); // equals about 30 LED strips (70bytes per LED strip)
JsonArray led_strips = connected_led_strips["online"].to<JsonArray>();

/////////////////////////////
/// Wifi
/////////////////////////////

boolean host_wifi_is_online(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || host_wifi_is_online()");
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

boolean host_is_online(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || host_is_online()");
    Serial.println("");

    // check if "theglowingstripes.local" is accessible
    // search for theglowingstripes domain
    if(mdns_init()!= ESP_OK){
        Serial.println("mDNS failed to start");
        return false;
    }
    IPAddress serverIp = MDNS.queryHost("theglowingstripes");
    if (serverIp){
        return true;
    }
    return false;
}

// TODO test
void update_animation(String id,StaticJsonDocument<500> customizations){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || update_animation()");
    Serial.println("");

    // Open file for writing
    StaticJsonDocument<850> led_strip_info;
    File led_strip_info_file = SPIFFS.open("/stripe_config.json");
    if(!led_strip_info_file){
        Serial.println("Failed to open stripe_config for reading");
        led_strip_info_file.close();
    }else {
        DeserializationError error = deserializeJson(led_strip_info, led_strip_info_file);
        if (error){
            Serial.println("Failed to read led_strip_info_file.");
        } else {
            Serial.println("Loaded stripe_config.json");
            led_strip_info_file.close();
            led_strip_info["4"] = id;
            led_strip_info["5"] = customizations;

            SPIFFS.remove("/stripe_config.json");

            // Open file for writing
            File file = SPIFFS.open("/stripe_config.json", FILE_WRITE);
            if (!file) {
                Serial.println("Failed to create stripe_config.json");
            }
            // Serialize JSON to file
            if (serializeJson(led_strip_info, file) == 0) {
                Serial.println(F("Failed to write to stripe_config.json"));
            }
            // Close the file
            file.close();
        }
    }
    
}


void become_client(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || become_client()");
    Serial.println("");

    role = "client";
    // remove host.txt file, if it exists
    if (SPIFFS.exists("/host.txt")){
        SPIFFS.remove("/host.txt");
    }
}

void become_host(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || become_host()");
    Serial.println("");

    role = "host";
    // create "host.txt" file, so ota_update.cpp knows this device is the host now - and sets the hotname to "theglowingstripes"
    File file = SPIFFS.open("/host.txt", FILE_WRITE);
    if(!file){
        Serial.println("There was an error opening host.txt for writing");
        return;
    }
    if(file.print("1")) {
        Serial.println("host.txt was written");
    }else {
        Serial.println("host.txt write failed");
    }
}

void start_hotspot(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_hotspot()");
    Serial.println("");
    
    WiFi.mode(WIFI_AP);          
    WiFi.softAP(glowing_stripes_ssid, glowing_stripes_password);
    // don't set hostname here, since ota_update.cpp will overwrite hostname after this
    Serial.println("Hotspot active now!");
}

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


void signup_led_strip(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || signup_led_strip()");
    Serial.println("");
    // make POST request to webserver to submit information like ip address and details
    StaticJsonDocument<850> led_strip_info = load_strip_config();
    
    led_strip_info["6"] = WiFi.localIP().toString();
    Serial.println("ip_address:  "+ led_strip_info["6"].as<String>());
    

    // make post request
    if(role == "client" && WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
        HTTPClient http;   
        
        IPAddress serverIp = MDNS.queryHost("theglowingstripes");
        http.begin("http://"+serverIp.toString()+"/signup_led_strip");
        http.addHeader("Content-Type", "application/json");
        
        String json;
        serializeJson(led_strip_info, json);
        Serial.println(json);
        int httpResponseCode = http.POST(json);   //TODO Send the actual POST request
        if(httpResponseCode>0){
            String response = http.getString();                       //Get the response to the request
            Serial.println(httpResponseCode);   //Print return code
            Serial.println(response);           //Print request answer
        }else{
        
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        
        }
        http.end();  //Free resources
        return;

    } else if (role == "host"){
        // if led strip isn't connected to wifi - because its the host, add led strip directly to list of signed up led strips
        led_strips.add(led_strip_info);
        Serial.println("Signed up LED strip");
    } else {
        Serial.println("Error in signup_led_strip(), couldnt sign up LED strip.");
    }

}

void start_wifi(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_wifi()");
    Serial.println("");
    // see if host wifi is nearby (the one defined in credentials, or TheGlowingStripes wifi)
    if (host_wifi_is_online()){
        if (connect_to_wifi()){
            if (host_is_online()){
                // if true, become a client (playing leds are get ready to take over host, if host goes offline)
                become_client();
            }
            else {
                become_host();
            }
        } else {
            // if that failed, restart ESP and try again
            Serial.println("Failed to connect to host wifi. Restarting...");
            ESP.restart();
        }
    } else {
        // else become the host by starting the hotspot
        become_host();
        start_hotspot();
    }
    // sign up led strip to webserver
    signup_led_strip();
}



/////////////////////////////
/// Webserver
/////////////////////////////


void notFound(AsyncWebServerRequest *request) {
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || notFound()");
    Serial.println("");
    request->send(404, "application/json", "{\"error\":\"page not found\"}");
}

void start_server(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_server()");
    Serial.println("");

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.on("/online", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /online");
        Serial.println("");
        request->send(200, "application/json", "{\"online\":true}");
    });

    // TODO fix flickering which happens when server request is made
    server.on("/wifi_networks_nearby", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /wifi_networks_nearby");
        Serial.println("");
        String json = "{\"networks\":[";

        Serial.println("Scan for Wifi nerworks...");
        int n = WiFi.scanNetworks();
        if (n == 0) {
            Serial.println("no networks found");
        } else {
            Serial.print(n);
            Serial.println(" networks found");
            
            vector<String> processed_essids;
            for (int i = 0; i < n; ++i) {
                // Print SSID and RSSI for each network found
                if (find(processed_essids.begin(), processed_essids.end(), WiFi.SSID(i)) != processed_essids.end()){
                }else{
                    if(i) json += ",";
                    json += "{";
                    json += "\"signal_strength\":"+String(WiFi.RSSI(i));
                    json += ",\"essid\":\""+WiFi.SSID(i)+"\"";
                    json += ",\"encryption\":"+String(WiFi.encryptionType(i) == 0 ? "off" : "on");
                    json += ",\"current_wifi\":false";
                    json += "}";
                    processed_essids.push_back(WiFi.SSID(i));
                }
            }
        }
        WiFi.scanDelete();

        json += "]}";
        request->send(200, "application/json", json);
        json = String();
    });

    server.on("/connected_led_strips", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /connected_led_strips");
        Serial.println("");
        String output;
        serializeJson(led_strips, output);
        request->send(200, "application/json", "{\"connected_led_strips\":"+output+"}");
    });

    server.on("/mode", HTTP_POST, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /mode");
        Serial.println("");
        // get json data from post request and update led strip mode
        String message;
        if (request->hasParam("body", true)) {
            message = request->getParam("body", true)->value();
            StaticJsonDocument<700> new_led_mode;
            DeserializationError error = deserializeJson(new_led_mode, message);
            if (error){
                Serial.print(F("DeserializationError when reading body from /mode POST request"));
                Serial.println(error.c_str());
                request->send(500, "application/json", "{\"error\":\"DeserializationError when reading body from /mode POST request\"}");
            } else {
                // change mode based on POST request
                update_animation(new_led_mode["0"],new_led_mode["1"]);
                
                Serial.println("Changed LED strip mode for {} to {}");
                new_led_mode.clear();
                request->send(200, "application/json", "{\"success\":true}");
            }
        } else {
            Serial.println("Request has no 'body' parameter. Return 500 response.");
            request->send(500, "application/json", "{\"error\":\"Request has no 'body' parameter\"}");
        }
    });

    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/signup_led_strip", [](AsyncWebServerRequest *request, JsonVariant &json) {
        // TODO resulting json is a mess... 
        StaticJsonDocument<850> led_strip_config = json.as<JsonObject>();
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /signup_led_strip");
        Serial.println("");
        // get json data from post request and add led strip to "led_strips"
        led_strips.add(led_strip_config);
        Serial.println("Signed up LED strip");
        request->send(200, "application/json", "{\"success\":true}");
    });
    server.addHandler(handler);

    server.onNotFound(notFound);

    server.begin();
    Serial.println("Server is online!");
}