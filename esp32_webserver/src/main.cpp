#include <SPIFFS.h>
#include "Arduino.h"
#include "webserver.h"
#include "ota_update.h"

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
  }
  
  start_wifi();
  start_server();
  start_ota();
}

void loop() {
  handle_ota();
  delay(500);
}