/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-wi-fi-manager-asyncwebserver/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include "../include/Configurator.h"
#include "FS.h"
#include <LittleFS.h>
#include <Preferences.h>

// Constants and global variables
const byte DNS_PORT = 53;
const int WEBSERVER_PORT = 80;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

DNSServer dnsServer;
WebServer server(WEBSERVER_PORT);

char ap_ssid[32];

TaskHandle_t webServerTaskHandle = NULL;

void setupAP()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    snprintf(ap_ssid, sizeof(ap_ssid), "esp_%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(ap_ssid, NULL);

    // DNS Server setup
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", apIP);
}

boolean isIp(String str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        int c = str.charAt(i);
        if (c != '.' && (c < '0' || c > '9'))
        {
            return false;
        }
    }
    return true;
}

String toStringIp(IPAddress ip)
{
    String res = "";
    for (int i = 0; i < 3; i++)
    {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);
    return res;
}

boolean captivePortal()
{
    if (!isIp(server.hostHeader()))
    {
        server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
        server.send(302, "text/plain", "");
        server.client().stop();
        return true;
    }
    return false;
}

String readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return String("");
    }

    Serial.println("- read from file:");
    
    size_t buf_size = 1024;
    char *out = (char *)malloc(buf_size * sizeof(char));

    int i;
    for(i = 0; file.available(); i++){
        if (i >= buf_size - 1) {
            buf_size *= 2;
            out = (char *)realloc(out, buf_size * sizeof(char));
        }

        out[i] = file.read();
    }
    file.close();

    out[i + 1] = '\0';

    String res = String(out);
    free(out);

    return res;
}

void handleHomePage()
{
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    
    String index;
    Preferences preferences;
    preferences.begin("ha");
    String wifi_host = preferences.getString("wifi_host", "");
    String wifi_ssid = preferences.getString("wifi_ssid", "");
    String mqtt_host = preferences.getString("mqtt_host", "");
    String mqtt_user = preferences.getString("mqtt_user", "");

    switch (server.method()) {
        case HTTP_GET:
            Serial.println("handling GET /");
            index = readFile(LittleFS, "/config.html");
            index = index.substring(0, index.length() - 1);
            int i;
            while ((i = index.indexOf("%wifi_host%")) != -1) {
                index = index.substring(0, i) + wifi_host + index.substring(i + 11);
            }
            while ((i = index.indexOf("%wifi_ssid%")) != -1) {
                index = index.substring(0, i) + wifi_ssid + index.substring(i + 11);
            }
            while ((i = index.indexOf("%mqtt_host%")) != -1) {
                index = index.substring(0, i) + mqtt_host + index.substring(i + 11);
            }
            while ((i = index.indexOf("%mqtt_user%")) != -1) {
                index = index.substring(0, i) + mqtt_user + index.substring(i + 11);
            }
            server.send(200, "text/html", index);
            break;
        case HTTP_POST:
            Serial.println("handling POST /");
            for (int i = 0; i < server.args(); i++) {
                String key = server.argName(i);
                String value = server.arg(i);

                if (value.isEmpty()) {
                    continue;
                }

                if (key.equals("wifi_host")) {
                    preferences.putString("wifi_host", value);
                } else if (key.equals("wifi_ssid")) {
                    preferences.putString("wifi_ssid", value);
                } else if (key.equals("wifi_password")) {
                    preferences.putString("wifi_password", value);
                } else if (key.equals("mqtt_host")) {
                    preferences.putString("mqtt_host", value);
                } else if (key.equals("mqtt_user")) {
                    preferences.putString("mqtt_user", value);
                } else if (key.equals("mqtt_password")) {
                    preferences.putString("mqtt_password", value);
                } else {
                    Serial.print("Unknown key: ");
                    Serial.println(key);
                }
            }
            server.send(200);
            ESP.restart();
            break;
    }
}

void handleAPHomepage() {
    if (captivePortal())
    {
        return;
    }

    handleHomePage();
}

void handleNotFound()
{
    String message = F("File Not Found\n\n");
    message += F("URI: ");
    message += server.uri();
    server.send(404, "text/plain", message);
}

void handleAPNotFound() {
    if (captivePortal())
    {
        return;
    }

    handleNotFound();
}

void setupWebServer(bool withAP)
{
    server.on("/", withAP ? handleAPHomepage : handleHomePage);
    server.onNotFound(withAP ? handleAPNotFound : handleNotFound);
    server.begin();
}

void webServerTask(void *pvParameters)
{
    bool withAP = *((bool *) pvParameters);
    setupWebServer(withAP);
    while (true)
    {
        if (withAP) {
            dnsServer.processNextRequest();
        }
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

Configurator::Configurator(bool _withAP) {
    withAP = _withAP;
}

void Configurator::setup() {
    if (withAP) {
        setupAP();
    }

    xTaskCreatePinnedToCore(
        webServerTask,
        "WebServerTask",
        8192,
        (void *) &withAP,
        1,
        &webServerTaskHandle,
        1);
}