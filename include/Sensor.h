#include <Component.h>

#pragma once

class Sensor: public Component {
    private:
        String unit;

    public:
        Sensor(Device *parent, String id, String key, String name, String device_class, String unit): 
            Component(parent, id, key, name, "sensor", device_class),
            unit(unit) {}

        void json(JsonObject obj) override;
};