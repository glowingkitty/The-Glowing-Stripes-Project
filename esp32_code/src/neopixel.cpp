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


//////////////////////////////////////

void start_leds(){
    StaticJsonDocument<850> led_strip_info = load_strip_config();
    
    int num_leds = led_strip_info["2"];
    int num_pin = 22;
    int time_passed_ms = 0;
    String previous_animation_id;
    Adafruit_NeoPixel leds(num_leds, num_pin, NEO_GRB + NEO_KHZ800);
    bool switch_direction {false};
    int start_r=0;
    int start_g=0;
    int start_b=0;
    leds.begin();

    Serial.println("Started LED strip:");
    Serial.println("id:                             "+ led_strip_info["0"].as<String>());
    Serial.println("name:                           "+ led_strip_info["1"].as<String>());
    Serial.println("num_of_leds:                    "+ led_strip_info["2"].as<String>());
    Serial.println("num_of_sections:                "+ led_strip_info["3"].as<String>());
    Serial.println("last_animation_id:              "+ led_strip_info["4"].as<String>());
    Serial.println("last_animation_customization:   "+ led_strip_info["5"].as<String>());

    // start animation loop
    for(;;){
        // leds.clear();

        // reload strip config on every loop
        led_strip_info.clear();
        StaticJsonDocument<850> led_strip_info = load_strip_config();
        String new_animation_id = led_strip_info["4"].as<String>();

        // detect if animation has changed - if true, update colors
        if (new_animation_id!=previous_animation_id){
            // set colors, which are looped over - so every time an animation repeats, it repeats with new colors
            rgb_colors.clear();

            if (led_strip_info["5"]["b"]=="random"){
                // generate random colors 
                int num_random_colors = led_strip_info["5"]["c"].as<int>();
                generate_random_colors(num_random_colors);
            }
            else {
                // get predefined colors
                for(int i = 0; i<led_strip_info["5"]["b"].size();i++){
                    vector<int> color;
                    color.push_back(led_strip_info["5"]["b"][i][0]);
                    color.push_back(led_strip_info["5"]["b"][i][1]);
                    color.push_back(led_strip_info["5"]["b"][i][2]);
                    rgb_colors.push_back(color);
                }
            }
        }
        // if colors are all processed, start with first color again
        else if (rgb_colors.size()==counter_current_color){
            counter_current_color = 0;
        }
        

        // get customization settings for animation
        int duration_ms = led_strip_info["5"]["f"].as<int>();
        int pause_a_ms = led_strip_info["5"]["g"].as<int>();
        float brightness = led_strip_info["5"]["d"].as<float>();
        String start = led_strip_info["5"]["k"];
        

        // define sections which should glow up
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

        //////////////////////////////////////
        // ANIMATIONS
        //////////////////////////////////////

        // Color
        if (new_animation_id == "col"){
            Serial.println("Glow color...");
            // glow in selected or random color, without transition
            leds.fill(leds.Color(
                        round(rgb_colors[0][0]*brightness),
                        round(rgb_colors[0][1]*brightness),
                        round(rgb_colors[0][2]*brightness)
                    ));
            leds.show();
            delay(500);
        }
        // Rainbow
        else if (new_animation_id == "rai"){
            Serial.println("Glow rainbow...");
            // TODO glow rainbow animation

            time_passed_ms += 60;
            
            for(int i=0; i<num_leds; i++) {
                // generate rainbow color
                int t = time_passed_ms/1000;

                float k = t + 0.05 * i;

                float r = 0.5 + 0.5 * cos(6.28318 * (1.0 * k + 0.00));
                float g = 0.5 + 0.5 * cos(6.28318 * (1.0 * k + 0.33));
                float b = 0.5 + 0.5 * cos(6.28318 * (1.0 * k + 0.67));

                r = round(255.0 * r * brightness);
                g = round(255.0 * g * brightness);
                b = round(255.0 * b * brightness);

                if (r > 255){
                    r=255;
                } else if (r<0){
                    r=0;
                }
                if (g > 255){
                    g=255;
                } else if (g<0){
                    g=0;
                }
                if (b > 255){
                    b=255;
                } else if (b<0){
                    b=0;
                }

                leds.setPixelColor(i, leds.Color(r,g,b));
            }
            leds.show();
        }
        // Beats
        else if (new_animation_id == "bea") {
            Serial.println("Glow beats...");
            // TODO make duration & pause 100 accurate, by calculating also required time for calculation

            int delay_step = ((duration_ms/num_leds)/2);
            for(int i=0; i<num_leds; i++) {
                if (start=="end"){
                    leds.setPixelColor(num_leds-i, leds.Color(
                        round(rgb_colors[counter_current_color][0]*brightness),
                        round(rgb_colors[counter_current_color][1]*brightness),
                        round(rgb_colors[counter_current_color][2]*brightness)
                    ));
                }
                else if (start=="start + end"){
                    leds.setPixelColor(i, leds.Color(
                        round(rgb_colors[counter_current_color][0]*brightness),
                        round(rgb_colors[counter_current_color][1]*brightness),
                        round(rgb_colors[counter_current_color][2]*brightness)
                    ));
                    leds.setPixelColor(num_leds-i, leds.Color(
                        round(rgb_colors[counter_current_color][0]*brightness),
                        round(rgb_colors[counter_current_color][1]*brightness),
                        round(rgb_colors[counter_current_color][2]*brightness)
                    ));
                }
                else if (start=="center"){
                    leds.setPixelColor((num_leds/2)+i, leds.Color(
                        round(rgb_colors[counter_current_color][0]*brightness),
                        round(rgb_colors[counter_current_color][1]*brightness),
                        round(rgb_colors[counter_current_color][2]*brightness)
                    ));
                    leds.setPixelColor((num_leds/2)-i, leds.Color(
                        round(rgb_colors[counter_current_color][0]*brightness),
                        round(rgb_colors[counter_current_color][1]*brightness),
                        round(rgb_colors[counter_current_color][2]*brightness)
                    ));
                }
                else {
                    // start = "start"
                    leds.setPixelColor(i, leds.Color(
                        round(rgb_colors[counter_current_color][0]*brightness),
                        round(rgb_colors[counter_current_color][1]*brightness),
                        round(rgb_colors[counter_current_color][2]*brightness)
                    ));
                }
                
                leds.show();
                delay(delay_step);
            }

            for(int i=num_leds; i>=0; i--) {
                if (start=="end"){
                    leds.setPixelColor(num_leds-i, leds.Color(0,0,0));
                }
                else if (start=="start + end"){
                    leds.setPixelColor((num_leds/2)+(num_leds-i), leds.Color(0,0,0));
                    leds.setPixelColor((num_leds/2)-(num_leds-i), leds.Color(0,0,0));
                }
                else if (start=="center"){
                    leds.setPixelColor((num_leds/2)+i, leds.Color(0,0,0));
                    leds.setPixelColor((num_leds/2)-i, leds.Color(0,0,0));
                }
                else {
                    // start = "start"
                    leds.setPixelColor(i, leds.Color(0,0,0));
                }
                
                leds.show();
                delay(delay_step);
            }
            delay(pause_a_ms);

        }
        // Moving dot
        else if (new_animation_id == "mov"){
            Serial.println("Glow moving dot...");
            int delay_step = (duration_ms/num_leds);
            // move forward
            if ((start=="start" && switch_direction==false) || (start=="end" && switch_direction==true)){
                for(int start_point=0; start_point<(num_leds+5); start_point++) {
                    for(int i=0; i<num_leds; i++) {
                        float dot_visibility;
                        if (i==start_point){
                            dot_visibility = 1.0;
                        }
                        else if (i==(start_point-1)){
                            dot_visibility = 0.7;
                        }
                        else if (i==(start_point-2)){
                            dot_visibility = 0.4;
                        }
                        else if (i==(start_point-3)){
                            dot_visibility = 0.1;
                        }
                        else if (i==(start_point-4)){
                            dot_visibility = 0.01;
                        }
                        else {
                            dot_visibility = 0;
                        }

                        int r = round((rgb_colors[counter_current_color][0]*brightness)*dot_visibility);
                        int g = round((rgb_colors[counter_current_color][1]*brightness)*dot_visibility);
                        int b = round((rgb_colors[counter_current_color][2]*brightness)*dot_visibility);
                        leds.setPixelColor(i, leds.Color(r,g,b));
                    }

                    leds.show();
                    delay(delay_step);
                }
            }
            // move backward
            else {
                for(int start_point=num_leds; start_point>-6; start_point--) {
                    for(int i=0; i<num_leds; i++) {
                        float dot_visibility;
                        if (i==start_point){
                            dot_visibility = 1.0;
                        }
                        else if (i==(start_point+1)){
                            dot_visibility = 0.7;
                        }
                        else if (i==(start_point+2)){
                            dot_visibility = 0.4;
                        }
                        else if (i==(start_point+3)){
                            dot_visibility = 0.1;
                        }
                        else if (i==(start_point+4)){
                            dot_visibility = 0.01;
                        }
                        else {
                            dot_visibility = 0;
                        }

                        int r = round((rgb_colors[counter_current_color][0]*brightness)*dot_visibility);
                        int g = round((rgb_colors[counter_current_color][1]*brightness)*dot_visibility);
                        int b = round((rgb_colors[counter_current_color][2]*brightness)*dot_visibility);
                        leds.setPixelColor(i, leds.Color(r,g,b));
                    }

                    leds.show();
                    delay(delay_step);
                }
            }
            
            // change direction
            switch_direction = !switch_direction;
            
            delay(pause_a_ms);
            
        }
        // Light up
        else if (new_animation_id == "lig"){
            Serial.println("Glow light up...");
            int brightness_steps = ((duration_ms/2)/20);
            int delay_step = ((duration_ms/2)/brightness_steps);

            // light up
            for (float max_brightness=0;max_brightness<=brightness;max_brightness+=(brightness/brightness_steps)){
                if (max_brightness>1){
                    max_brightness = 1;
                }
                if (all_sections){
                    leds.fill(leds.Color(
                        round(rgb_colors[counter_current_color][0]*max_brightness),
                        round(rgb_colors[counter_current_color][1]*max_brightness),
                        round(rgb_colors[counter_current_color][2]*max_brightness)
                    ));
                    
                } else {
                    // for every led in section, fill color
                    for(int i=0; i<num_leds; i++) {
                        if (find(section_leds.begin(), section_leds.end(), i) != section_leds.end()){
                            leds.setPixelColor(i, leds.Color(
                                round(rgb_colors[counter_current_color][0]*max_brightness),
                                round(rgb_colors[counter_current_color][1]*max_brightness),
                                round(rgb_colors[counter_current_color][2]*max_brightness)
                            ));
                        }
                    }
                }
                
                leds.show();
                delay(delay_step);
            }
            // light down
            for (float max_brightness=brightness;max_brightness>=0;max_brightness-=(brightness/brightness_steps)){
                if (max_brightness<0){
                    max_brightness = 0;
                }
                if (all_sections){
                    leds.fill(leds.Color(
                        round(rgb_colors[counter_current_color][0]*max_brightness),
                        round(rgb_colors[counter_current_color][1]*max_brightness),
                        round(rgb_colors[counter_current_color][2]*max_brightness)
                    ));
                    
                } else {
                    // for every led in section, fill color
                    for(int i=0; i<num_leds; i++) {
                        if (find(section_leds.begin(), section_leds.end(), i) != section_leds.end()){
                            leds.setPixelColor(i, leds.Color(
                                round(rgb_colors[counter_current_color][0]*max_brightness),
                                round(rgb_colors[counter_current_color][1]*max_brightness),
                                round(rgb_colors[counter_current_color][2]*max_brightness)
                            ));
                        }
                    }
                }
                leds.show();
                delay(delay_step);
            }

            leds.fill(leds.Color(0,0,0));
            leds.show();
            
        }
        // Transition
        else if (new_animation_id == "tra") {
            Serial.println("Glow transition...");
            // transition from previous to new color in x steps
            int num_of_steps = 255;

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
        
        else {
            Serial.println("No animation selected. Turn LEDs off.");
            leds.fill(leds.Color(0,0,0));
            leds.show();
            delay(100);
        }

        counter_current_color+=1;
        previous_animation_id = led_strip_info["4"].as<String>();
    }
}


