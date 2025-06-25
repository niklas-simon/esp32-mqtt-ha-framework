#include <Component.h>

#pragma once

typedef std::function<void ()> ButtonCommandHandler;

class Button: public Component {
    private:
        ButtonCommandHandler command_handler;

    public:
        Button(Device *parent, String id, String key, String name, String icon, ButtonCommandHandler command_handler): 
            Component(parent, id, key, name, "button", "", icon),
            command_handler(command_handler) {}

        void json(JsonObject obj) override;

        void on_command(String &payload) override;
};