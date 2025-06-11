#include <Component.h>

void Component::json(JsonObject obj) {
    obj["unique_id"] = id;
    obj["name"] = name;
    obj["platform"] = platform;
    obj["device_class"] = device_class;
    
    obj["~"] = parent->base_topic();
    obj["state_topic"] = "~/state";
    obj["command_topic"] = "~/command";
}