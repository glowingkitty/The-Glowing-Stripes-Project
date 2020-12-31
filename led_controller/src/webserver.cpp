#include <Arduino.h>

#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include <string>
using namespace std;

AsyncWebServer server(80);

string gen_random(const int len) {
    string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    srand( (unsigned) time(NULL) * getpid());

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
    
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void start_server(){
    Serial.println("Starting server...");
    server.serveStatic("/", SPIFFS, "/");

    server.on("/mode", HTTP_POST, [](AsyncWebServerRequest *request){
        Serial.println("Process POST /mode request...");
        DynamicJsonDocument new_mode(1024);

        if(request->hasParam("body", true)){
            AsyncWebParameter* p = request->getParam("body", true);
            // DynamicJsonBuffer buffer;
            // deserializeJson(led_strip, buffer.parseObject(p->value()));
            deserializeJson(new_mode, p->value());
            request->send(200, "application/json", "{\"success\":true}");
        }
    });

    server.on("/led_strip_info", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Process GET /led_strip_info request...");
        // get led stripe info from data/stripe_config.json & generate default values (id, name, animation) if they are set to none

        // load stripe_config.json
        StaticJsonDocument<512> led_strip_info;
        File led_strip_info_file = SPIFFS.open("/stripe_config.json");
        if(!led_strip_info_file){
            Serial.println("Failed to open led_strip_info for reading");
        }
        DeserializationError error = deserializeJson(led_strip_info, led_strip_info_file);
        led_strip_info_file.close();
        if (error){
            Serial.println("Failed to read led_strip_info_file, using default configuration");
        }


        bool update_led_strip_info {false};
        // generate values if they don't exist yet
        if (!led_strip_info["name"]){
            Serial.println("led_strip_info.name is null, generating name...");
            led_strip_info["name"] = "LED strip";
            update_led_strip_info = true;
        }
        if (!led_strip_info["id"]){
            Serial.println("led_strip_info.id is null, generating random id...");
            // generate random id
            led_strip_info["id"] = gen_random(10);
            update_led_strip_info = true;
        }
        if (!led_strip_info["last_animation"]["id"]){
            Serial.println("led_strip_info.last_animation is null, generating last_animation...");

            // read led_animations.json
            StaticJsonDocument<3072> led_animations;
            File led_animations_file = SPIFFS.open("/led_animations.json");
            
            if(!led_animations_file){
                Serial.println("Failed to open led_strip_info for reading");
            }
            DeserializationError error = deserializeJson(led_animations, led_animations_file);
            led_animations_file.close();
            if (error){
                Serial.println("Failed to read led_animations_file, using default configuration");
            }

            // define defaul animation - based on led_animations.json
            led_strip_info["last_animation"]["id"] = led_animations["default_animation"]["id"];
            led_strip_info["last_animation"]["name"] = led_animations["default_animation"]["name"];
            led_strip_info["last_animation"]["customization"] = led_animations["default_animation"]["customization"];
            update_led_strip_info = true;
        }

        if (update_led_strip_info){
            // write updated file
            SPIFFS.remove("/stripe_config.json");

            // Open file for writing
            File file = SPIFFS.open("/stripe_config.json", FILE_WRITE);
            if (!file) {
                Serial.println("Failed to create file");
                return;
            }
            // Serialize JSON to file
            if (serializeJson(led_strip_info, file) == 0) {
                Serial.println(F("Failed to write to file"));
            }
            // Close the file
            file.close();
        }
        
        request->send(200, "application/json", led_strip_info.as<String>());
    });

    server.onNotFound(notFound);

    server.begin();
    Serial.println("Server is online!");
}