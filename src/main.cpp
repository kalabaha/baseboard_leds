#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "version.h"

void setup()
{
    Serial.begin(CONFIG_UART_BAUD_RATE);
    Serial.setDebugOutput(true);

    printf("\n\n");
    printf("========================\n");
    if (BUILD_VERSION < 0)
    {
        printf("  LOCAL BUILD: %03d\n", - BUILD_VERSION);
    }
    else
    {
        printf("  FW REV: %03d\n", BUILD_VERSION);
    }
    printf("  %s %s\n", __DATE__, __TIME__);
    printf("========================\n");
    printf("\n");

    String hostname = "BBL-" + String((uint32_t)(ESP.getEfuseMac()));
    log_i("Name: %s\n", hostname.c_str());
    WiFi.setHostname(hostname.c_str());

    log_i("connecting to SSDI %s", CONFIG_WIFI_SSID);

    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    WiFi.waitForConnectResult();

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        log_w("connection failed");
    }
    else
    {
        log_i("connected");
    }
}

void loop()
{
    // put your main code here, to run repeatedly:
}