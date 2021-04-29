#include <Arduino.h>

HardwareSerial SerialToLEDs(2);

void init_serial_to_led_esp(){
    SerialToLEDs.begin(115200, SERIAL_8N1, 17, 16);
}

void send_to_led_esp(String text){
    Serial.println("Sending message via serial to led_esp... ('"+text+"')");
    SerialToLEDs.println(text);
}

boolean led_esp_available(){
    return SerialToLEDs.available();
}

String led_esp_received_message(){
    Serial.println("Receiving message via serial from led_esp...");
    String message = "";
    message = SerialToLEDs.readStringUntil('\n');
    return message;
}