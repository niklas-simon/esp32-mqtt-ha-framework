#include <Component.h>

#pragma once

typedef std::function<void (bool value)> SwitchCommandHandler;

class Switch: public Component {
    private:
        SwitchCommandHandler command_handler;

    public:
        Switch(Device *parent, String id, String key, String name, String icon, SwitchCommandHandler command_handler): 
            Component(parent, id, key, name, "switch", "", icon),
            command_handler(command_handler) {}

        void json(JsonObject obj) override;

        void on_command(String &payload) override;
};