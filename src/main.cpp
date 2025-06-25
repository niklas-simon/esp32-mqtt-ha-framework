#include <Arduino.h>
#include <HomeAssistant.h>
#include "Switch.h"
#include "Button.h"

#define MON_PIN 10
#define PC_PIN 9

#define PC_ON_TIME 3500
unsigned long last_pc_press = 0;

void on_monitors(bool value);
void on_pc();

HomeAssistant ha = HomeAssistant();
Device desk = Device("e6b62730-cbe7-427a-a8c0-353904a4f296", "Desk Relais", "ESP32-C3", "livingroom", "desk_relais");
Switch monitors = Switch(&desk, "4b6496a7-bba2-4619-8f0b-b8e3a98d0b1a", "monitors", "Monitors", "mdi:monitor", on_monitors);
Button pc = Button(&desk, "4b6496a7-bba2-4619-8f0b-b8e3a98d0b1a", "pc", "PC", "mdi:desktop-tower", on_pc);

void on_monitors(bool value) {
    monitors.set_state(value ? "ON" : "OFF");
    digitalWrite(MON_PIN, value ? LOW : HIGH);
}

void on_pc() {
    digitalWrite(PC_PIN, LOW);
    last_pc_press = millis();
}

void update_state() {}

void setup()
{
    Serial.begin(9600);

    pinMode(MON_PIN, OUTPUT);
    pinMode(PC_PIN, OUTPUT);

    digitalWrite(MON_PIN, HIGH);
    digitalWrite(PC_PIN, HIGH);

    ha.configure(&desk, update_state, 5, 60000);
    ha.begin();

    monitors.set_state("OFF");
}

void loop()
{
    ha.loop();

    if (last_pc_press > 0) {
        if (millis() - last_pc_press >= PC_ON_TIME) {
            digitalWrite(PC_PIN, HIGH);
            last_pc_press = 0;
        }
    }
}