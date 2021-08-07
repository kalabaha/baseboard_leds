#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "version.h"

#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

#include "Adafruit_NeoPixel.h"

static Adafruit_NeoPixel strip_0(CONFIG_NEOPIXEL_CH0_NUM, CONFIG_NEOPIXEL_CH0_PIN, NEO_GRB + NEO_KHZ800);

static uint32_t _color = strip_0.Color(127, 127, 128);

static AsyncWebServer* _server;

#define TASK_WIFI_CONNECT_STACK_SIZE       (3000)
#define TASK_WIFI_CONNECT_PRIORITY         (configMAX_PRIORITIES - 5)  

static TaskHandle_t _wifi_connect_task_handler;

static void wifi_connect_task(void* parameter)
{
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
        _server = new AsyncWebServer(80);
        log_i("connected");

        if (SPIFFS.begin(true))
        {
            _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                       { request->redirect("/index.html"); });
            _server->serveStatic("/", SPIFFS, "/");
        }
        else
        {
            log_w("An Error has occurred while mounting SPIFFS");
        }
        _server->begin();
    }

    vTaskDelete(NULL);
}

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

    strip_0.begin();
    strip_0.show();
    strip_0.setBrightness(255);

    xTaskCreate(wifi_connect_task,
                "task_wifi_connect",
                TASK_WIFI_CONNECT_STACK_SIZE,
                NULL,
                TASK_WIFI_CONNECT_PRIORITY,
                &_wifi_connect_task_handler);
}


void loop()
{
    static uint32_t frame = 1;
    for (uint32_t i = 0; i < CONFIG_NEOPIXEL_CH0_NUM; ++i)
    {
        strip_0.setPixelColor(i, i < frame ? _color : 0);
    }

    frame = (frame + 1) % (CONFIG_NEOPIXEL_CH0_NUM + 1);
    strip_0.show();

    delay(1000);
}
