#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>

void beats(Adafruit_NeoPixel& leds,int num_pixels){
    Serial.println("Glow beats...");
    // TODO
    for(int i=0; i<num_pixels; i++) {

        leds.setPixelColor(i, leds.Color(0, 150, 0));
        leds.show();
        delay(50);
    }
}