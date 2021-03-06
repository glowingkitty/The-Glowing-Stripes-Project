#include <Adafruit_NeoPixel.h>
#include <vector>
#include <string>
using namespace std;
#include <math.h>
#include "strip_config.h"
#include <algorithm>
#include "ota_update.h"
#include "wifi_connection.h"

int counter_current_color = 0;
vector<vector<int>> rgb_colors;

bool skip_remaining_animation {false};
bool received_stop_animation_command {false};
bool stopped_animation {false};


void generate_random_colors(int num_random_colors){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || generate_random_colors()");
    Serial.println("");
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

void stop_animation(Adafruit_NeoPixel leds){
    if (stopped_animation==false){
        // TODO decrease brightness of last LED state step by step (quickly)


        // then turn LEDs off
        leds.fill(leds.Color(0,0,0));
        leds.show();
        stopped_animation = true;
    }
}

void start_leds(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || start_leds()");
    Serial.println("");
    StaticJsonDocument<850> led_strip_info = load_strip_config();
    
    int num_leds = led_strip_info.containsKey("2") ? led_strip_info["2"].as<int>() : 60;
    int num_pin = led_strip_info.containsKey("p") ? led_strip_info["p"].as<int>() : 22;
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
    Serial.println("last_animation_id:              "+ led_strip_info["4"].as<String>());
    Serial.println("last_animation_customization:   "+ led_strip_info["7"].as<String>());

    // start animation loop
    for(;;){

        // reset stop_animation
        skip_remaining_animation = false;
        stopped_animation = false;

        // reload strip config on every loop
        led_strip_info.clear();
        StaticJsonDocument<850> led_strip_info = load_strip_config();

        // check if strip should prepare for update, if so - stop animation loop, start wifi and OTA
        if (led_strip_info.containsKey("u") && led_strip_info["u"].as<bool>()==true){
            Serial.println("Received 'Prepare for software update' command. Turning LEDs off, starting WIFI and starting OTA update...");
            // turn leds off
            leds.fill(leds.Color(0,0,0));
            leds.show();
            break;
        }

        String new_animation_id = led_strip_info["4"].as<String>();

        // detect if animation has changed - if true, update colors
        if (new_animation_id!=previous_animation_id){
            // set colors, which are looped over - so every time an animation repeats, it repeats with new colors
            rgb_colors.clear();

            if (led_strip_info["7"].containsKey("b")==false || led_strip_info["7"]["b"]=="random"){
                // generate random colors 
                int num_random_colors = led_strip_info["7"].containsKey("c") ? led_strip_info["7"]["c"].as<int>() : 5;
                generate_random_colors(num_random_colors);
            }
            else {
                // get predefined colors
                for(int i = 0; i<led_strip_info["7"]["b"].size();i++){
                    vector<int> color;
                    color.push_back(led_strip_info["7"]["b"][i][0]);
                    color.push_back(led_strip_info["7"]["b"][i][1]);
                    color.push_back(led_strip_info["7"]["b"][i][2]);
                    rgb_colors.push_back(color);
                }
            }
        }
        // if colors are all processed, start with first color again
        else if (rgb_colors.size()==counter_current_color){
            counter_current_color = 0;
        }
        

        // get customization settings for animation | if they don't exist use default settings
        int duration_ms = led_strip_info.containsKey("7") ? led_strip_info["7"].containsKey("f") ? led_strip_info["7"]["f"].as<int>() :200 : 200;
        int pause_a_ms = led_strip_info.containsKey("7") ? led_strip_info["7"].containsKey("g") ? led_strip_info["7"]["g"].as<int>() :0 : 0;
        float max_brightness = led_strip_info.containsKey("7") ? led_strip_info["7"].containsKey("d") ? led_strip_info["7"]["d"].as<float>() : 1.0 : 1.0;
        // check if brightness is not fixed
        bool brightness_fixed = led_strip_info.containsKey("7") ? led_strip_info["7"].containsKey("m") ? led_strip_info["7"]["m"].as<bool>() : true : true;
        float brightness = brightness_fixed==false ? 0 : max_brightness;
        
        String start = led_strip_info.containsKey("7") ? led_strip_info["7"].containsKey("k") ? led_strip_info["7"]["k"].as<String>() : "start" : "start";
        

        // define sections which should glow up
        bool all_sections {true};
        if (led_strip_info.containsKey("7") && led_strip_info["7"].containsKey("j")){
            if (led_strip_info["7"]["j"].as<String>()!="all"){
                all_sections = !all_sections;
            }
        }

        vector<int> section_leds;
        if (all_sections==false) {
            // if section random - generate random section number
            if (led_strip_info["7"]["j"].as<String>()=="random"){
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
            for(int i = 0; i<led_strip_info["7"]["j"].size();i++){
                int section = led_strip_info["7"]["j"][i].as<int>();
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
            // if "Stop animation" command received, loop will be stopped and instead animation will fade away (decreasing brightness up to 0)
            if (received_stop_animation_command){
                stop_animation(leds);
            } else {
                // glow in selected or random color, without transition
                leds.fill(leds.Color(
                            round(rgb_colors[0][0]*max_brightness),
                            round(rgb_colors[0][1]*max_brightness),
                            round(rgb_colors[0][2]*max_brightness)
                        ));
            
                leds.show();
                delay(500);
            }
            
        }
        // Rainbow
        else if (new_animation_id == "rai"){
            Serial.println("");
            Serial.print("|| Core ");
            Serial.print(xPortGetCoreID());
            Serial.print(" || Glow rainbow...");
            Serial.println("");
            
            // TODO make delay precise
            int steps = (256*5);
            float delay_step = (duration_ms/steps);
            double brightness_step = (1.0/(steps/2));

            // based on "rainbowCycle" from https://learn.adafruit.com/florabrella/test-the-neopixel-strip
            uint16_t i, j;
            for(j=0; j<steps; j++) {
                // if "Stop animation" command received, loop will be stopped and instead animation will fade away (decreasing brightness up to 0)
                if (received_stop_animation_command){
                    stop_animation(leds);
                    break;
                }

                for(i=0; i< num_leds; i++) {
                    byte WheelPos = ((i * 256 / num_leds) + j) & 255;
                    uint32_t Wheel;
                    if(WheelPos < 85) {
                        Wheel = leds.Color(round((WheelPos * 3)*brightness), round((255 - WheelPos * 3)*brightness), 0);
                    } else if(WheelPos < 170) {
                        WheelPos -= 85;
                        Wheel = leds.Color(round((255 - WheelPos * 3)*brightness), 0, round((WheelPos * 3)*brightness));
                    } else {
                        WheelPos -= 170;
                        Wheel = leds.Color(0, round((WheelPos * 3)*brightness), round((255 - WheelPos * 3)*brightness));
                    }

                    // add brightness + brightness going up and down if pause after animation
                    leds.setPixelColor(i, Wheel);
                }

                leds.show();
                delay(delay_step);

                // increase brightness if brightness not fixed
                if (brightness_fixed==false){
                    if (switch_direction){
                        brightness-=brightness_step;
                    } else {
                        brightness+=brightness_step;
                    }

                    // changing direction
                    if (brightness>=1){
                        brightness = 1;
                        switch_direction = true;
                    } else if (brightness<=0){
                        brightness = 0;
                        switch_direction = false;
                    }
                }

            }
            
            if (brightness_fixed==false){
                delay(pause_a_ms);
            }
        }
        // Beats
        else if (new_animation_id == "bea") {
            Serial.println("Glow beats...");
            // TODO make duration & pause 100 accurate, by calculating also required time for calculation

            // TODO add increasing/decreasing brightness

            int delay_step = ((duration_ms/num_leds)/2);
            for(int i=0; i<num_leds; i++) {
                // if "Stop animation" command received, loop will be stopped and instead animation will fade away (decreasing brightness up to 0)
                if (received_stop_animation_command){
                    stop_animation(leds);
                    break;
                }
                
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
                if (skip_remaining_animation==true || received_stop_animation_command){
                    stop_animation(leds);
                    break;
                }

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
                    if (skip_remaining_animation==true || received_stop_animation_command){
                        stop_animation(leds);
                        break;
                    }
                    for(int i=0; i<num_leds; i++) {
                        if (skip_remaining_animation==true || received_stop_animation_command){
                            stop_animation(leds);
                            break;
                        }
                        
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
                    if (skip_remaining_animation==true || received_stop_animation_command){
                        stop_animation(leds);
                        break;
                    }
                    for(int i=0; i<num_leds; i++) {
                        if (skip_remaining_animation==true || received_stop_animation_command){
                            stop_animation(leds);
                            break;
                        }
                        
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
                if (skip_remaining_animation==true || received_stop_animation_command){
                    stop_animation(leds);
                    break;
                }

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
                if (skip_remaining_animation==true || received_stop_animation_command){
                    stop_animation(leds);
                    break;
                }

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
            // TODO transition stops without an error.... fix it. Issue maybe num_of_steps? or brightness? or both?
            int num_of_steps = 50;

            int target_r = rgb_colors[counter_current_color][0];
            int target_g = rgb_colors[counter_current_color][1];
            int target_b = rgb_colors[counter_current_color][2];

            int by_step_change_r = round((target_r-start_r)/num_of_steps);
            int by_step_change_g = round((target_g-start_g)/num_of_steps);
            int by_step_change_b = round((target_b-start_b)/num_of_steps);

            int delay_step = (duration_ms/num_of_steps);

            
            for(int i=0; i<num_of_steps; i++) {
                if (received_stop_animation_command){
                    stop_animation(leds);
                    break;
                }
                
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
        
        // Software update
        else if (new_animation_id == "upd"){
            Serial.println("Show software update progress...");
            // show progress in leds
            int r = 0;
            int g = 0;
            int b = 0;

            if (led_strip_info["7"]["o"]=="filesystem"){
                g = 255;
            } else {
                b = 255;
            }

            int num_leds_to_glow = round(num_leds*led_strip_info["7"]["p"].as<float>());
            for(int i=num_leds; i>=0; i--) {
                if (i < num_leds_to_glow){
                    leds.setPixelColor(i, leds.Color(r,g,b));
                } else {
                    leds.setPixelColor(i, leds.Color(0,0,0));
                }
            }
        }

        // Error
        else if (new_animation_id == "err"){
            // show blinking red leds for warning
            for(float brightness=0; brightness<=1.0; brightness+=0.1) {
                leds.fill(leds.Color(round(255*brightness),0,0));
                delay(10);
            }
            for(float brightness=1.0; brightness>=0; brightness-=0.1) {
                leds.fill(leds.Color(round(255*brightness),0,0));
                delay(10);
            }
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

    // if "prepare_for_software_update": activate wifi and OTA
    start_wifi();
    start_ota();

    for(;;){
        handle_ota();
        delay(1000);
    }

}


