#include <Device.h>
#include <Component.h>

String Device::base_topic() {
    return "homeassistant/device/" + node + "/" + object;
}

void Device::json(JsonObject obj) {
    JsonObject dev = obj["dev"].to<JsonObject>();
    
    dev["ids"] = id;
    dev["name"] = name;

    JsonObject o = obj["o"].to<JsonObject>();
    o["name"] = origin;

    JsonObject j_cmps = obj["cmps"].to<JsonObject>();
    for (int i = 0; i < cmps_len; i++) {
        JsonObject obj = j_cmps[cmps[i]->get_key()].to<JsonObject>();
        cmps[i]->json(obj);
    }
}

void Device::state(JsonObject obj) {
    for (int i = 0; i < cmps_len; i++) {
        if (cmps[i]->get_error().isEmpty()) {
            obj[cmps[i]->get_key()] = cmps[i]->get_state();
        } else {
            obj[cmps[i]->get_key()] = "None";
        }
    }
}

String Device::error() {
    String error = "";

    for (int i = 0; i < cmps_len; i++) {
        if (!cmps[i]->get_error().isEmpty()) {
            if (!error.isEmpty()) {
                error += "\n";
            }
            error += cmps[i]->get_error();
        }
    }

    return error;
}

void Device::add_component(Component *cmp) {
    if (!cmps) {
        cmps = (Component **) malloc(2 * sizeof(Component *));
        cmps_max = 2;
    } else if (cmps_max <= cmps_len) {
        cmps_max *= 2;
        cmps = (Component **) realloc(cmps, cmps_max * sizeof(Component *));
    }

    cmps[cmps_len] = cmp;
    cmps_len++;
}

void Device::on_command(String &component, String &payload) {
    Serial.print(component);
    Serial.print(": ");
    Serial.println(payload);
    for (int i = 0; i < cmps_len; i++) {
        if (component.equals(cmps[i]->get_key())) {
            cmps[i]->on_command(payload);
        }
    }
}

void Device::subscribe(MQTTClient &client) {
    String topic_prefix = base_topic() + "/command/";
    for (int i = 0; i < cmps_len; i++) {
        client.subscribe(topic_prefix + cmps[i]->get_key());
    }
}