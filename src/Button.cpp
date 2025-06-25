#include <Arduino.h>
#include "Button.h"

void Button::json(JsonObject obj) {
    Component::json(obj);
}

void Button::on_command(String &payload) {
    command_handler();
}