#include <SPIFFS.h>
#include "Arduino.h"
#include "webserver.h"
#include "serial.h"
#include "ota_update.h"
#include "stripe_config.h"

void setup() {
  Serial.begin(115200);
  init_serial_to_led_esp();

  if (!SPIFFS.begin(true)) {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
  }

  check_stripe_config();
  
  start_wifi();
  start_server();
}

void loop() {
  check_ota_status();
  // TODO check if server still online
  // TODO check if ota_status from LED esp32 was received via serial
  delay(500);
}