#pragma once

class Configurator {
    private:
        bool withAP;

    public:
        Configurator(bool _withAP);
        void setup();
};