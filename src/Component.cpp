#include <Component.h>

void Component::json(JsonObject obj) {
    obj["unique_id"] = id;
    obj["name"] = name;
    obj["platform"] = platform;

    if (!device_class.isEmpty()) {
        obj["device_class"] = device_class;
    }
    
    obj["~"] = parent->base_topic();
    obj["state_topic"] = "~/state";
    obj["command_topic"] = "~/command/" + key;

    if (!icon.isEmpty()) {
        obj["icon"] = icon;
    }
}

void Component::on_command(String &payload) {
    Serial.print("received command: ");
    Serial.println(payload);
}