#include <vector>
#include <iostream>
using namespace std;
#include <Adafruit_NeoPixel.h>

void beats(
    Adafruit_NeoPixel& leds,
    int num_leds,
    vector<int> rgb_color
    ){
    Serial.println("Glow beats...");
    // TODO add duration, pause
    leds.clear();
    for(int i=0; i<num_leds; i++) {
        leds.setPixelColor(i, leds.Color(rgb_color[0],rgb_color[1],rgb_color[2]));
        leds.show();
        delay(5);
    }
    for(int i=num_leds; i>=0; i--) {
        leds.setPixelColor(i, leds.Color(0,0,0));
        leds.show();
        delay(5);
    }
}