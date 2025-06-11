#include <Arduino.h>
#include <HomeAssistant.h>

HomeAssistant ha = HomeAssistant();
Device example = Device("561070cc-d216-4a10-9417-b14ee20db271", "Example Device", "ESP32-C3", "livingroom", "example");

void update_state() {

}

void setup()
{
    Serial.begin(9600);

    ha.configure(&example, update_state);
    ha.begin();
}

void loop()
{
    ha.loop();
}