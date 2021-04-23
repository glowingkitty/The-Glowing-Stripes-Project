#include "Arduino.h"
String role;

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
}

boolean device_is_client(){
    if (role=="client"){
        return true;
    }
    return false;
}