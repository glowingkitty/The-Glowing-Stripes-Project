#include <Arduino.h>
#include <SPIFFS.h>
#include "ArduinoJson.h"
#include <string>
using namespace std;

bool first_boot {true};

String gen_random() {
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || gen_random()");
    Serial.println("");
    String tmp_s;
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

    Serial.println("Updated stripe_config.json to:");
    Serial.println(new_config.as<String>());

    new_config.clear();
    return true;
}


bool update_stripe_config_based_on_string(String new_config_text){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || update_stripe_config_based_on_string()");
    Serial.println("");

    Serial.println("Deserialize new_config_text: "+new_config_text);
    StaticJsonDocument<850> led_strip_config;
    DeserializationError error = deserializeJson(led_strip_config, new_config_text);
    if (error){
        Serial.println("deserializeJson failed");
        led_strip_config.clear();
        return false;
    } else {
        update_stripe_config(led_strip_config);
        led_strip_config.clear();
        return true;
    }
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

    if (first_boot){
        // make sure software update mode isn't triggered on boot
        if (led_strip_config.containsKey("u")){
            led_strip_config.remove("u");
            update_config = true;
        }
        // If currently in setup mode while booting, restore previous animation instead
        if (led_strip_config["4"]["a"]=="set"){
            led_strip_config["4"] = led_strip_config["5"];
            update_config = true;
        }
        
        first_boot = false;
    }

    if (update_config){
        Serial.println("Update stripe_config.json...");
        update_stripe_config(led_strip_config);
    }
    

    return led_strip_config;
}


