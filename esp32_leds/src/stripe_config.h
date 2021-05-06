#include "ArduinoJson.h"

void check_stripe_config();
StaticJsonDocument<850> load_strip_config();
bool update_stripe_config_based_on_string(String new_config_text);
bool update_stripe_config(StaticJsonDocument<850> new_config);