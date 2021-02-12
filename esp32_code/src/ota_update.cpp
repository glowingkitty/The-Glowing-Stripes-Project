#include <ArduinoOTA.h>
#include "strip_config.h"
#include "webserver.h"
#include <SPIFFS.h>

String type;

String backup_animation_id;
StaticJsonDocument<500> backup_customizations;

void backup_config(){
  StaticJsonDocument<850> led_strip_info = load_strip_config();
  backup_animation_id = led_strip_info["4"].as<String>();
  backup_customizations = led_strip_info["5"];
}

void restore_backup(){
  StaticJsonDocument<850> led_strip_info;
  File led_strip_info_file = SPIFFS.open("/stripe_config.json");
  if(!led_strip_info_file){
      Serial.println("Failed to open stripe_config for reading");
      led_strip_info_file.close();
  }else {
      DeserializationError error = deserializeJson(led_strip_info, led_strip_info_file);
      if (error){
          Serial.println("Failed to read led_strip_info_file.");
      } else {
          Serial.println("Loaded stripe_config.json");
          led_strip_info_file.close();
          led_strip_info["4"] = backup_animation_id;
          led_strip_info["5"] = backup_customizations;

          SPIFFS.remove("/stripe_config.json");

          // Open file for writing
          File file = SPIFFS.open("/stripe_config.json", FILE_WRITE);
          if (!file) {
              Serial.println("Failed to create stripe_config.json");
          }
          // Serialize JSON to file
          if (serializeJson(led_strip_info, file) == 0) {
              Serial.println(F("Failed to write to stripe_config.json"));
          }
          // Close the file
          file.close();
      }
  }
}

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
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
        // backup current animation mode
        // backup_config();
        
        // send new mode: "process_update" with "progress=0" and "type=X"
        StaticJsonDocument<500> customizations;
        customizations["o"]=type;
        customizations["p"]=0;
        // TODO make showing update progress work
        // update_animation("upd",customizations);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
        // restore previous animation
        // restore_backup();
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        // start LED update animation 
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        // send new mode: "process_update" with "progress=X" and "type=X"
        StaticJsonDocument<500> customizations;
        customizations["o"]=type;
        customizations["p"]=((progress / (total / 100))/100);
        Serial.println(customizations["p"].as<float>());
        // TODO make showing update progress work
        // update_animation("upd",customizations);
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        // send new mode: "update_failed"
        StaticJsonDocument<500> customizations;
        // TODO make showing error light up work
        // update_animation("err",customizations);

        delay(2000);

        // restore previous animation
        // restore_backup();
      });

    ArduinoOTA.begin();
}

void handle_ota(){
    ArduinoOTA.handle();
}