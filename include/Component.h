#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Device.h>

class Component {
    protected:
        String id;
        String key;
        String name;
        String platform;
        String device_class;
        String icon;
        String state;
        String error;
        Device *parent;

    public:
        Component(Device *parent, String id, String key, String name, String platform, String device_class, String icon = "", String state = "None"):
            parent(parent),
            id(id),
            name(name),
            key(key),
            platform(platform),
            device_class(device_class),
            icon(icon),
            state(state),
            error("") {
            parent->add_component(this);
        }
        
        String get_key() { return key; }

        virtual void json(JsonObject obj);
        
        void set_state(String _state) { state = _state; }
        String get_state() { return state; }
        
        void set_error(String _error) { error = _error; }
        String get_error() { return error; }

        virtual void on_command(String &payload);
};