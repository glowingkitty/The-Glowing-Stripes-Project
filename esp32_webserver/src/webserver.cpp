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
#include "AsyncJson.h"
#include <tuple>
#include "ota_update.h"
#include "serial.h"
#include "device_role.h"
#include "stripe_config.h"

using namespace std;

const char* glowing_stripes_ssid = "TheGlowingStripes";
const char* glowing_stripes_password = "letsglow";
String host_ip_address;

AsyncWebServer server(80);

DynamicJsonDocument connected_led_strips(2048);
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
    }

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
    } 
    
    Serial.println("No known wifi is nearby.");
    return false;

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
    if(device_is_client()){
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

    }
    
    // else if device is host
    // if led strip isn't connected to wifi - because its the host, add led strip directly to list of signed up led strips
    led_strips.add(led_strip_info);
    host_ip_address = led_strip_info["8"].as<String>();
    Serial.println("Signed up this LED strip as host. Host IP address: "+host_ip_address);

}

void start_wifi(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_wifi()");
    Serial.println("");

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

    server.on("/led_animations", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /led_animations");
        Serial.println("");

        // load json with all LED animations and details about them - and return them
        String output = load_json_as_string("/led_animations.json");
        
        request->send(200, "application/json", "{\"led_animations\":"+output+"}");
    });

    server.on("/web_control_config", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /web_control_config");
        Serial.println("");

        // load json with details about currently open mix, if animations are synced etc. - and return them
        String output = load_json_as_string("/web_control_config.json");
        
        request->send(200, "application/json", output);
    });

    

    // Over the internet update code:
    AsyncCallbackJsonWebHandler* update_firmware_handler = new AsyncCallbackJsonWebHandler("/update_firmware", [](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /update_firmware");
        Serial.println("");

        // make sure input json is led strip details with "u":true - to start update
        if (json["0"]){
            json["u"]=true;
            Serial.print("Input json correct.");
            String serialized_json;
            serializeJson(json, serialized_json);
            send_to_led_esp(serialized_json);
            request->send(200, "application/json", "{\"started_update\":true}");
        } else {
            Serial.print("Input json missing or incomplete! Make sure all LED strip details are included.");
            request->send(500, "application/json", "{\"started_update\":false}");
        }
        
    });

    AsyncCallbackJsonWebHandler* update_led_strips_handler = new AsyncCallbackJsonWebHandler("/update_led_strips", [](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /update_led_strips");
        Serial.println("");

        // send request to each led strip to update firmware
        JsonArray led_strips = json["led_strips"].as<JsonArray>();
        for (int e = 0; e < led_strips.size();e++){
            String id = led_strips[e]["0"].as<String>();
            String ip_address = led_strips[e]["8"].as<String>();
            HTTPClient http;
            http.begin("http://"+ip_address+"/update_firmware");
            http.addHeader("Content-Type", "application/json");
            
            int httpResponseCode = http.POST(led_strips[e].as<String>());

            if(httpResponseCode==200){
                Serial.println("Started firmware upgrade on "+led_strips[e]["0"].as<String>());
                update_firmware_status(id,"updating...");

            } else {
                Serial.println("Failed to update LED strip "+led_strips[e]["0"].as<String>()+" via POST request.");
                Serial.println("httpResponseCode:");
                Serial.println(httpResponseCode);
                Serial.println("response:");
                String response = http.getString();
                Serial.println(response);

                update_firmware_status(id,"failed");
            }
            http.end();
            
        }

        request->send(200, "application/json", "{\"started_update\":true}");
    });

    AsyncCallbackJsonWebHandler* forward_changes_handler = new AsyncCallbackJsonWebHandler("/forward_changes", [](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /forward_changes");
        Serial.println("");

        // for every update
        for (int e = 0; e < json["updates"].size();e++){
            // for every led_strip in "ids"
            JsonArray ids = json["updates"][e]["ids"].as<JsonArray>();
            for (int a = 0; a < ids.size();a++){
                // check if the id exist on the host server
                for (int i = 0; i < led_strips.size();i++){
                    if (led_strips[i]["0"].as<String>() == ids[a].as<String>()){

                        StaticJsonDocument<800> new_animation;
                        deserializeJson(new_animation, json["updates"][e]["new_animation"].as<String>());

                        if(host_ip_address == led_strips[i]["8"].as<String>()){
                            led_strips[i] = update_this_led_strip(new_animation,led_strips[i]);
                        } else {
                            led_strips[i] = update_other_led_strip(new_animation,led_strips[i]);
                        }

                        break;
                    }
                }
            }
            
        }
        
        request->send(200, "application/json", "{\"success\":true}");
    });

    AsyncCallbackJsonWebHandler* start_setup_mode_handler = new AsyncCallbackJsonWebHandler("/start_setup_mode", [](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /start_setup_mode");
        Serial.println("");

        Serial.print("Starting setup mode for... ");
        Serial.print(json["id"].as<String>());

        StaticJsonDocument<800> new_animation;
        new_animation["a"] = "set";
        new_animation["b"] = "Setup mode";
        StaticJsonDocument<30> customization;
        new_animation["d"] =  customization;
        new_animation["d"]["b"] = json["color"];

        // check if the id exist on the host server
        for (int i = 0; i < led_strips.size();i++){
            if (led_strips[i]["0"].as<String>() == json["id"].as<String>()){
                // if THIS LED strip needs to be updated, start update_this_led_strip(), else update_other_led_strip()
                if(host_ip_address == led_strips[i]["8"].as<String>()){
                    led_strips[i] = update_this_led_strip(new_animation,led_strips[i]);
                } else {
                    led_strips[i] = update_other_led_strip(new_animation,led_strips[i]);
                }
                break;
            }
        }

        request->send(200, "application/json", "{\"success\":true}");
    });

    AsyncCallbackJsonWebHandler* end_setup_mode_handler = new AsyncCallbackJsonWebHandler("/end_setup_mode", [](AsyncWebServerRequest *request, JsonVariant &json) {
        Serial.println("");
        Serial.print("|| Core ");
        Serial.print(xPortGetCoreID());
        Serial.print(" || /end_setup_mode");
        Serial.println("");

        // for every led strip in "setup_not_complete_led_strips_ids" - restore previous animation
        JsonArray ids = json["ids"].as<JsonArray>();
        Serial.print("Ending setup mode for... ");
        for(JsonVariant v : ids) {
            Serial.println(v.as<String>());
        }

        for (int a = 0; a < ids.size();a++){
            // check if the id exist on the host server
            for (int i = 0; i < led_strips.size();i++){
                if (led_strips[i]["0"].as<String>() == ids[a].as<String>()){
                    if(host_ip_address == led_strips[i]["8"].as<String>()){
                        // restore backup
                        led_strips[i] = update_this_led_strip(led_strips[i]["5"],led_strips[i]);
                    } else {
                        // restore backup
                        led_strips[i] = update_other_led_strip(led_strips[i]["5"],led_strips[i]);
                    }

                    // set setup as complete
                    led_strips[i]["s"] = true;

                    break;
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
        StaticJsonDocument<800> new_animation;
        deserializeJson(new_animation, json.as<String>());

        for (int i = 0; i < led_strips.size();i++){
            if (led_strips[i]["8"].as<String>() == host_ip_address){
                update_this_led_strip(new_animation,led_strips[i]);
                break;
            }
        }
        
        Serial.println("Changed LED strip mode to "+json["a"].as<String>());
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

    server.addHandler(update_firmware_handler);
    server.addHandler(update_led_strips_handler);
    server.addHandler(forward_changes_handler);
    server.addHandler(mode_handler);
    server.addHandler(start_setup_mode_handler);
    server.addHandler(end_setup_mode_handler);
    server.addHandler(signup_led_strip_handler);

    server.onNotFound(notFound);

    server.begin();
    Serial.println("Server is online!");
}