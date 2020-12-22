#include "neopixel.hpp"
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <vector>
using namespace std;

#include "animations/setup_mode.h"
#include "animations/off.h"
#include "animations/color.h"
#include "animations/rainbow.h"
#include "animations/beats.h"
#include "animations/moving_dot.h"
#include "animations/light_up.h"
#include "animations/transition.h"

NeoPixel::NeoPixel()
{
    current_animation = "beats";
    num_pin = 22;
    num_leds = 30;
    leds = Adafruit_NeoPixel(num_leds, num_pin, NEO_GRB + NEO_KHZ800);
    leds.begin();
}

NeoPixel::~NeoPixel()
{

}

void NeoPixel::glow(){
    leds.clear();

    if (current_animation == "setup_mode") {
        setup_mode(leds,num_leds);
    } else if (current_animation == "rainbow") {
        rainbow(leds,num_leds);
    } else if (current_animation == "off") {
        off(leds,num_leds);
    } else if (current_animation == "color") {
        color(leds,num_leds);
    } else if (current_animation == "rainbow") {
        rainbow(leds,num_leds);
    } else if (current_animation == "beats") {
        int num_random_colors = 5;
        vector<vector<int>> rgb_colors;
        for (int i=0; i<num_random_colors; i++){
            vector<int> color;
            color.push_back(rand() % 255 + 1);
            color.push_back(rand() % 255 + 1);
            color.push_back(rand() % 255 + 1);
            rgb_colors.push_back(color);
        }
        
        beats(
            leds,
            num_leds,
            rgb_colors
            );
    } else if (current_animation == "moving_dot") {
        moving_dot(leds,num_leds);
    } else if (current_animation == "light_up") {
        light_up(leds,num_leds);
    } else if (current_animation == "transition") {
        transition(leds,num_leds);
    } else{
        Serial.println("ERROR: current_animation not found. Turning LEDs off");
        off(leds,num_leds);
    }
}