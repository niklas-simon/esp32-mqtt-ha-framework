#include <Sensor.h>

void Sensor::json(JsonObject obj) {
    Component::json(obj);

    obj["unit_of_measurement"] = unit;
    obj["value_template"] = "{{ value_json." + key + " }}";
}