#include <Arduino.h>

HardwareSerial SerialToWebserver(2);

void init_serial_to_web_esp(){
    SerialToWebserver.begin(115200, SERIAL_8N1, 16, 17);
}

void send_to_web_esp(String text){
    SerialToWebserver.println(text);
}

boolean web_esp_available(){
    return SerialToWebserver.available();
}

String web_esp_received_message(){
    String message = "";
    message = SerialToWebserver.readStringUntil('\n');
    return message;
}