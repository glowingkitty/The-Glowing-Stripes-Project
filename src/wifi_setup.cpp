#include <WiFi.h>
#include <ESPmDNS.h>
#include "wifi_setup.hpp"  
#include <SPI.h>
#include <SD.h>

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
    WiFi.mode(WIFI_AP);           // changing ESP9266 wifi mode to AP + STATION
    WiFi.softAP(hotspot_ssid.c_str(), hotspot_password.c_str());         //Starting AccessPoint on given credential

    // make ESP accessible via "theglowingstripes.local"
    if(!MDNS.begin("theglowingstripes")) {
        Serial.println("Error starting mDNS");
        return;
    }

    IPAddress myIP = WiFi.softAPIP();        //IP Address of our Esp32 accesspoint(where we can host webpages, and see data)
    Serial.print("Access Point IP address: ");
    Serial.println(myIP);
    
    Serial.println("");
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
    if (!WifiSetup::connect_to_wifi()){
      WifiSetup::start_hotspot();
    }
}