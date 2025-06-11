#include <Arduino.h>
#include <HomeAssistant.h>
#include <Device.h>
#include <Sensor.h>
#include <DHT.h>
#include <MHZ.h>

#define DHTPIN 10
#define DHTTYPE DHT22
#define MHZPIN 9

DHT dht(DHTPIN, DHTTYPE);
MHZ mhz(MHZPIN);
HomeAssistant ha;

Device sensory = Device("273693b6-02fd-48e1-a2b4-5c1f78412d18", "Livingroom Air Sensory", "ESP32-C3", "livingroom", "air_sensory");
Sensor temp = Sensor(&sensory, "84bd3de1-9d11-455f-903f-959209b9e4bd", "temperature", "Temperature", "temperature", "°C");
Sensor humid = Sensor(&sensory, "ffb5bd31-5756-4849-a33f-53a289fd4d7b", "humidity", "Humidity", "humidity", "%");
Sensor co2 = Sensor(&sensory, "9d6b9616-1de4-4024-aa3d-71edcb4a01a6", "co2", "CO2", "carbon_dioxide", "ppm");

void update_state() {
    Serial.println("reading sensor values...");
    Serial.print("Temperature: ");

    float t = dht.readTemperature();
    
    Serial.println(t);
    Serial.print("Humidity: ");
    
    float h = dht.readHumidity();

    Serial.println(h);
    Serial.print("CO2: ");

    int ppm = mhz.read();

    Serial.println(ppm);

    if (isnan(t)) {
        temp.set_error("could not read from sensor");
    } else {
        temp.set_error("");
        temp.set_state(String(t));
    }

    if (isnan(h)) {
        humid.set_error("could not read from sensor");
    } else {
        humid.set_error("");
        humid.set_state(String(h));
    }

    if (ppm <= 0) {
        co2.set_error("could not read from sensor");
    } else {
        co2.set_error("");
        co2.set_state(String(ppm));
    }

    Serial.println("sensor read");
}

void setup()
{
    Serial.begin(9600);

    ha.configure(&sensory, update_state);
    ha.begin();

    dht.begin();
}

void loop()
{
    ha.loop();
}