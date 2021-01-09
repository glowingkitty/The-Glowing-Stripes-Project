#include <Adafruit_NeoPixel.h>
#include <vector>
#include <string>
using namespace std;


// #define ANALOG_PIN 32
int num_pin = 22;
int num_leds = 60;

Adafruit_NeoPixel leds(num_leds, num_pin, NEO_GRB + NEO_KHZ800);


string previous_animation;
string new_animation = "beats";

int num_random_colors = 5;
int counter_current_color = 0;
vector<vector<int>> rgb_colors;
int loudness;
int previous_loudness;

void generate_random_colors(){
    // define random rgb colors here, loop over them and create new random colors if mode is changing 
    for (int i=0; i<num_random_colors; i++){
        vector<int> color;
        color.push_back(rand() % 255 + 1);
        color.push_back(rand() % 255 + 1);
        color.push_back(rand() % 255 + 1);
        rgb_colors.push_back(color);
    }
}

boolean animation_has_changed(){
    // see if the animation has changed
    if (new_animation==previous_animation){
        return false;
    } else {
        return true;
    }
}

void beats(){
    Serial.println("Glow beats...");
    // TODO add duration, pause
    leds.clear();
    for(int i=0; i<num_leds; i++) {
        leds.setPixelColor(i, leds.Color(rgb_colors[counter_current_color][0],rgb_colors[counter_current_color][1],rgb_colors[counter_current_color][2]));
        leds.show();
        delay(10);
    }
    for(int i=num_leds; i>=0; i--) {
        leds.setPixelColor(i, leds.Color(0,0,0));
        leds.show();
        delay(10);
    }
}

void init_leds(){
    leds.begin();
}

void glow(){
    if (rgb_colors.size()==counter_current_color){
        counter_current_color = 0;
    }

    if (animation_has_changed()==true){
        generate_random_colors();
    }

    // play animation
    if (new_animation == "beats") {
        beats();
    } else{
        Serial.println("ERROR: new_animation not found.");
    }

    previous_animation = new_animation;
    counter_current_color+=1;
}

