#include <SPIFFS.h>
#include "neopixel.h"
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "ota_update.h"
#include "wifi_connection.h"

void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    start_wifi();
    start_ota();
    start_leds();
}


void loop() {
    handle_ota();
    delay(1000);
}