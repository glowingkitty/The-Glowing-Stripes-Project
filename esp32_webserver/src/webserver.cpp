#include <Arduino.h>

#include <vector>
#include <algorithm>
#include <WiFi.h>
#include "esp_wifi.h"
#include <ESPmDNS.h>
#include <SPI.h>
#include <SD.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include <string>
#include <stdio.h>
#include "strip_config.h"
#include "AsyncJson.h"
#include <tuple>
using namespace std;

HardwareSerial Sender(2);

#define Sender_Txd_pin 17
#define Sender_Rxd_pin 16

const char* glowing_stripes_ssid = "TheGlowingStripes";
const char* glowing_stripes_password = "letsglow";
string role;
String host_ip_address;

AsyncWebServer server(80);

//// stripe_config.json fields - around 70bytes when fields are filled:
// p:data_pin
// 0:id
// 1:name
// 2:num_of_leds
// 3:num_of_sections
// 4:last_animation_id
// 5:last_animation_name
// 6:last_animation_based_on_animation_id
// 7:last_animation_customization
// 8:ip_address

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

DynamicJsonDocument connected_led_strips(2048); // equals about 30 LED strips (70bytes per LED strip)
JsonArray led_strips = connected_led_strips["online"].to<JsonArray>();

/////////////////////////////
/// Wifi
/////////////////////////////


boolean wifi_is_nearby(String wifi_ssid, int number_of_nearby_wifis){
    for (int i = 0; i < number_of_nearby_wifis; ++i) {
        // check for every possible wifi in known_wifi_ssids
        if (WiFi.SSID(i)==wifi_ssid){
            Serial.println("Known wifi found: "+wifi_ssid);
            WiFi.scanDelete();
            return true;
        }
    }
    return false;
}

boolean connect_to_ssid(String ssid,String password){
    Serial.println("Connecting...");
    delay(2000);
    WiFi.begin(ssid.c_str(), password.c_str());
    delay(10000);

    // if that fails, return false (causing a restart)
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to wifi '"+ssid+"'. Restarting...");
        ESP.restart();
    }

    Serial.println("");
    Serial.print("Connected LED strip to Wifi: ");
    Serial.print(ssid);
    Serial.println("");
    return true;
}

boolean connect_to_wifi(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || connect_to_wifi()");
    Serial.println("");

    // load wifi credentials, see if any wifi is online - if so, connect with it
    Serial.println("Check if any known wifi is online...");
    
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("no networks found. connect_to_wifi() failed.");
        WiFi.scanDelete();
        return false;
    } else {
        // try to connect to one of the known wifis in wifi_credentials.json
        if (SPIFFS.exists("/wifi_credentials.json")){
            Serial.println("Wifi credientias found");
            StaticJsonDocument<120> wifi_credentials;
            File wifi_credentials_file = SPIFFS.open("/wifi_credentials.json");
            if(!wifi_credentials_file){
                Serial.println("Failed to open wifi_credentials for reading");
            }else {
                DeserializationError error = deserializeJson(wifi_credentials, wifi_credentials_file);
                if (error){
                    Serial.println("Failed to read wifi_credentials_file.");
                } else {
                    Serial.println("Loaded wifi_credentials.json");

                    wifi_credentials_file.close();

                    // check if wifi_credentials is a single wifi network
                    if (wifi_credentials.containsKey("0") && wifi_credentials.containsKey("1")){
                        // check if wifi is online
                        if (wifi_is_nearby(wifi_credentials["0"],n)){
                            // if wifi is online, connect to it
                            return connect_to_ssid(wifi_credentials["0"].as<String>(),wifi_credentials["1"].as<String>());
                        }
                    }

                    // else check for every single wifi in wifi_credentials, if its online & connect to it
                    for (int i = 0; i < wifi_credentials.size();i++){
                        if (wifi_is_nearby(wifi_credentials[i]["0"],n)){
                            // if wifi is online, connect to it
                            return connect_to_ssid(wifi_credentials[i]["0"].as<String>(),wifi_credentials[i]["1"].as<String>());
                        }
                    }
                }
            }
        }

        // else check if default glowing_stripes_ssid is online
        if (wifi_is_nearby(glowing_stripes_ssid,n)){
            // if wifi is online, connect to it
            return connect_to_ssid(glowing_stripes_ssid,glowing_stripes_password);
        } else {
            Serial.println("No known wifi is nearby.");
            return false;
        }
    }
}

