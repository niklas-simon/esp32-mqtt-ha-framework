#pragma once

class MHZ {
    private:
        int pin;
        int timeout;
        bool await_state(int value);

    public:
        MHZ(int pin, int timeout = 5000): pin(pin), timeout(timeout) {}
        int read();
};