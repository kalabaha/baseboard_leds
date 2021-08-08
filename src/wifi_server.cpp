#include "config.h"

#include <Arduino.h>

#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "ESPAsyncWebServer.h"

/* -------------------------------------------------------------------------- */

#define TASK_WIFI_SERVER_STACK_SIZE         (3000)
#define TASK_WIFI_SERVER_PRIORITY           (configMAX_PRIORITIES - 5)  

static TaskHandle_t _wifi_server_task_handler;
static AsyncWebServer* _server;

/* -------------------------------------------------------------------------- */

static void wifi_server_task(void* parameter)
{
    String hostname = "BBL-" + String((uint32_t)(ESP.getEfuseMac()));
    log_i("Name: %s\n", hostname.c_str());
    WiFi.setHostname(hostname.c_str());

    log_i("connecting to SSID %s", CONFIG_WIFI_SSID);

    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    WiFi.waitForConnectResult();

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        log_w("connection failed");
    }
    else
    {
        if (!MDNS.begin("bbl"))
        {
            log_w("Error starting mDNS");
        }

        _server = new AsyncWebServer(80);
        log_i("connected with ip: %s", WiFi.localIP().toString().c_str());

        if (SPIFFS.begin(true))
        {
            _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) { request->redirect("/index.html"); });
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

/* -------------------------------------------------------------------------- */

void wifi_server_init(void)
{
    xTaskCreate(wifi_server_task,
                "wifi_server",
                TASK_WIFI_SERVER_STACK_SIZE,
                NULL,
                TASK_WIFI_SERVER_PRIORITY,
                &_wifi_server_task_handler);
}

/* -------------------------------------------------------------------------- */
