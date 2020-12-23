#include <Arduino.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "neopixel.hpp"

 #ifdef __cplusplus
  extern "C" {
 #endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

NeoPixel leds;

void setup() {
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
    #endif
    Serial.begin(115200);               // to enable Serial Commmunication with connected Esp32 board
}

void loop() {
    Serial.print("ESP32 Temperature: ");
  
    // Give out current temperature in degrees celcius
    Serial.print((temprature_sens_read() - 32) / 1.8);
    Serial.println(" C");
    leds.glow();
}