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
#include "credentials.h"
#include <string>
#include <stdio.h>
#include "strip_config.h"
using namespace std;

const char* glowing_stripes_ssid = "TheGlowingStripes";
const char* glowing_stripes_password = "letsglow";
string role;

AsyncWebServer server(80);

//// stripe_config.json fields - around 70bytes when fields are filled:
// 0:id
// 1:name
// 2:num_of_leds
// 3:num_of_sections
// 4:last_animation_id
// 5:ip_address

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

DynamicJsonDocument connected_led_strips(2048); // equals about 30 LED strips (70bytes per LED strip)
JsonArray led_strips = connected_led_strips["online"].to<JsonArray>();

/////////////////////////////
/// Wifi
/////////////////////////////

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

    // TODO if credentials for a wifi exist, connect to wifi, else connect to TheGlowingStripes hotspot
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
    StaticJsonDocument<140> led_strip_info = load_strip_config();
    String name = led_strip_info["1"];
    Serial.println(name);
    
    led_strip_info["5"] = String(WiFi.localIP());

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