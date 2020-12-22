#include <SPI.h>
#include <SD.h>
#include <vector>
using namespace std;

#include <Adafruit_NeoPixel.h>

void beats(
    Adafruit_NeoPixel& leds,
    int num_leds,
    vector<vector<int>> rgb_colors
    ){
    Serial.println("Glow beats...");
    // TODO

    for(int c=0; c<rgb_colors.size();c++){
        leds.clear();
        int r = rgb_colors[c][0];
        int g = rgb_colors[c][1];
        int b = rgb_colors[c][2];
        for(int i=0; i<num_leds; i++) {
            leds.setPixelColor(i, leds.Color(r,g,b));
            leds.show();
            delay(5);
        }
        for(int i=num_leds; i>=0; i--) {
            leds.setPixelColor(i, leds.Color(0,0,0));
            leds.show();
            delay(5);
        }
    }
}