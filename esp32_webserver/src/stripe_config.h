#include "ArduinoJson.h"

StaticJsonDocument<850> load_strip_config();
void update_animation(StaticJsonDocument<800> new_animation);