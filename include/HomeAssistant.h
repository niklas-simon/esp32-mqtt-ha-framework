#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>

class HomeAssistant {
    private:
        WiFiClient wifiClient;
        MQTTClient client;
        
        int connect(String wifi_host, String wifi_ssid, String wifi_password, String mqtt_host, String mqtt_user, String mqtt_password);

    public:
        void begin();
};