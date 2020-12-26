#include <string>
#include <iostream>
using namespace std;
#include <SPI.h>
#include <SD.h>
#include "wifi_setup.h"
#include "webserver.h"
#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"

 #ifdef __cplusplus
  extern "C" {
 #endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

void setup() {
    Serial.begin(115200);               // to enable Serial Commmunication with connected Esp32 board
    
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    
    start_wifi();
    start_server();
}

void loop() {
    Serial.print("ESP32 Temperature: ");
  
    // Give out current temperature in degrees celcius
    Serial.print((temprature_sens_read() - 32) / 1.8);
    Serial.println(" C");


    // if this webserver is not the host currently, check every few seconds if host is still online - else make this the host
    if (get_role()=="backup_server"){
        if (!host_is_online()){
            Serial.print("Host went offline. Becoming new host by starting hotspot...");
            start_hotspot();
        }
    }

    delay(1000);
}