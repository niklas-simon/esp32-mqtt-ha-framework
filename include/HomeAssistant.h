#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>
#include <Device.h>
#include <Preferences.h>

#pragma once

class HomeAssistant {
    private:
        static Preferences preferences;
        static WiFiClient wifiClient;
        static MQTTClient client;
        static Device *device;
        static int retries;
        static unsigned long interval;
        static unsigned long last_loop;
        static bool initialized;
        static std::function<void ()> update_state;
        
        static void send_discovery();
        static void on_message(String &topic, String &payload);
        static int connect(String wifi_host, String wifi_ssid, String wifi_password, String mqtt_host, String mqtt_user, String mqtt_password);

    public:
        static void configure(Device *_device, std::function<void ()> _update_state, int _retries = 5, int _interval = 30000);
        static void begin();
        static void loop();
};