// #include <string>
// #include <iostream>
// using namespace std;
// #include <SPI.h>
// #include <SD.h>
// #include "wifi_setup.h"
// #include "webserver.h"
// #include "neopixel.hpp"
// #include "Arduino.h"
// #include "SPIFFS.h"
// #include <WiFi.h>
// #include <ESPmDNS.h>
// #include <WiFiUdp.h>
// #include <ArduinoOTA.h>

// #ifdef __AVR__
//   #include <avr/power.h>
// #endif

// #ifdef __cplusplus
// extern "C" {
// #endif

// uint8_t temprature_sens_read();

// #ifdef __cplusplus
// }
// #endif

// uint8_t temprature_sens_read();

// NeoPixel leds;

// int response_code;

// const char* ssid = "TheGlowingStripes";
// const char* password = "letsglow";

// void connect_to_host(){
//   Serial.println("Connecting to host...");
//   WiFi.begin(ssid,password);  
//   int waited = 0;              
//   while (WiFi.status() != WL_CONNECTED && waited<20) {
//     Serial.println("Coudnt connect to host. Waiting...");
//     waited+=1;
//     delay(100);
//   }
//   if (WiFi.status() != WL_CONNECTED && waited==20){
//     Serial.println("Coudnt connect to host. Restarting...");
//     ESP.restart();
//   }
// }

// void reconnect_to_new_host(WiFiEvent_t event, WiFiEventInfo_t info){
//     Serial.println("Host is offline. Reconnecting to new host...");
//     connect_to_host();
// }

// void setup() {
//     #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
//       clock_prescale_set(clock_div_1);
//     #endif

//     Serial.begin(115200);               // to enable Serial Commmunication with connected Esp32 board
    
//     if (!SPIFFS.begin(true)) {
//         Serial.println("An Error has occurred while mounting SPIFFS");
//         return;
//     }
    
//     start_wifi();
//     start_server();

//     ArduinoOTA
//       .onStart([]() {
//         String type;
//         if (ArduinoOTA.getCommand() == U_FLASH)
//           type = "sketch";
//         else // U_SPIFFS
//           type = "filesystem";

//         // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
//         Serial.println("Start updating " + type);
//       })
//       .onEnd([]() {
//         Serial.println("\nEnd");
//       })
//       .onProgress([](unsigned int progress, unsigned int total) {
//         Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//       })
//       .onError([](ota_error_t error) {
//         Serial.printf("Error[%u]: ", error);
//         if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
//         else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
//         else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
//         else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
//         else if (error == OTA_END_ERROR) Serial.println("End Failed");
//       });

//     ArduinoOTA.begin();
// }

// void loop() {
//     // ArduinoOTA.handle();
//     Serial.print("ESP32 Temperature: ");
  
//     // Give out current temperature in degrees celcius
//     Serial.print((temprature_sens_read() - 32) / 1.8);
//     Serial.println(" C");


//     // if this webserver is not the host currently, check every few seconds if host is still online - else make this the host
//     if (get_role()=="backup_server"){
//         if (!host_is_online()){
//             Serial.print("Host went offline. Becoming new host by starting hotspot...");
//             start_hotspot();
//         }
//     }
    
//     delay(1000);
//     // leds.glow();
// }