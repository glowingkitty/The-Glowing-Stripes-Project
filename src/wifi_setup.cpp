#include <WiFi.h>
#include <string>
#include <iostream>
using namespace std;

const char*Wifi_ssid = "maya";                             // SSID of your Router OR mobile hotspot
const char*Wifi_password = "JODA522242200";                       //  PASSWORD of your Router or Mobile hotspot see below example

const char *Apssid = "TheGlowingStripes";                      //give Accesspoint SSID, your esp's hotspot name 
const char *Appassword = "letsglow";       


void start_wifi(){
    delay(500);
    WiFi.mode(WIFI_AP_STA);           // changing ESP9266 wifi mode to AP + STATION

    WiFi.softAP(Apssid, Appassword);         //Starting AccessPoint on given credential
    IPAddress myIP = WiFi.softAPIP();        //IP Address of our Esp32 accesspoint(where we can host webpages, and see data)
    Serial.print("Access Point IP address: ");
    Serial.println(myIP);
    
    Serial.println("");

    delay(1500);
    Serial.println("connecting to Wifi:");
    Serial.println(Wifi_ssid);

    WiFi.begin(Wifi_ssid, Wifi_password);                  // to tell Esp32 Where to connect and trying to connect
    while (WiFi.status() != WL_CONNECTED) {                // While loop for checking Internet Connected or not
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
}