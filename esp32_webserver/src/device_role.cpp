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

    if (!MDNS.begin("theglowingstripes")) {             // Start the mDNS responder for esp8266.local
        Serial.println("Error setting up MDNS responder!");
    }

}

boolean device_is_client(){
    if (role=="client"){
        return true;
    }
    return false;
}