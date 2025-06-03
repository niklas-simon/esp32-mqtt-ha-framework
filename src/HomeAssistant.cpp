#include <Arduino.h>
#include <Preferences.h>
#include <LittleFS.h>

#include "HomeAssistant.h"
#include "Configurator.h"

int HomeAssistant::connect(String wifi_host, String wifi_ssid, String wifi_password, String mqtt_host, String mqtt_user, String mqtt_password) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("Connecting to WiFi ");
        WiFi.setHostname(wifi_host.c_str());
        WiFi.begin(wifi_ssid, wifi_password);
        while (WiFi.status() != WL_CONNECTED) {
            if (WiFi.status() == WL_CONNECT_FAILED) {
                return 1;
            }
            Serial.print('.');
            delay(1000);
        }
        Serial.println(WiFi.localIP());
    }

    client.begin(mqtt_host.c_str(), wifiClient);

    if (!client.connected()) {
        Serial.print("Connecting to MQTT ");

        while (!client.connect(wifi_host.c_str(), mqtt_user.c_str(), mqtt_password.c_str())) {
            if (client.lastError() != LWMQTT_SUCCESS) {
                Serial.println(client.lastError());
                return 1;
            }
            Serial.print(".");
            delay(1000);
        }

        Serial.println("connected");
    }

    return 0;
}

void HomeAssistant::begin() {
    Preferences preferences;
    preferences.begin("ha");

    if(!LittleFS.begin()){
        Serial.println("LittleFS Mount Failed");
        return;
    }

    String wifi_host = preferences.getString("wifi_host", "");
    String wifi_ssid = preferences.getString("wifi_ssid", "");
    String wifi_password = preferences.getString("wifi_password", "");
    String mqtt_host = preferences.getString("mqtt_host", "");
    String mqtt_user = preferences.getString("mqtt_user", "");
    String mqtt_password = preferences.getString("mqtt_password", "");

    if (wifi_host.isEmpty() || wifi_ssid.isEmpty() || wifi_password.isEmpty() || mqtt_host.isEmpty() || mqtt_user.isEmpty() || mqtt_password.isEmpty()) {
        Serial.println("setting up captive portal");
        Configurator config = Configurator();
        config.setup();
        return;
    }

    Serial.println("already configured");

    if (connect(wifi_host, wifi_ssid, wifi_password, mqtt_host, mqtt_user, mqtt_password) != 0) {
        Serial.println("failed to connect. back to setup");
        preferences.clear();
        ESP.restart();
    }
}