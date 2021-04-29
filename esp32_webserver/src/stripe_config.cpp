#include <Arduino.h>
#include <SPIFFS.h>
#include "ArduinoJson.h"
#include "serial.h"
#include <string>
#include <HTTPClient.h>
using namespace std;


//// stripe_config.json fields - around 70bytes when fields are filled:
// p:data_pin
// "0":id <String>
// "1":name <String>
// "2":num_of_leds <int>
// "3":num_of_sections <int>
// 4:current_animation <JsonObject>
//// a:id <String>
//// b:name <String>
//// c:based_on_animation_id <String>
//// d:customization <JsonObject>
// 5:previous_animation
//// a:id <String>
//// b:name <String>
//// c:based_on_animation_id <String>
//// d:customization <JsonObject>
// 6:ip_address
// s:setup_complete
// u:start_firmware_update

//// led_animations.json fields - around 930bytes with default content:
// 0:id
// 1:name
// 2:neopixel_plus_function
// 3:customization
//// customization settings:
// a:colors_selected
// b:rgb_colors
// c:num_random_colors
// d:brightness
// e:timing_selected
// f:duration_ms
// g:pause_a_ms
// h:pause_b_ms
// i:sections_selected
// j:sections
// k:start
// l:possible_directions
// m:brightness_fixed
// n:max_height
// o:update_type
// p:update_progress
// 4:based_on_animation_id (for custom animations)

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


bool update_stripe_config(StaticJsonDocument<850> new_config){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || update_stripe_config()");
    Serial.println("");

    SPIFFS.remove("/stripe_config.json");

    // Open file for writing
    File file = SPIFFS.open("/stripe_config.json", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to create stripe_config.json");
        return false;
    }
    // Serialize JSON to file
    if (serializeJson(new_config, file) == 0) {
        Serial.println(F("Failed to write to stripe_config.json"));
        return false;
    }
    // Close the file
    file.close();

    new_config.clear();

    Serial.println("Updated stripe_config.json");
    return true;
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
    

    bool update_config {false};
    // generate values if they don't exist yet
    if (!led_strip_config["0"]){
        Serial.println("led_strip_config.id is null, generating random id...");
        // generate random id
        led_strip_config["0"] = gen_random().c_str();
        update_config = true;
    }
    if (!led_strip_config["1"]){
        Serial.println("led_strip_config.name is null, generating name...");
        led_strip_config["1"] = "LED strip";
        update_config = true;
    }
    if (!led_strip_config["4"] || !led_strip_config["4"]["a"]){
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
        update_config = true;
    }

    // If currently in setup mode while booting, restore previous animation instead
    if (led_strip_config["4"]["a"]=="set"){
        led_strip_config["4"] = led_strip_config["5"];
        update_config = true;
    }

    if (update_config){
        Serial.println("Update stripe_config.json...");
        update_stripe_config(led_strip_config);
    }

    return led_strip_config;
}

StaticJsonDocument<850> update_this_led_strip(StaticJsonDocument<800> new_animation){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || update_this_led_strip()");
    Serial.println("");

    // Open file for writing
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
            // backup existing animation
            led_strip_info["5"] = led_strip_info["4"];
            // set new animation
            led_strip_info["4"] = new_animation;

            new_animation.clear();

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

            // Submit new stripe_config.json via serial
            String serialized_json;
            serializeJson(led_strip_info, serialized_json);
            send_to_led_esp(serialized_json);
            
            Serial.println("Sent new stripe_config.json via Serial to other ESP:");
            Serial.println(serialized_json);
        }
    }

    return led_strip_info;
}

StaticJsonDocument<850> update_other_led_strip(
    StaticJsonDocument<800> new_animation,
    StaticJsonDocument<850> current_led_strip_config
    ){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || update_other_led_strip()");
    Serial.println("");

    String led_strip_id = current_led_strip_config["0"].as<String>();
    String led_strip_ip_address = current_led_strip_config["8"].as<String>();

    Serial.println("Updating 'current_animation' of "+led_strip_id+" ...");
    HTTPClient http;
    http.begin("http://"+led_strip_ip_address+"/mode");
    http.addHeader("Content-Type", "application/json");
    
    String json_text;
    serializeJson(current_led_strip_config, json_text);
    int httpResponseCode = http.POST(json_text);
    if(httpResponseCode==200){
        // backup current animation
        current_led_strip_config["5"] = current_led_strip_config["4"];

        // update to new animation
        current_led_strip_config["4"] = new_animation;

        Serial.println("Updated LED strip "+led_strip_id+" via POST request.");
    } else {
        Serial.println("Failed to update LED strip "+led_strip_id+" via POST request.");
        Serial.println("httpResponseCode:");
        Serial.println(httpResponseCode);
        Serial.println("response:");
        String response = http.getString();
        Serial.println(response);
    }
    http.end();

    return current_led_strip_config;
}