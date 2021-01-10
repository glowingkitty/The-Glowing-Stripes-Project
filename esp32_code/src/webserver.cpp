#include <Arduino.h>

#include <vector>
#include <algorithm>
#include <WiFi.h>
#include "esp_wifi.h"
#include <ESPmDNS.h>
#include <SPI.h>
#include <SD.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include "credentials.h"
#include <string>
using namespace std;

const char* glowing_stripes_ssid = "TheGlowingStripes";
const char* glowing_stripes_password = "letsglow";
string role;

AsyncWebServer server(80);

DynamicJsonDocument connected_led_strips(2048);
JsonArray led_strips = connected_led_strips["connected_led_strips"].to<JsonArray>();

/////////////////////////////
/// Wifi
/////////////////////////////

string gen_random(const int len) {
    string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    srand( (unsigned) time(NULL) * getpid());

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
    
}

void start_hotspot(){
    Serial.println("Starting hotspot...");
    
    WiFi.mode(WIFI_AP);          
    WiFi.softAP(glowing_stripes_ssid, glowing_stripes_password);

    // make ESP accessible via "theglowingstripes.local"
    if(!MDNS.begin("theglowingstripes")) {
        Serial.println("Error starting mDNS");
        return;
    }
    role = "host";
    Serial.println("Hotspot active now!");
}

boolean host_is_online(){
  // see if TheGlowinStripes in network nearby
  Serial.println("Check if TheGlowinStripes wifi is online...");
  int n = WiFi.scanNetworks();
  if (n == 0) {
      Serial.println("no networks found");
      WiFi.scanDelete();
      return false;
  } else {
      for (int i = 0; i < n; ++i) {
          // Print SSID and RSSI for each network found
          if (WiFi.SSID(i)=="TheGlowingStripes"){
            Serial.println("TheGlowingStripes wifi found!");
            return true;
          }
      }
  }
  WiFi.scanDelete();
  Serial.println("TheGlowingStripes wifi NOT found!");
  return false;
}

boolean connect_to_wifi(){
    Serial.println("Connect to wifi...");
    Serial.println(wifi_ssid);
    delay(500);

    WiFi.begin(wifi_ssid, wifi_password);                  // to tell Esp32 Where to connect and trying to connect

    // after 2 fails, create hotspot instead
    int failed = 0;
    while (WiFi.status() != WL_CONNECTED) {                // While loop for checking Internet Connected or not
      failed = failed+1;
      if (failed==5){
        return false;
      }
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected");
    return true;
}

void signup_led_strip(){
    // TODO make POST request to webserver to submit information like ip address and details
    // load stripe_config.json
    StaticJsonDocument<512> led_strip_info;
    File led_strip_info_file = SPIFFS.open("/stripe_config.json");
    if(!led_strip_info_file){
        Serial.println("Failed to open led_strip_info for reading");
    }
    DeserializationError error = deserializeJson(led_strip_info, led_strip_info_file);
    led_strip_info_file.close();
    if (error){
        Serial.println("Failed to read led_strip_info_file, using default configuration");
    }


    bool update_led_strip_info {false};
    // generate values if they don't exist yet
    if (!led_strip_info["name"]){
        Serial.println("led_strip_info.name is null, generating name...");
        led_strip_info["name"] = "LED strip";
        update_led_strip_info = true;
    }
    if (!led_strip_info["id"]){
        Serial.println("led_strip_info.id is null, generating random id...");
        // generate random id
        led_strip_info["id"] = gen_random(10);
        update_led_strip_info = true;
    }
    if (!led_strip_info["last_animation"]["id"]){
        Serial.println("led_strip_info.last_animation is null, generating last_animation...");

        // read led_animations.json
        StaticJsonDocument<3072> led_animations;
        File led_animations_file = SPIFFS.open("/led_animations.json");
        
        if(!led_animations_file){
            Serial.println("Failed to open led_strip_info for reading");
        }
        DeserializationError error = deserializeJson(led_animations, led_animations_file);
        led_animations_file.close();
        if (error){
            Serial.println("Failed to read led_animations_file, using default configuration");
        }

        // define defaul animation - based on led_animations.json
        led_strip_info["last_animation"]["id"] = led_animations["default_animation"]["id"];
        led_strip_info["last_animation"]["name"] = led_animations["default_animation"]["name"];
        led_strip_info["last_animation"]["customization"] = led_animations["default_animation"]["customization"];
        update_led_strip_info = true;
    }

    if (update_led_strip_info){
        // write updated file
        SPIFFS.remove("/stripe_config.json");

        // Open file for writing
        File file = SPIFFS.open("/stripe_config.json", FILE_WRITE);
        if (!file) {
            Serial.println("Failed to create file");
            return;
        }
        // Serialize JSON to file
        if (serializeJson(led_strip_info, file) == 0) {
            Serial.println(F("Failed to write to file"));
        }
        // Close the file
        file.close();
    }

    led_strip_info["ip_address"] = String(WiFi.localIP());

    // TODO make post request
    if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
        HTTPClient http;   
        
        http.begin("http://theglowingstripes.local/signup_led_strip");
        http.addHeader("Content-Type", "application/json");
        
        int httpResponseCode = http.POST("{}");   //TODO Send the actual POST request
        if(httpResponseCode>0){
            String response = http.getString();                       //Get the response to the request
            Serial.println(httpResponseCode);   //Print return code
            Serial.println(response);           //Print request answer
        }else{
        
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        
        }
        http.end();  //Free resources
    }else{
        Serial.println("Error in WiFi connection. Couldnt sign up LED strip.");   
    }

}

void start_wifi(){
    Serial.println(wifi_ssid);
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    // see if TheGlowingStripes wifi already exists (if a host is already active nearby)
    if (host_is_online() && connect_to_wifi()){
      // if true, become a client (playing leds are get ready to take over host, if host goes offline)
      role = "client";
    } else {
      // else become the host by starting the hotspot
      start_hotspot();
    }
    // sign up led strip to webserver
    signup_led_strip();
}



/////////////////////////////
/// Webserver
/////////////////////////////


void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void start_server(){
    Serial.println("Starting server...");
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.on("/wifi_networks_nearby", HTTP_GET, [](AsyncWebServerRequest *request){
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

    server.on("/mode", HTTP_POST, [](AsyncWebServerRequest *request){
        Serial.println("Process POST /mode request...");
        // TODO
        request->send(200, "application/json", "{\"success\":true}");
    });

    server.on("/signup_led_strip", HTTP_POST, [](AsyncWebServerRequest *request){
        Serial.println("Process POST /signup_led_strip request...");
        // TODO add led strip data to variable
        String message;
        if (request->hasParam("body", true)) {
            message = request->getParam("body", true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "application/json", "{\"success\":true}");
    });

    server.onNotFound(notFound);

    server.begin();
    Serial.println("Server is online!");
}