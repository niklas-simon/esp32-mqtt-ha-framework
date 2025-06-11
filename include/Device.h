#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class Component;

class Device {
    private:
        String id;
        String name;
        String origin;
        String node;
        String object;
        Component **cmps = nullptr;
        size_t cmps_len = 0;
        size_t cmps_max = 0;

    public:
        Device(String id, String name, String origin, String node, String object): 
            id(id),
            name(name),
            origin(origin),
            node(node),
            object(object),
            cmps(cmps),
            cmps_len(cmps_len) {}

        String base_topic();
        void json(JsonObject obj);
        void state(JsonObject obj);
        String error();
        void add_component(Component *cmp);
};