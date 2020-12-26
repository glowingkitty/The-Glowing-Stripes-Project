#include <Arduino.h>

#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <vector>
#include <algorithm>
using namespace std;
#include "ArduinoJson.h"

AsyncWebServer server(80);

DynamicJsonDocument connected_led_strips(1024);
JsonArray led_strips = connected_led_strips["led_strips"].to<JsonArray>();

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void start_server(){
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");;

    server.on("/signup_led_strip", HTTP_POST, [](AsyncWebServerRequest *request){
        DynamicJsonDocument led_strip(1024);

        if(request->hasParam("body", true)){
            AsyncWebParameter* p = request->getParam("body", true);
            // DynamicJsonBuffer buffer;
            // deserializeJson(led_strip, buffer.parseObject(p->value()));
            deserializeJson(led_strip, p->value());
            
            if (led_strip["ip_address"]){
                led_strips.add(led_strip);

                request->send(200, "application/json", "{\"success\":true}");
            }
        }
    });

    server.on("/connected_led_strips", HTTP_GET, [] (AsyncWebServerRequest *request) {
        request->send(200, "application/json", connected_led_strips.as<String>());
    });

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

    server.onNotFound(notFound);

    server.begin();
}