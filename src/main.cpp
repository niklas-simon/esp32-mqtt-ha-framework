#include <Arduino.h>
#include <HomeAssistant.h>

HomeAssistant ha = HomeAssistant();

void setup()
{
    Serial.begin(9600);
    while (!Serial);

    ha.begin();
}

void loop()
{
    delay(10);
}