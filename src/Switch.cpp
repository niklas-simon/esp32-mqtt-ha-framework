#include <Arduino.h>
#include "Switch.h"

void Switch::json(JsonObject obj) {
    Component::json(obj);

    obj["value_template"] = "{{ value_json." + key + " }}";
}

void Switch::on_command(String &payload) {
    command_handler(payload.equals("ON"));
}