#include <Arduino.h>

void init_serial_to_web_esp();
void send_to_web_esp(String text);
boolean web_esp_available();
String web_esp_received_message();