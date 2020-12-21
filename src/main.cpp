#include <string>
#include <iostream>
using namespace std;
#include <SPI.h>
#include <SD.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "wifi_setup.hpp"
#include "webserver.h"
#include "neopixel.h"

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    Serial.begin(115200);               // to enable Serial Commmunication with connected Esp32 board
    start_leds();
    WifiSetup wifisetup;
    wifisetup.start_wifi();
    start_server();
}

void loop() {
    glow_leds();
}