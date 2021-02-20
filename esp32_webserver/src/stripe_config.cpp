#include <Arduino.h>
#include <SPIFFS.h>
#include "ArduinoJson.h"
#include <string>
using namespace std;

string gen_random() {
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || gen_random()");
    Serial.println("");
    string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";
    
    srand( (unsigned) time(NULL) * getpid());

    tmp_s.reserve(3);

    for (int i = 0; i < 3; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
    
}

// load strip config: for led animation (num of leds, last animation, num of sections) & webserver (all details)
StaticJsonDocument<850> load_strip_config(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || load_strip_config()");
    Serial.println("");
    StaticJsonDocument<850> led_strip_config;
    File led_strip_config_file = SPIFFS.open("/stripe_config.json");
    if(!led_strip_config_file){
        Serial.println("Failed to open led_strip_config for reading");
    }else {
        // load strip config
        
        DeserializationError error = deserializeJson(led_strip_config, led_strip_config_file);
        if (error){
            Serial.print(F("Failed to read stripe_config.json: "));
            Serial.println(error.c_str());
        } else {
            Serial.println("Loaded stripe_config.json");
        }
    }
    led_strip_config_file.close();
    

    bool update_led_strip_config {false};
    // generate values if they don't exist yet
    if (!led_strip_config["0"]){
        Serial.println("led_strip_config.id is null, generating random id...");
        // generate random id
        led_strip_config["0"] = gen_random().c_str();
        update_led_strip_config = true;
    }
    if (!led_strip_config["1"]){
        Serial.println("led_strip_config.name is null, generating name...");
        led_strip_config["1"] = "LED strip";
        update_led_strip_config = true;
    }
    if (!led_strip_config["4"]){
        Serial.println("led_strip_config.last_animation_id is null, generating last_animation_..");

        // read led_animations.json
        File led_animations_file = SPIFFS.open("/led_animations.json");
        
        if(!led_animations_file){
            Serial.println("Failed to open led_strip_config for reading");
        }
        
        DynamicJsonDocument led_animations(2048);
        DeserializationError error = deserializeJson(led_animations, led_animations_file);
        if (error){
            Serial.print(F("Failed to read led_animations.json: "));
            Serial.println(error.c_str());
        } else {
            Serial.println("Loaded led_animations.json");
        }

        led_animations_file.close();

        // TODO define defaul animation - based on led_animations.json
        // led_strip_config["last_animation"]["id"] = led_animations["default_animation"]["id"];
        // led_strip_config["last_animation"]["name"] = led_animations["default_animation"]["name"];
        // led_strip_config["last_animation"]["customization"] = led_animations["default_animation"]["customization"];
        update_led_strip_config = true;
    }

    if (update_led_strip_config){
        Serial.println("Update stripe_config.json...");
        // write updated file
        SPIFFS.remove("/stripe_config.json");

        // Open file for writing
        File file = SPIFFS.open("/stripe_config.json", FILE_WRITE);
        if (!file) {
            Serial.println("Failed to create file");
        }
        // Serialize JSON to file
        if (serializeJson(led_strip_config, file) == 0) {
            Serial.println(F("Failed to write to file"));
        }
        // Close the file
        file.close();
        Serial.println("Updated stripe_config.json");
    }

    return led_strip_config;
}

