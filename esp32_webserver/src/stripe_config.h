#include "ArduinoJson.h"

StaticJsonDocument<850> load_strip_config();
void update_animation(String id, String name, String based_on_id, StaticJsonDocument<500> customizations);