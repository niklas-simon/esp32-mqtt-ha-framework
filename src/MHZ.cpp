#include <Arduino.h>
#include "MHZ.h"

bool MHZ::await_state(int value) {
    unsigned long start = millis();
    
    while (digitalRead(pin) != value) {
        if (millis() - start >= timeout) {
            return false;
        }
    }  

    return true;
}

int MHZ::read() {
    // wait for low
    if (!await_state(LOW)) {
        return 0;
    }
    // wait for high
    if (!await_state(HIGH)) {
        return 0;
    }
    // set start time
    unsigned long start = micros();
    // wait for low
    if (!await_state(LOW)) {
        return 0;
    }
    // set stop_h time
    unsigned long stop_h = micros();
    // wait for high
    if (!await_state(HIGH)) {
        return 0;
    }
    // set stop_l time
    unsigned long stop_l = micros();

    unsigned long th = stop_h - start;
    unsigned long tl = stop_l - stop_h;
    unsigned long ppm = 5000 * (th - 2000UL) / (th + tl - 4000);

    return ppm;
}