#include <WiFi.h>
#include <ESPmDNS.h>
#include "wifi_setup.hpp"  
#include <SPI.h>
#include <SD.h>
#include <string>

WifiSetup::WifiSetup()
{
    wifi_ssid = "TheGlowingStripes";
    wifi_password = "letsglow";
    hotspot_ssid = "TheGlowingStripes";
    hotspot_password = "letsglow";
}

WifiSetup::~WifiSetup()
{

}

void WifiSetup::start_hotspot(){
  Serial.print("Starting hotspot...");
    WiFi.mode(WIFI_AP);           // changing ESP9266 wifi mode to AP + STATION
    WiFi.softAP(hotspot_ssid.c_str(), hotspot_password.c_str());         //Starting AccessPoint on given credential

    // make ESP accessible via "theglowingstripes.local"
    if(!MDNS.begin("theglowingstripes")) {
        Serial.println("Error starting mDNS");
        return;
    }
    role = "host";
}

std::string WifiSetup::get_role(){
  return role;
}

boolean WifiSetup::host_is_online(){
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

boolean WifiSetup::connect_to_wifi(){
    Serial.println("Connect to wifi...");
    Serial.println(wifi_ssid.c_str());
    delay(500);

    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());                  // to tell Esp32 Where to connect and trying to connect

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

void WifiSetup::start_wifi(){
    // see if TheGlowingStripes wifi already exists (if a host is already active nearby)
    if (WifiSetup::host_is_online()){
      // if true, become a backup server - to enable easy switching between hosts
      role = "backup_server";
    } else {
      // else become the host by starting the hotspot
      WifiSetup::start_hotspot();
    }
}

