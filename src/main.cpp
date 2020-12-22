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
    leds.glow();
}