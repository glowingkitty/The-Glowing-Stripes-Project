#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "neopixel.hpp"
#include "webserver.h"
#include <HTTPClient.h>
#include "ArduinoJson.h"

 #ifdef __cplusplus
  extern "C" {
 #endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

NeoPixel leds;

int response_code;

const char* ssid = "TheGlowingStripes";
const char* password = "letsglow";

void connect_to_host(){
  WiFi.begin(ssid,password);  
  delay(500);              
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Coudnt connect to host. Restarting...");
    // TODO replace with a better solution to cut down boot time
    ESP.restart();
  }
}

void reconnect_to_new_host(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Host is offline. Reconnecting to new host...");
    connect_to_host();
}

void setup() {
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
    #endif
    Serial.begin(115200);               // to enable Serial Commmunication with connected Esp32 board

    // if connection to host drops, auto connect to new host
    WiFi.onEvent(reconnect_to_new_host, SYSTEM_EVENT_STA_DISCONNECTED);

    // connect to TheGlowingStripes wifi
    connect_to_host();

    // start server to receive "change mode" requests
    start_server();
}

void loop() {
    leds.glow();
}