boolean host_is_online(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || host_is_online()");
    Serial.println("");

    // check if "theglowingstripes.local" is accessible
    // search for theglowingstripes domain
    if(mdns_init()!= ESP_OK){
        Serial.println("mDNS failed to start");
        return false;
    }
    IPAddress serverIp = MDNS.queryHost("theglowingstripes");
    if (serverIp){
        return true;
    }
    return false;
}

void update_animation(String id, String name, String based_on_id, StaticJsonDocument<500> customizations){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || update_animation()");
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
            led_strip_info["4"] = id;
            led_strip_info["5"] = name;
            if (based_on_id!=""){
                led_strip_info["6"] = based_on_id;
            }
            led_strip_info["7"] = customizations;

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
            Sender.println(serialized_json);
            
            Serial.println("Sent new stripe_config.json via Serial to other ESP:");
            Serial.println(serialized_json);
        }
    }
    
}


void become_client(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || become_client()");
    Serial.println("");

    role = "client";
    // remove host.txt file, if it exists
    if (SPIFFS.exists("/host.txt")){
        SPIFFS.remove("/host.txt");
    }
}

void become_host(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || become_host()");
    Serial.println("");

    role = "host";
    // create "host.txt" file, so ota_update.cpp knows this device is the host now - and sets the hotname to "theglowingstripes"
    File file = SPIFFS.open("/host.txt", FILE_WRITE);
    if(!file){
        Serial.println("There was an error opening host.txt for writing");
        return;
    }
    if(file.print("1")) {
        Serial.println("host.txt was written");
    }else {
        Serial.println("host.txt write failed");
    }
}

void start_hotspot(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_hotspot()");
    Serial.println("");
    
    WiFi.mode(WIFI_AP);          
    WiFi.softAP(glowing_stripes_ssid, glowing_stripes_password);
    // don't set hostname here, since ota_update.cpp will overwrite hostname after this
    Serial.println("Hotspot active now!");
}



void signup_led_strip(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || signup_led_strip()");
    Serial.println("");
    // make POST request to webserver to submit information like ip address and details
    StaticJsonDocument<850> led_strip_info = load_strip_config();
    
    led_strip_info["8"] = WiFi.localIP().toString();
    // set Setup as false (default)
    led_strip_info["s"] = false;
    Serial.println("ip_address:  "+ led_strip_info["8"].as<String>());

    // make post request
    if(role == "client" && WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
        HTTPClient http;   
        
        IPAddress serverIp = MDNS.queryHost("theglowingstripes");
        http.begin("http://"+serverIp.toString()+"/signup_led_strip");
        http.addHeader("Content-Type", "application/json");
        
        String json;
        serializeJson(led_strip_info, json);
        Serial.println(json);
        int httpResponseCode = http.POST(json);
        if(httpResponseCode==200){
            Serial.println("Signed up this LED strip to host");
        }else{
        
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        
        }
        http.end();  //Free resources
        return;

    } else if (role == "host"){
        // if led strip isn't connected to wifi - because its the host, add led strip directly to list of signed up led strips
        led_strips.add(led_strip_info);
        host_ip_address = led_strip_info["8"].as<String>();
        Serial.println("Signed up this LED strip as host. IP address: "+host_ip_address);
    } else {
        Serial.println("Error in signup_led_strip(), couldnt sign up LED strip.");
    }

}

