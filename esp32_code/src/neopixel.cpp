#include <Adafruit_NeoPixel.h>
#include <vector>
#include <string>
using namespace std;
#include <math.h>
#include "strip_config.h"
#include <algorithm>

int counter_current_color = 0;
vector<vector<int>> rgb_colors;


void generate_random_colors(int num_random_colors){
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

boolean animation_has_changed(string new_animation_id,string previous_animation_id){
    // see if the animation has changed
    if (new_animation_id==previous_animation_id){
        return false;
    } else {
        return true;
    }
}

//////////////////////////////////////

void start_leds(){
    StaticJsonDocument<350> led_strip_info = load_strip_config();
    
    int num_leds = led_strip_info["2"];
    int num_pin = 22;
    Adafruit_NeoPixel leds(num_leds, num_pin, NEO_GRB + NEO_KHZ800);
    leds.begin();

    Serial.println("Started LED strip:");
    Serial.println("id:                             "+ led_strip_info["0"].as<String>());
    Serial.println("name:                           "+ led_strip_info["1"].as<String>());
    Serial.println("num_of_leds:                    "+ led_strip_info["2"].as<String>());
    Serial.println("num_of_sections:                "+ led_strip_info["3"].as<String>());
    Serial.println("last_animation_id:              "+ led_strip_info["4"].as<String>());
    Serial.println("last_animation_customization:   "+ led_strip_info["5"].as<String>());

    int num_random_colors = led_strip_info["5"]["c"].as<int>();
    generate_random_colors(num_random_colors);

    // start animation loop
    for(;;){
        // reload strip config on every loop
        led_strip_info.clear();
        StaticJsonDocument<350> led_strip_info = load_strip_config();

        // get customization settings for animation
        int duration_ms = led_strip_info["5"]["f"];
        int pause_a_ms = led_strip_info["5"]["g"];
        float brightness = led_strip_info["5"]["d"];
        bool all_sections {false};
        vector<int> section_leds;
        if (led_strip_info["5"]["j"].as<String>()=="all"){
            all_sections = true;
        } else {
            // if section random - generate random section number
            if (led_strip_info["5"]["j"].as<String>()=="random"){
                int section = (rand() % 4 + 1);

                if (section==1){
                    for (int processed_leds = 0; processed_leds<15; processed_leds++){
                        section_leds.push_back(processed_leds);
                    }
                }
                else if (section==2){
                    for (int processed_leds = 15; processed_leds<30; processed_leds++){
                        section_leds.push_back(processed_leds);
                    }
                }
                else if (section==3){
                    for (int processed_leds = 30; processed_leds<45; processed_leds++){
                        section_leds.push_back(processed_leds);
                    }
                }
                else if (section==4){
                    for (int processed_leds = 45; processed_leds<60; processed_leds++){
                        section_leds.push_back(processed_leds);
                    }
                }
            }

            // for every section, add leds to section_leds which should glow up
            for(int i = 0; i<led_strip_info["5"]["j"].size();i++){
                int section = led_strip_info["5"]["j"][i].as<int>();
                if (section==1){
                    for (int processed_leds = 0; processed_leds<15; processed_leds++){
                        section_leds.push_back(processed_leds);
                    }
                }
                else if (section==2){
                    for (int processed_leds = 15; processed_leds<30; processed_leds++){
                        section_leds.push_back(processed_leds);
                    }
                }
                else if (section==3){
                    for (int processed_leds = 30; processed_leds<45; processed_leds++){
                        section_leds.push_back(processed_leds);
                    }
                }
                else if (section==4){
                    for (int processed_leds = 45; processed_leds<60; processed_leds++){
                        section_leds.push_back(processed_leds);
                    }
                }
                
            }
        }

        // TODO detect if animation has changed - if true, update colors

        leds.clear();
        if (rgb_colors.size()==counter_current_color){
            counter_current_color = 0;
        }
        

        //////////////////////////////////////
        // ANIMATIONS
        //////////////////////////////////////

        // Off
        if (led_strip_info["4"].as<String>() == "111"){
            // TODO turn all leds off
        }
        // Color
        else if (led_strip_info["4"].as<String>() == "col"){
            // TODO glow selected or random color
        }
        // Rainbow
        else if (led_strip_info["4"].as<String>() == "rai"){
            // TODO glow rainbox animation
        }
        // Beats
        else if (led_strip_info["4"].as<String>() == "bea") {
            Serial.println("Glow beats...");
            // TODO make duration & pause 100 accurate, by calculating also required time for calculation
            int duration_ms = led_strip_info["5"]["f"].as<int>();
            int pause_a_ms = led_strip_info["5"]["g"].as<int>();
            float brightness = led_strip_info["5"]["d"].as<float>();
            int delay_step = ((duration_ms/num_leds)/2);

            for(int i=0; i<num_leds; i++) {
                leds.setPixelColor(i, leds.Color(
                    round(rgb_colors[counter_current_color][0]*brightness),
                    round(rgb_colors[counter_current_color][1]*brightness),
                    round(rgb_colors[counter_current_color][2]*brightness)
                    ));
                leds.show();
                delay(delay_step);
            }
            for(int i=num_leds; i>=0; i--) {
                leds.setPixelColor(i, leds.Color(0,0,0));
                leds.show();
                delay(delay_step);
            }

            delay(pause_a_ms);

        }
        // Moving dot
        else if (led_strip_info["4"].as<String>() == "mov"){
            // TODO glow moving dot
        }
        // Light up
        else if (led_strip_info["4"].as<String>() == "lig"){
            // TODO light up leds, from start, end, center or start and end
        }
        // Transition
        else if (led_strip_info["4"].as<String>() == "tra") {
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

                // if all sections, glow all leds, else only those in the selected sections
                if (all_sections){
                    leds.fill(leds.Color(
                        round(start_r*brightness),
                        round(start_g*brightness),
                        round(start_b*brightness)
                    ));
                    
                } else {
                    // for every led in section, fill color
                    for(int i=0; i<num_leds; i++) {
                        if (find(section_leds.begin(), section_leds.end(), i) != section_leds.end()){
                            leds.setPixelColor(i, leds.Color(
                                round(start_r*brightness),
                                round(start_g*brightness),
                                round(start_b*brightness)
                            ));
                        }
                    }
                }
                
                leds.show();
                delay(delay_step);
            }

            delay(pause_a_ms);
        }
        // 
        
        else {
            Serial.println("ERROR: last_animation_id doesnt exist: '"+led_strip_info["4"].as<String>()+"'");
        }

        counter_current_color+=1;
    }
}


