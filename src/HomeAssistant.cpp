#include <Arduino.h>
#include <LittleFS.h>

#include <HomeAssistant.h>
#include <Configurator.h>

Preferences HomeAssistant::preferences;
WiFiClient HomeAssistant::wifiClient;
MQTTClient HomeAssistant::client;
int HomeAssistant::retries;
unsigned long HomeAssistant::interval;
unsigned long HomeAssistant::last_loop;
std::function<void ()> HomeAssistant::update_state;
Device* HomeAssistant::device = nullptr;
bool HomeAssistant::initialized = false;

void HomeAssistant::send_discovery() {
    Serial.println("sending discovery message");
    
    JsonDocument j_discovery;
    String discovery;

    device->json(j_discovery.to<JsonObject>());
    serializeJson(j_discovery, discovery);
    client.publish(device->base_topic() + "/config", discovery);
}

void HomeAssistant::on_message(String &topic, String &payload) {
    if (topic.equals("homeassistant/status")) {
        Serial.println("Discovery resend was requested");
        send_discovery();
    }
}

int HomeAssistant::connect(String wifi_host, String wifi_ssid, String wifi_password, String mqtt_host, String mqtt_user, String mqtt_password) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("connecting to WiFi ");
        WiFi.setHostname(wifi_host.c_str());
        WiFi.begin(wifi_ssid, wifi_password);
        while (WiFi.status() != WL_CONNECTED) {
            if (WiFi.status() == WL_CONNECT_FAILED) {
                return 1;
            }
            Serial.print('.');
            delay(1000);
        }

        Serial.println();
    }
    
    Serial.print("WiFi connected: ");
    Serial.println(WiFi.localIP());

    client.onMessage(on_message);
    client.begin(mqtt_host.c_str(), wifiClient);
    
    if (!client.connected()) {
        Serial.print("connecting to MQTT ");

        while (!client.connect(wifi_host.c_str(), mqtt_user.c_str(), mqtt_password.c_str())) {
            if (client.lastError() != LWMQTT_SUCCESS) {
                Serial.println(client.lastError());
                return 1;
            }
            Serial.print(".");
            delay(1000);
        }

        Serial.println();
    }

    Serial.println("MQTT connected");

    return 0;
}

void HomeAssistant::configure(Device *_device, std::function<void ()> _update_state, int _retries, int _interval) {
    device = _device;
    update_state = _update_state;
    retries = _retries;
    interval = _interval;
}

void HomeAssistant::begin() {
    if (device == nullptr) {
        Serial.println("HomeAssistant::begin: error: begin called before configure");
        return;
    }

    last_loop = 0;

    Serial.println("mounting LittleFS");

    if(!LittleFS.begin()){
        Serial.println("LittleFS Mount Failed");
        return;
    }

    Serial.println("reading configuration");
    preferences.begin("ha");

    String wifi_host = preferences.getString("wifi_host", "");
    String wifi_ssid = preferences.getString("wifi_ssid", "");
    String wifi_password = preferences.getString("wifi_password", "");
    String mqtt_host = preferences.getString("mqtt_host", "");
    String mqtt_user = preferences.getString("mqtt_user", "");
    String mqtt_password = preferences.getString("mqtt_password", "");

    if (wifi_host.isEmpty() || wifi_ssid.isEmpty() || wifi_password.isEmpty() || mqtt_host.isEmpty() || mqtt_user.isEmpty() || mqtt_password.isEmpty()) {
        Serial.println("opening captive portal");
        Configurator config = Configurator();
        config.setup();
        return;
    }

    Serial.println("successfully read configuration");

    int tries;
    for (tries = 1; true; tries++) {
        if (connect(wifi_host, wifi_ssid, wifi_password, mqtt_host, mqtt_user, mqtt_password) == 0) {
            break;
        }

        if (tries < retries) {
            Serial.print("failed to connect. Retrying (");
            Serial.print(tries);
            Serial.println(")");
            continue;
        }

        Serial.println("failed to connect. Restarting...");
        preferences.clear();
        ESP.restart();
        return;
    }

    send_discovery();

    initialized = true;

    Serial.println("initialization successful");
}

void HomeAssistant::loop() {
    if (!initialized) {
        Serial.println("HomeAssistant::loop: error: loop called before begin");
        return;
    }

    if (WiFi.status() != WL_CONNECTED || !client.connected()) {
        Serial.println("disconnected. Restarting...");
        ESP.restart();
    }

    client.loop();

    unsigned long current_loop = millis();
    if (last_loop && current_loop - last_loop < interval) {
        return;
    }
    last_loop = current_loop;

    update_state();

    String error = device->error();

    if (!error.isEmpty()) {
        Serial.println("sending error message");
        client.publish(device->base_topic() + "/error", error);
    }

    Serial.println("sending value message");
    
    JsonDocument j_value;
    String value;

    device->state(j_value.to<JsonObject>());
    serializeJson(j_value, value);
    client.publish(device->base_topic() + "/state", value);
}