#include <Arduino.h>
#include <WiFi.h>

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

DynamicJsonDocument led_strip_info(2048);

const char* ssid = "TheGlowingStripes";
const char* password = "letsglow";

void connect_to_host(){
  delay(500);
  
  WiFi.begin(ssid,password);                  // to tell Esp32 Where to connect and trying to connect
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Couldnt connect to host wifi. Trying again...");
    Serial.println(WiFi.status());
    
    WiFi.disconnect();
    delay(100);
    WiFi.begin(ssid,password);
    led_strip_info["ip_address"] = String(WiFi.localIP());
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


    // send "signup" request - and try as long as it fails
    led_strip_info["id"] = "9sjs82jas";
    led_strip_info["name"] = "LED strip";
    String json;
    serializeJson(led_strip_info, json);

    HTTPClient http;
    while (response_code!=200){
      Serial.println("Try to signup led strip to host...");
      http.begin("http://theglowingstripes.local/signup_led_strip");
      response_code = http.POST(json);
      // Disconnect
      http.end();

      if (response_code!=200){
        Serial.println("Signup failed. Received response code:");
        Serial.print(response_code);
        delay(1000);
      }
    }

}

void loop() {
    Serial.print("ESP32 Temperature: ");
  
    // Give out current temperature in degrees celcius
    Serial.print((temprature_sens_read() - 32) / 1.8);
    Serial.println(" C");
    leds.glow();

}