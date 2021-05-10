#include <Arduino.h>

HardwareSerial SerialToWebserver(2);

void init_serial_to_web_esp(){
    SerialToWebserver.begin(115200, SERIAL_8N1, 17, 16);
}

void send_to_web_esp(String text){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || send_to_web_esp()");
    Serial.println("");
    Serial.println("Sending message via serial to web_esp... ('"+text+"')");
    SerialToWebserver.println(text);
}

boolean web_esp_available(){
    return SerialToWebserver.available();
}

String web_esp_received_message(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || web_esp_received_message()");
    Serial.println("");
    String message = "";
    message = SerialToWebserver.readStringUntil('\n');
    return message;
}