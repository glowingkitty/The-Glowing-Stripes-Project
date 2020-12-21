#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>

void beats(Adafruit_NeoPixel& pixels,int num_pixels){
    Serial.println("Glow beats...");
    // TODO
    for(int i=0; i<num_pixels; i++) {

        pixels.setPixelColor(i, pixels.Color(0, 150, 0));
        pixels.show();
        delay(50);
    }
}