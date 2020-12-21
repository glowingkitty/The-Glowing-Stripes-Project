#include <string>
#include <iostream>
using namespace std;
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN        22
#define NUMPIXELS 30

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 50


// define default animation
string ANIMATION =  "rainbow";

// define animations

void rainbow(){
  // make leds glow rainbow
  cout << "Starting rainbow...\n";

  for(int i=0; i<NUMPIXELS; i++) {

    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.show();
    delay(DELAYVAL);
  }
}

void beats(){

}

void transition(){

}

void start_leds(){
    pixels.begin();
}

void glow_leds(){
    pixels.clear();

    if (ANIMATION == "rainbow") {
        rainbow();
    } else{
        cout << "Animation not found";
    }
}