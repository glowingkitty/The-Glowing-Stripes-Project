#include "neopixel.hpp"
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "animations/boot.h"
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
    boot(leds,num_leds);
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
        beats(leds,num_leds);
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