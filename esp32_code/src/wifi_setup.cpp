#include <WiFi.h>
#include "esp_wifi.h"
#include <ESPmDNS.h>
#include <SPI.h>
#include <SD.h>
#include <string>
#include "SPIFFS.h"
#include <HTTPClient.h>
#include "ArduinoJson.h"
using namespace std;

const char* wifi_ssid = "TheGlowingStripes";
const char* wifi_password = "letsglow";
const char* hotspot_ssid = "TheGlowingStripes";
const char* hotspot_password = "letsglow";
string role;
string led_strip_info;

void signup_new_led_strip(WiFiEvent_t event, WiFiEventInfo_t info){
    HTTPClient http;
    Serial.println("New device is connected to the webserver!");
    // if a new device is connected, request from all led strips .../led_strip_info url, delete connected_led_strips.json and rebuild connected_led_strips.json
    DynamicJsonDocument connected_led_strips(2048);
    JsonArray led_strips = connected_led_strips["connected_led_strips"].to<JsonArray>();

    wifi_sta_list_t wifi_sta_list;
    tcpip_adapter_sta_list_t adapter_sta_list;
    
    memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
    memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
    
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
    
    for (int i = 0; i < adapter_sta_list.num; i++) {
        tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
        string ip_address = ip4addr_ntoa(&(station.ip));
        Serial.println(("Trying to access "+ip_address).c_str());

        // make request to led strip & if response successfull - add to led_strips
        // TODO seems to fail, the request - check serial output on led strip at the same time (connect via wifi to check both at the same time)
        http.begin(("http://"+ip_address+"/led_strip_info").c_str());

        int httpResponseCode = http.GET();
        
        String led_strip_info = "{}"; 
        
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          led_strip_info = http.getString();

          // parse json with ArdunoJson (need to change functions)
          StaticJsonDocument<512> led_strip_info_json;
          DeserializationError error = deserializeJson(led_strip_info_json, led_strip_info);

          // Test if parsing succeeds.
          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
          } else{
            led_strips.add(led_strip_info_json);
            Serial.println(("Collected LED strip info for "+ip_address).c_str());
          }
        }
        else {
          Serial.println("Didn't get a valid response from new device. Probably its not an LED strip");
        }
        // Free resources
        http.end();
    }

    // delete previous connected_led_strips.json
    if (SPIFFS.exists("/connected_led_strips.json")){
      SPIFFS.remove("/connected_led_strips.json");
    }

    // save new connected_led_strips.json
    File file = SPIFFS.open("/connected_led_strips.json", FILE_WRITE);
    if (!file) {
      Serial.println(F("Failed to create file"));
      return;
    }
    // Serialize JSON to file
    if (serializeJson(connected_led_strips, file) == 0) {
      Serial.println(F("Failed to write to file"));
    }
    // Close the file
    file.close();

    
}

void start_hotspot(){
    Serial.println("Starting hotspot...");
    // remove json with currently connected LED strips
    if (SPIFFS.exists("/connected_led_strips.json")){
      SPIFFS.remove("/connected_led_strips.json");
    }
    
    
    WiFi.mode(WIFI_AP);           // changing ESP9266 wifi mode to AP + STATION
    WiFi.softAP(hotspot_ssid, hotspot_password);         //Starting AccessPoint on given credential

    // make ESP accessible via "theglowingstripes.local"
    if(!MDNS.begin("theglowingstripes")) {
        Serial.println("Error starting mDNS");
        return;
    }
    role = "host";

    WiFi.onEvent(signup_new_led_strip, SYSTEM_EVENT_AP_STAIPASSIGNED);
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
      if (failed==2){
        return false;
      }
      delay(500);
      Serial.print(".");
    }

    

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());            // successful Connection of Esp32,
                                              // printing Local IP given by your Router or Mobile Hotspot,
                                            // Esp32 connect at this IP  see in advanced Ip scanner 
    Serial.println("");
    return true;
}

string get_role(){
  return role;
}

void start_wifi(){
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    // see if TheGlowingStripes wifi already exists (if a host is already active nearby)
    if (connect_to_wifi()){
      // if true, become a client (playing leds are get ready to take over host, if host goes offline)
      role = "client";
    } else {
      // else become the host by starting the hotspot
      start_hotspot();
    }
}

