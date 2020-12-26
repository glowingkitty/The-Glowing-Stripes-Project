#include <Arduino.h>

#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
#include "ArduinoJson.h"
#include <WiFi.h>
#include "esp_wifi.h"

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void start_server(){
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");;

    server.on("/connected_led_strips", HTTP_GET, [] (AsyncWebServerRequest *request) {
        // get the IP addresses of all LED strips and return them to frontend - so frontend can make /change_mode requests to led strips for changing to setup mode
        // and get details like length of led strip, name and last animation in return - to generate preview of led strips
        DynamicJsonDocument connected_led_strips(1024);
        JsonArray led_strips = connected_led_strips["led_strips"].to<JsonArray>();

        wifi_sta_list_t wifi_sta_list;
        tcpip_adapter_sta_list_t adapter_sta_list;
        
        memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
        memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
        
        esp_wifi_ap_get_sta_list(&wifi_sta_list);
        tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
        
        for (int i = 0; i < adapter_sta_list.num; i++) {
            tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
            led_strips.add(String(ip4addr_ntoa(&(station.ip))));    
        }

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