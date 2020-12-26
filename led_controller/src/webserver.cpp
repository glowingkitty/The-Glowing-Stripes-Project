#include <Arduino.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void start_server(){
    server.on("/change_mode", HTTP_POST, [](AsyncWebServerRequest *request){
        DynamicJsonDocument new_mode(1024);

        if(request->hasParam("body", true)){
            AsyncWebParameter* p = request->getParam("body", true);
            // DynamicJsonBuffer buffer;
            // deserializeJson(led_strip, buffer.parseObject(p->value()));
            deserializeJson(new_mode, p->value());
            request->send(200, "application/json", "{\"success\":true}");
        }
    });

    server.onNotFound(notFound);

    server.begin();
}