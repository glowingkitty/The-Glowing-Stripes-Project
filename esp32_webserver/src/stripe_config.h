#include "ArduinoJson.h"

StaticJsonDocument<850> load_strip_config();
void update_animation(StaticJsonDocument<800> new_animation);
StaticJsonDocument<850> update_this_led_strip(StaticJsonDocument<800> new_animation);
StaticJsonDocument<850> update_other_led_strip(
    StaticJsonDocument<800> new_animation,
    StaticJsonDocument<850> current_led_strip_config
    );