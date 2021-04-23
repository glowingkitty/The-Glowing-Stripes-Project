#include "Arduino.h"
String role;
#include <ESPmDNS.h>

void become_client(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || become_client()");
    Serial.println("");

    role = "client";
}

void become_host(){
    Serial.println("");
    Serial.print("|| Core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" || become_host()");
    Serial.println("");

    role = "host";

    if (MDNS.begin("theglowingstripes")) {
        Serial.println("LED strip is now available under http://theglowingstripes.local");
    } else {
        Serial.println("Error setting up MDNS responder!");
    }

}

boolean device_is_client(){
    if (role=="client"){
        return true;
    }
    return false;
}