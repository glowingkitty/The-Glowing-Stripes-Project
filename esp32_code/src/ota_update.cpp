#include <ArduinoOTA.h>
#include "strip_config.h"
#include <SPIFFS.h>

void start_ota(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_ota()");
    Serial.println("");
    // if this is the host, set hostname to "theglowingstripes"
    if (SPIFFS.exists("/host.txt")){
      ArduinoOTA.setHostname("theglowingstripes");
      Serial.println("Hostname set: theglowingstripes.local");
    } else {
      // get host name from saved config json
      StaticJsonDocument<850> led_strip_info = load_strip_config();
      String hostname = "led_strip__"+led_strip_info["0"].as<String>();
      ArduinoOTA.setHostname(hostname.c_str());
      Serial.println("Hostname set: "+hostname+".local");
    }
    
    ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });

    ArduinoOTA.begin();
}

void handle_ota(){
    ArduinoOTA.handle();
}