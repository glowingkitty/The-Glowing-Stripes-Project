#include <WiFi.h>
#include <ESPmDNS.h>
#include <SPI.h>
#include <SD.h>
#include <string>
using namespace std;

const char* wifi_ssid = "TheGlowingStripes";
const char* wifi_password = "letsglow";
const char* hotspot_ssid = "TheGlowingStripes";
const char* hotspot_password = "letsglow";
string role;

void signup_new_led_strip(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Station connected");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

void start_hotspot(){
    Serial.print("Starting hotspot...");
    WiFi.mode(WIFI_AP);           // changing ESP9266 wifi mode to AP + STATION
    WiFi.softAP(hotspot_ssid, hotspot_password);         //Starting AccessPoint on given credential

    // make ESP accessible via "theglowingstripes.local"
    if(!MDNS.begin("theglowingstripes")) {
        Serial.println("Error starting mDNS");
        return;
    }
    role = "host";

    WiFi.onEvent(signup_new_led_strip, SYSTEM_EVENT_AP_STAIPASSIGNED);
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
            return true;
          }
      }
  }
  WiFi.scanDelete();
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
    // see if TheGlowingStripes wifi already exists (if a host is already active nearby)
    if (host_is_online()){
      // if true, become a backup server - to enable easy switching between hosts
      role = "backup_server";
    } else {
      // else become the host by starting the hotspot
      start_hotspot();
    }
}

