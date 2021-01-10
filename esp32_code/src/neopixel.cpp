#include <Adafruit_NeoPixel.h>
#include <vector>
#include <string>
using namespace std;
#include <math.h>


// #define ANALOG_PIN 32
int num_pin = 22;
int num_leds = 60;
int duration_ms = 500;
int pause_ms = 0;


Adafruit_NeoPixel leds(num_leds, num_pin, NEO_GRB + NEO_KHZ800);


string previous_animation;
string new_animation = "transition";

int num_random_colors = 5;
int counter_current_color = 0;
vector<vector<int>> rgb_colors;


void generate_random_colors(){
    // define random rgb colors here, loop over them and create new random colors if mode is changing 
    for (int i=0; i<num_random_colors; i++){
        vector<int> color;
        int r = (rand() % 255 + 1);
        int g = (rand() % 255 + 1);
        int b = (rand() % 255 + 1);

        color.push_back(r);
        color.push_back(g);
        color.push_back(b);
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

//////////////////////////////////////
// ANIMATIONS
//////////////////////////////////////
void beats(){
    Serial.println("Glow beats...");
    // TODO make duration & pause 100 accurate, by calculating also required time for calculation
    int delay_step = ((duration_ms/num_leds)/2);

    for(int i=0; i<num_leds; i++) {
        leds.setPixelColor(i, leds.Color(rgb_colors[counter_current_color][0],rgb_colors[counter_current_color][1],rgb_colors[counter_current_color][2]));
        leds.show();
        delay(delay_step);
    }
    for(int i=num_leds; i>=0; i--) {
        leds.setPixelColor(i, leds.Color(0,0,0));
        leds.show();
        delay(delay_step);
    }
}

void transition(){
    Serial.println("Glow transition...");
    // transition from previous to new color in x steps
    int num_of_steps = 20;

    int start_r = rgb_colors.back()[0];
    int start_g = rgb_colors.back()[1];
    int start_b = rgb_colors.back()[2];
    if (counter_current_color>0){
        start_r = rgb_colors[counter_current_color-1][0];
        start_g = rgb_colors[counter_current_color-1][1];
        start_b = rgb_colors[counter_current_color-1][2];
    }

    int target_r = rgb_colors[counter_current_color][0];
    int target_g = rgb_colors[counter_current_color][1];
    int target_b = rgb_colors[counter_current_color][2];

    int by_step_change_r = round((target_r-start_r)/num_of_steps);
    int by_step_change_g = round((target_g-start_g)/num_of_steps);
    int by_step_change_b = round((target_b-start_b)/num_of_steps);

    int delay_step = (duration_ms/num_of_steps);

    
    for(int i=0; i<num_of_steps; i++) {
        start_r += by_step_change_r;
        start_g += by_step_change_g;
        start_b += by_step_change_b;

        if (start_r>255){
            start_r = 255;
        } else if (start_r<0){
            start_r = 0;
        }
        if (start_g>255){
            start_g = 255;
        } else if (start_g<0){
            start_g = 0;
        }
        if (start_b>255){
            start_b = 255;
        } else if (start_b<0){
            start_b = 0;
        }

        leds.fill(leds.Color(start_r,start_g,start_b));
        leds.show();
        delay(delay_step);
    }

}

//////////////////////////////////////

void init_leds(){
    leds.begin();
}

void glow(){

    leds.clear();
    if (rgb_colors.size()==counter_current_color){
        counter_current_color = 0;
    }

    if (animation_has_changed()==true){
        generate_random_colors();
    }

    // play animation
    if (new_animation == "beats") {
        beats();
    } else if (new_animation == "transition") {
        transition();
    } else{
        Serial.println("ERROR: new_animation not found.");
    }

    previous_animation = new_animation;
    counter_current_color+=1;
    
    delay(pause_ms);
}

