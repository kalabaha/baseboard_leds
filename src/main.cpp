#include <Arduino.h>
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

}

void loop()
{
    // put your main code here, to run repeatedly:
}