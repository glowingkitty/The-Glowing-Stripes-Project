#include "ArduinoJson.h"

void start_wifi();
void start_server();
void update_animation(String id,StaticJsonDocument<500> customizations);
boolean host_is_online();
void become_host();
void update_firmware_status(String stripe_id, String new_status);