bool field_valid(
    StaticJsonDocument<850> led_strip_config,
    String fieldname,
    String type, 
    boolean is_required,
    int required_length=0
    ){

    if (is_required){
        // check if field is required and exists
        if (!led_strip_config.containsKey(fieldname)){
            Serial.println("Field not valid: '"+fieldname+"' doesnt exist!");
            return false;
        }
    }

    // if the field exists, check if its correct
    if (led_strip_config.containsKey(fieldname)){
        if (type=="<String>"){
            // check if field is String
            if (!led_strip_config[fieldname].is<String>()){
                Serial.println("Field not valid: '"+fieldname+"' is not <String>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }
            // check if field is the right length
            if (required_length>0 && (led_strip_config[fieldname].as<String>().length()!=required_length)){
                Serial.println("Field not valid: '"+fieldname+"' is not the right length! Length: "+led_strip_config[fieldname].as<String>().length());
                return false;
            }

        } else if (type=="<int>"){
            // check if field is int
            if (!led_strip_config[fieldname].is<int>()){
                Serial.println("Field not valid: '"+fieldname+"' is not <int>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }

        } else if (type=="<float>"){
            // check if field is float
            if (!led_strip_config[fieldname].is<float>()){
                Serial.println("Field not valid: '"+fieldname+"' is not <float>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }
        } else if (type=="<JsonObject>"){
            // check if field is JsonObject
            if (!led_strip_config[fieldname].is<JsonObject>()){
                Serial.println("Field not valid: '"+fieldname+"' is not <JsonObject>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }
        } else if (type=="<boolean>"){
            // check if field is boolean
            if (!led_strip_config[fieldname].is<boolean>()){
                Serial.println("Field not valid: '"+fieldname+"' is not <boolean>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }
        } else if (type=="<array<String>>"){
            // check if field is an array with String
            if (!led_strip_config[fieldname].is<JsonArray>() || !led_strip_config[fieldname][0].is<String>()){
                Serial.println("Field not valid: '"+fieldname+"' is not <array<String>>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }
        } else if (type=="<array<int>>"){
            // check if field is an array with int
            if (!led_strip_config[fieldname].is<JsonArray>() || !led_strip_config[fieldname][0].is<int>()){
                Serial.println("Field not valid: '"+fieldname+"' is not <array<int>>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }
        } else if (type=="<array<int>>|<array<array<int>>>"){
            // check if field is an array
            if (!led_strip_config[fieldname].is<JsonArray>()){
                Serial.println("Field not valid: '"+fieldname+"' is not <array...>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }
            // check if field is an array with int or an array with arrays with int
            if (!led_strip_config[fieldname][0].is<int>() && !(led_strip_config[fieldname][0].is<JsonArray>() && led_strip_config[fieldname][0][0].is<int>())){
                Serial.println("Field not valid: '"+fieldname+"' is not <array<int>> and not <array<array<int>>>! Content: "+led_strip_config[fieldname].as<String>());
                return false;
            }
        }

    }

    led_strip_config.clear();

    return true;
}

bool stripe_config_valid(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || stripe_config_valid()");
    Serial.println("");

    // load stripe config and check every field if its valid
    Serial.println("Loading stripe_config.json ...");
    StaticJsonDocument<850> led_strip_config;
    File led_strip_config_file = SPIFFS.open("/stripe_config.json");
    if(!led_strip_config_file){
        Serial.println("Failed to open led_strip_config for reading");
        led_strip_config_file.close();
        return false;
    }else {
        DeserializationError error = deserializeJson(led_strip_config, led_strip_config_file);
        if (error){
            Serial.println("Failed to read stripe_config.json: ");
            Serial.println(error.c_str());
            led_strip_config_file.close();
            return false;
        } else {
            Serial.println("Loaded stripe_config.json");
            led_strip_config_file.close();
        }
    }
    

    Serial.println("Checking if stripe_config.json is valid ...");
    // check "p" (data_pin):                   if it exists, make sure its <int>
    if (!field_valid(led_strip_config,"p","<int>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "0" (id):                         must exist, make sure its <String> and 3 characters long
    if (!field_valid(led_strip_config,"0","<String>",true,3)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "1" (name):                       must exist, make sure its <String>
    if (!field_valid(led_strip_config,"1","<String>",true)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "2" (num_of_leds):                must exist, make sure its <int>
    if (!field_valid(led_strip_config,"2","<int>",true)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "3" (num_of_sections):            if it exists, make sure its <int>
    if (!field_valid(led_strip_config,"3","<int>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4" (current_animation):          must exist, make sure its <JsonObject>
    if (!field_valid(led_strip_config,"4","<JsonObject>",true)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""a" (id):                      must exist, make sure its <String> and 3 characters long
    if (!field_valid(led_strip_config["4"],"a","<String>",true,3)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""b" (name):                    must exist, make sure its <String>
    if (!field_valid(led_strip_config["4"],"b","<String>",true)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""c" (based_on_id):             if it exists, make sure its <String>
    if (!field_valid(led_strip_config["4"],"c","<String>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d" (customization):           if it exists, make sure its <JsonObject>
    if (!field_valid(led_strip_config["4"],"d","<JsonObject>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""a" (colors_selected):      if it exists, make sure its <String>
    if (!field_valid(led_strip_config["4"]["d"],"a","<String>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""b" (rgb_colors):           if it exists, make sure its <array<int>> or <array<array<int>>>
    if (!field_valid(led_strip_config["4"]["d"],"b","<array<int>>|<array<array<int>>>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""c" (num_random_colors):    if it exists, make sure its <int>
    if (!field_valid(led_strip_config["4"]["d"],"c","<int>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""d" (brightness):           if it exists, make sure its <float>
    if (!field_valid(led_strip_config["4"]["d"],"d","<float>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""f" (duration_ms):          if it exists, make sure its <int>
    if (!field_valid(led_strip_config["4"]["d"],"f","<int>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""g" (pause_a_ms):           if it exists, make sure its <int>
    if (!field_valid(led_strip_config["4"]["d"],"g","<int>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""h" (pause_b_ms):           if it exists, make sure its <int>
    if (!field_valid(led_strip_config["4"]["d"],"h","<int>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""i" (sections_selected):    if it exists, make sure its <String>
    if (!field_valid(led_strip_config["4"]["d"],"i","<String>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""j" (sections):             if it exists, make sure its <array<int>>
    if (!field_valid(led_strip_config["4"]["d"],"j","<array<int>>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""k" (start):                if it exists, make sure its <String>
    if (!field_valid(led_strip_config["4"]["d"],"k","<String>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""l" (possible_directions):  if it exists, make sure its <array<String>>
    if (!field_valid(led_strip_config["4"]["d"],"l","<array<String>>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""m" (brightness_fixed):     if it exists, make sure its <boolean>
    if (!field_valid(led_strip_config["4"]["d"],"m","<boolean>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "4""d""n" (max_height):           if it exists, make sure its <float>
    if (!field_valid(led_strip_config["4"]["d"],"n","<float>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }
    // check "5" (previous_animation):         if it exists, make sure its <JsonObject> (further checking not needed, since json needs to be valid to be backup anyway)
    if (!field_valid(led_strip_config,"5","<JsonObject>",false)){
        Serial.println("stripe_config.json:");
        Serial.println(led_strip_config.as<String>());
        return false;
    }

    if (led_strip_config["4"]["a"]=="set" && led_strip_config["5"]["a"]=="set"){
        Serial.println("Setup mode is saved as both current and previous animation...this shouldnt happen! Restoring backup...");
        return false;
    }

    // If currently in setup mode while booting, restore previous animation instead
    if (led_strip_config["4"]["a"]=="set"){
        led_strip_config["4"] = led_strip_config["5"];
        Serial.println("Restore LED strip from setup mode to previous animation...");
        update_stripe_config(led_strip_config);
        led_strip_config.clear();
        Serial.println("Restored previous animation!");
        Serial.println("Rebooting...");
        ESP.restart();
    }

    return true;
}

void restore_default_config(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || restore_default_config()");
    Serial.println("");

    File file = SPIFFS.open("/stripe_config.json", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to create stripe_config.json");
    }
    
    StaticJsonDocument<850> default_config;
    default_config["p"] = 22;
    default_config["0"] = gen_random();
    default_config["1"] = "Glowing Stripe";
    default_config["2"] = 60;
    default_config["4"]["a"] = "rai";
    default_config["4"]["b"] = "Rainbow";
    default_config["4"]["d"]["d"] = 1.0;
    default_config["4"]["d"]["e"] = "manual";
    default_config["4"]["d"]["f"] = 20000;
    default_config["4"]["d"]["g"] = 0;


    if (serializeJson(default_config, file) == 0) {
        Serial.println("Failed to serializeJson to restore default config!");
    }
    default_config.clear();
    file.close();
    Serial.println("/stripe_config.json was restored to default");
    Serial.println("Rebooting...");
    ESP.restart();
}

bool copy_json(String path_from, String path_to){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || copy_json()");
    Serial.println("");

    StaticJsonDocument<850> origin_json;
    File origin_json_file = SPIFFS.open(path_from);
    if(!origin_json_file){
        Serial.println("Failed to open "+path_from+" for reading");
        origin_json.clear();
        origin_json_file.close();
        return false;
    }else {
        DeserializationError error = deserializeJson(origin_json, origin_json_file);
        if (error){
            Serial.println("Failed to read "+path_from+"!");
            Serial.println(error.c_str());
            origin_json.clear();
            origin_json_file.close();
            return false;
        } else {
            Serial.println("Loaded "+path_from);
            origin_json.clear();
            origin_json_file.close();
        }
    }
    

    Serial.println("Write to "+path_to+" ...");
    // Open file for writing
    File target_json_file = SPIFFS.open(path_to, FILE_WRITE);
    if (!target_json_file) {
        Serial.println("Failed to create "+path_to);
        origin_json.clear();
        target_json_file.close();
        return false;
    }
    // Serialize JSON to file
    if (serializeJson(origin_json, target_json_file) == 0) {
        Serial.println("Failed to write "+path_to);
        origin_json.clear();
        target_json_file.close();
        return false;
    }

    origin_json.clear();
    target_json_file.close();

    return true;
}

void restore_backup(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || restore_backup()");
    Serial.println("");

    // restore /stripe_config_backup.json, if backup exists - else restore_default_config
    if (!SPIFFS.exists("/stripe_config_backup.json")){
        Serial.print("/stripe_config_backup.json doesn't exist! Restoring default stripe_config.json...");
        restore_default_config();

    } else {
        Serial.println("Read backup...");
        if (!copy_json("/stripe_config_backup.json","/stripe_config.json")){
            Serial.println("Restoring backup failed!");
        } else {
            Serial.println("Success! Restored /stripe_config_backup.json to /stripe_config.json");

            Serial.println("Rebooting...");
            ESP.restart();
        }
    }
}

void check_stripe_config(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || check_stripe_config()");
    Serial.println("");

    // if stripe config does not exist -> restore from factory_settings, defined in code
    if (!SPIFFS.exists("/stripe_config.json")){
        Serial.print("/stripe_config.json doesn't exist! Restoring default stripe_config.json...");
        restore_default_config();
        
    } else if (!stripe_config_valid()){
        // if stripe config exist & is unvalid -> restore from backup & reboot
        Serial.print("/stripe_config.json is invalid! Restoring /stripe_config_backup.json...");
        restore_backup();
    } else {
        // if stripe config exist & is valid, backup current config and continue code
        Serial.print("/stripe_config.json is valid. Creating backup...");
        if (!copy_json("/stripe_config.json","/stripe_config_backup.json")){
            Serial.print("Creating /stripe_config_backup.json failed!");
        } else {
            Serial.print("Success. Continue boot process...");
        }
        
    }
}
