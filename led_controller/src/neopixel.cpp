#include "neopixel.hpp"
#include <Adafruit_NeoPixel.h>
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

#define ANALOG_PIN 32

NeoPixel::NeoPixel()
{
    new_animation = "beats";
    num_pin = 22;
    num_leds = 30;
    num_random_colors = 5;
    counter_current_color = 0;
    leds = Adafruit_NeoPixel(num_leds, num_pin, NEO_GRB + NEO_KHZ800);
    leds.begin();
}

NeoPixel::~NeoPixel()
{

}

void NeoPixel::generate_random_colors(){
    // define random rgb colors here, loop over them and create new random colors if mode is changing 
    for (int i=0; i<num_random_colors; i++){
        vector<int> color;
        color.push_back(rand() % 255 + 1);
        color.push_back(rand() % 255 + 1);
        color.push_back(rand() % 255 + 1);
        rgb_colors.push_back(color);
    }
}

boolean NeoPixel::animation_has_changed(){
    // see if the animation has changed
    if (new_animation==previous_animation){
        return false;
    } else {
        return true;
    }
}

void NeoPixel::glow(){
    loudness = analogRead(ANALOG_PIN);
    Serial.println("Previous");
    Serial.println(previous_loudness);
    Serial.println("New");
    Serial.println(loudness);
    
    if (loudness>previous_loudness){
        
        if (rgb_colors.size()==counter_current_color){
            counter_current_color = 0;
        }

        if (NeoPixel::animation_has_changed()==true){
            NeoPixel::generate_random_colors();
        }

        if (new_animation == "setup_mode") {
            setup_mode(leds,num_leds);
        } else if (new_animation == "rainbow") {
            rainbow(leds,num_leds);
        } else if (new_animation == "off") {
            off(leds,num_leds);
        } else if (new_animation == "color") {
            color(leds,num_leds);
        } else if (new_animation == "rainbow") {
            rainbow(leds,num_leds);
        } else if (new_animation == "beats") {
            beats(
                leds,
                num_leds,
                rgb_colors[counter_current_color]
                );
        } else if (new_animation == "moving_dot") {
            moving_dot(leds,num_leds);
        } else if (new_animation == "light_up") {
            light_up(leds,num_leds);
        } else if (new_animation == "transition") {
            transition(leds,num_leds);
        } else{
            Serial.println("ERROR: new_animation not found. Turning LEDs off");
            off(leds,num_leds);
        }

        previous_animation = new_animation;
        counter_current_color+=1;
    }

    previous_loudness = loudness;
}