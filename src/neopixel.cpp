#include <string>
#include <iostream>
using namespace std;
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// import animations
#include "animations/boot.h"
#include "animations/setup_mode.h"
#include "animations/off.h"
#include "animations/color.h"
#include "animations/rainbow.h"
#include "animations/beats.h"
#include "animations/moving_dot.h"
#include "animations/light_up.h"
#include "animations/transition.h"

#define PIN        22
#define NUMPIXELS 30

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// define default animation
string ANIMATION =  "beats";

void start_leds(){
    pixels.begin();
    // show boot animation
    boot(pixels,NUMPIXELS);
}

void glow_leds(){
    pixels.clear();

    if (ANIMATION == "setup_mode") {
        setup_mode(pixels,NUMPIXELS);
    } else if (ANIMATION == "rainbow") {
        rainbow(pixels,NUMPIXELS);
    } else if (ANIMATION == "off") {
        off(pixels,NUMPIXELS);
    } else if (ANIMATION == "color") {
        color(pixels,NUMPIXELS);
    } else if (ANIMATION == "rainbow") {
        rainbow(pixels,NUMPIXELS);
    } else if (ANIMATION == "beats") {
        beats(pixels,NUMPIXELS);
    } else if (ANIMATION == "moving_dot") {
        moving_dot(pixels,NUMPIXELS);
    } else if (ANIMATION == "light_up") {
        light_up(pixels,NUMPIXELS);
    } else if (ANIMATION == "transition") {
        transition(pixels,NUMPIXELS);
    } else{
        Serial.println("ERROR: Animation not found. Turning LEDs off");
        off(pixels,NUMPIXELS);
    }
}