void start_wifi(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_wifi()");
    Serial.println("");

    // setup Serial for sending new mode to LED strip
    Sender.begin(115200, SERIAL_8N1, Sender_Txd_pin, Sender_Rxd_pin);

    // see if host wifi is nearby (the one defined in credentials, or TheGlowingStripes wifi)
    if (connect_to_wifi()){
        if (host_is_online()){
            // if true, become a client (playing leds are get ready to take over host, if host goes offline)
            become_client();
        }
        else {
            become_host();
        }
    } else {
        // else become the host by starting the hotspot
        become_host();
        start_hotspot();
    }
    // sign up led strip to webserver
    signup_led_strip();
}



/////////////////////////////
/// Webserver
/////////////////////////////


void notFound(AsyncWebServerRequest *request) {
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || notFound()");
    Serial.println("");
    request->send(404, "application/json", "{\"error\":\"page not found\"}");
}

void start_server(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_server()");
    Serial.println("");

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.on("/online", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /online");
        Serial.println("");
        request->send(200, "application/json", "{\"online\":true}");
    });

    server.on("/activate_ota", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /activate_ota");
        Serial.println("");

        // Open file for writing
        StaticJsonDocument<850> led_strip_info;
        File led_strip_info_file = SPIFFS.open("/stripe_config.json");
        if(!led_strip_info_file){
            Serial.println("Failed to open stripe_config for reading");
            led_strip_info_file.close();
            request->send(500, "application/json", "{\"error\":\"Failed to open stripe_config for reading\"}");
        }else {
            DeserializationError error = deserializeJson(led_strip_info, led_strip_info_file);
            if (error){
                Serial.println("Failed to read led_strip_info_file.");
                request->send(500, "application/json", "{\"error\":\"Failed to read led_strip_info_file\"}");
            } else {
                Serial.println("Loaded stripe_config.json");
                led_strip_info_file.close();
                led_strip_info["u"] = true;

                // Submit new stripe_config.json via serial
                String serialized_json;
                serializeJson(led_strip_info, serialized_json);
                Sender.println(serialized_json);
                
                Serial.println("Sent new stripe_config.json via Serial to other ESP:");
                Serial.println(serialized_json);
                request->send(200, "application/json", "{\"success\":true}");
            }
        }
    });

    server.on("/wifi_networks_nearby", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /wifi_networks_nearby");
        Serial.println("");
        String json = "{\"networks\":[";

        Serial.println("Scan for Wifi nerworks...");
        int n = WiFi.scanNetworks();
        if (n == 0) {
            Serial.println("no networks found");
        } else {
            Serial.print(n);
            Serial.println(" networks found");
            
            vector<String> processed_essids;
            for (int i = 0; i < n; ++i) {
                // Print SSID and RSSI for each network found
                if (find(processed_essids.begin(), processed_essids.end(), WiFi.SSID(i)) != processed_essids.end()){
                }else{
                    if(i) json += ",";
                    json += "{";
                    json += "\"signal_strength\":"+String(WiFi.RSSI(i));
                    json += ",\"essid\":\""+WiFi.SSID(i)+"\"";
                    json += ",\"encryption\":"+String(WiFi.encryptionType(i) == 0 ? "off" : "on");
                    json += ",\"current_wifi\":false";
                    json += "}";
                    processed_essids.push_back(WiFi.SSID(i));
                }
            }
        }
        WiFi.scanDelete();

        json += "]}";
        request->send(200, "application/json", json);
        json = String();
    });

    server.on("/connected_led_strips", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /connected_led_strips");
        Serial.println("");
        String output;
        serializeJson(led_strips, output);
        request->send(200, "application/json", "{\"connected_led_strips\":"+output+"}");
    });

    AsyncCallbackJsonWebHandler* forward_changes_handler = new AsyncCallbackJsonWebHandler("/forward_changes", [](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /forward_changes");
        Serial.println("");

        JsonArray updates = json["updates"].as<JsonArray>();
        // for every update
        for (int e = 0; e < updates.size();e++){
            // for every led_strip in "ids"
            JsonArray ids = updates[e]["ids"].as<JsonArray>();
            for (int a = 0; a < ids.size();a++){
                // check if the id exist on the host server
                for (int i = 0; i < led_strips.size();i++){
                    if (led_strips[i]["0"].as<String>() == ids[a].as<String>()){
                        // if found, forward change to IP address of LED strip
                        String led_strip_id = led_strips[i]["0"].as<String>();
                        String led_strip_ip_address = led_strips[i]["8"].as<String>();

                        // make sure to remove "setup" before submitting it to LED strip
                        if (led_strips[i].containsKey("s")){
                            led_strips[i].remove("s");
                        }

                        // if the ip is the host ip, update host, else make post request
                        if (led_strip_ip_address==host_ip_address){
                            // update id of last animation
                            led_strips[i]["4"] = updates[e]["new_animation"]["4"].as<String>();
                            // update name of last animation
                            led_strips[i]["5"] = updates[e]["new_animation"]["5"].as<String>();
                            // update based_on_id of last animation
                            if (updates[e]["new_animation"].containsKey("6")){
                                led_strips[i]["6"] = updates[e]["new_animation"]["6"].as<String>();
                            }
                            // update customizations of last animation
                            if (updates[e]["new_animation"].containsKey("7")){
                                led_strips[i]["7"] = updates[e]["new_animation"]["7"].as<JsonObject>();
                            }

                            // send updated LED strip info (with new animation)
                            String serialized_json;
                            serializeJson(led_strips[i], serialized_json);
                            Sender.println(serialized_json);
                            
                            Serial.println("Updated LED strip "+led_strip_id+" via Serial.");
                            Serial.println(serialized_json);

                        } else {
                            HTTPClient http;
                            http.begin("http://"+led_strip_ip_address+"/mode");
                            http.addHeader("Content-Type", "application/json");
                            
                            int httpResponseCode = http.POST(updates[e].as<String>());
                            if(httpResponseCode==200){
                                // if response is successfull update led_strip entry in led_strips
                                // update id of last animation
                                led_strips[i]["4"] = updates[e]["new_animation"]["4"].as<String>();
                                // update customizations of last animation
                                led_strips[i]["7"] = updates[e]["new_animation"]["7"].as<JsonObject>();
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
                        }
                        
                        break;
                    }
                }
            }
            
        }
        
        request->send(200, "application/json", "{\"success\":true}");
    });

    AsyncCallbackJsonWebHandler* mode_handler = new AsyncCallbackJsonWebHandler("/mode", [](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /mode");
        Serial.println("");

        // change mode based on POST request
        update_animation(
            json["0"].as<String>(),
            json["1"].as<String>(),
            json.containsKey("2") ? json["2"].as<String>() : "",
            json["3"].as<JsonObject>()
        );
        
        Serial.println("Changed LED strip mode for "+json["0"].as<String>()+" to "+json["1"].as<String>());
        request->send(200, "application/json", "{\"success\":true}");
    });

    AsyncCallbackJsonWebHandler* signup_led_strip_handler = new AsyncCallbackJsonWebHandler("/signup_led_strip", [](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /signup_led_strip");
        Serial.println("");

        StaticJsonDocument<800> led_strip_config;
        DeserializationError error = deserializeJson(led_strip_config, json.as<String>());
        if (error){
            Serial.print(F("DeserializationError"));
            Serial.println(error.c_str());
            request->send(500, "application/json", "{\"error\":\"DeserializationError\"}");
        } else {
            // if the LED strip id ("0") already exists, remove existing version first
            for (int i = 0; i < led_strips.size();i++){
                if (led_strips[i]["0"]==led_strip_config["0"]){
                    led_strips.remove(i);
                    break;
                }
            }
            
            // add new LED strip data
            // make sure "setup" is added (but not saved to file)
            led_strips.add(led_strip_config);
            
            Serial.println("Signed up LED strip:");
            Serial.println(led_strip_config.as<String>());
            led_strip_config.clear();
            request->send(200, "application/json", "{\"success\":true}");
        }
    });

    server.addHandler(forward_changes_handler);
    server.addHandler(mode_handler);
    server.addHandler(signup_led_strip_handler);

    server.onNotFound(notFound);

    server.begin();
    Serial.println("Server is online!");
}