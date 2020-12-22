#include <string>
#include <iostream>
using namespace std;
#include <SPI.h>
#include <SD.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "wifi_setup.hpp"
#include "neopixel.hpp"
#include "webserver.h"

 #ifdef __cplusplus
  extern "C" {
 #endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

NeoPixel leds;
WifiSetup wifisetup;

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    Serial.begin(115200);               // to enable Serial Commmunication with connected Esp32 board
    
    wifisetup.start_wifi();
    start_server();
}

void loop() {
    Serial.print("ESP32 Temperature: ");
  
    // Give out current temperature in degrees celcius
    Serial.print((temprature_sens_read() - 32) / 1.8);
    Serial.println(" C");
    leds.glow();
}