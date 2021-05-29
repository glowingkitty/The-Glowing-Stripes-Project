#include "ArduinoJson.h"

void check_stripe_config();
bool update_stripe_config(StaticJsonDocument<850> new_config);
StaticJsonDocument<850> load_strip_config();
String load_json_as_string(String path);
void update_animation(StaticJsonDocument<800> new_animation);
StaticJsonDocument<850> update_this_led_strip(
    StaticJsonDocument<800> new_animation,
    StaticJsonDocument<850> current_led_strip_config
    );
StaticJsonDocument<850> update_other_led_strip(
    StaticJsonDocument<800> new_animation,
    StaticJsonDocument<850> current_led_strip_config
    );