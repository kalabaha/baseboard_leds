#include "config.h"

#include <Arduino.h>

#include "button.h"
#include "neo_led.h"
#include "wifi_server.h"

/* -------------------------------------------------------------------------- */

static void _button_click_cb(uint32_t click_cnt)
{
    switch (click_cnt)
    {
    case BUTTON_HOLD:
    case BUTTON_HOLD_RELEASE:
        /* do nothing */
        break;

    default:
        neo_led_next_mode();
        break;
    }
}

/* -------------------------------------------------------------------------- */

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

    neo_led_init();
    button_init(_button_click_cb);
    wifi_server_init();
}

/* -------------------------------------------------------------------------- */

void loop()
{
}

/* -------------------------------------------------------------------------- */
