#include "Arduino.h"
#include "neopixel.hpp"

// create neopixel led object
NeoPixel leds;

void setup() {
    Serial.begin(115200);

    
    

    // check if glowing stripes wifi exists

    // if it does, connect to it and set role as "client"


}

void loop() {

    // if role is "client", check if host is still online, else start hotspot to become host

    // play animation
    leds.glow();
}