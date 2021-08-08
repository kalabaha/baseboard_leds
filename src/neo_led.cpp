#include "config.h"

#include <Arduino.h>

#include "Adafruit_NeoPixel.h"

#include "utils.h"

/* -------------------------------------------------------------------------- */

static Adafruit_NeoPixel strip_0(CONFIG_NEOPIXEL_CH0_NUM, CONFIG_NEOPIXEL_CH0_PIN, NEO_GRB + NEO_KHZ800);

static uint32_t _color_list[] = 
{
    strip_0.Color(127, 127, 127),
    strip_0.Color(127, 0, 0),
    strip_0.Color(0, 127, 0),
    strip_0.Color(0, 0, 127)
};
static uint32_t _color_index = 0;

/* -------------------------------------------------------------------------- */

#define TASK_NEO_LED_STACK_SIZE             (3000)
#define TASK_NEO_LED_PRIORITY               (configMAX_PRIORITIES - 5)  

static TaskHandle_t _neo_led_task_handler;

/* -------------------------------------------------------------------------- */

static void neo_led_task(void* parameter)
{
    for (;;)
    {
        static uint32_t frame = 1;
        for (uint32_t i = 0; i < CONFIG_NEOPIXEL_CH0_NUM; ++i)
        {
            strip_0.setPixelColor(i, i < frame ? _color_list[_color_index] : 0);
        }

        frame = (frame + 1) % (CONFIG_NEOPIXEL_CH0_NUM + 1);
        strip_0.show();

        delay(1000);
    }
}

/* -------------------------------------------------------------------------- */

void neo_led_init(void)
{
    strip_0.begin();
    strip_0.show();
    strip_0.setBrightness(255);

    xTaskCreate(neo_led_task,
                "neo_led",
                TASK_NEO_LED_STACK_SIZE,
                NULL,
                TASK_NEO_LED_PRIORITY,
                &_neo_led_task_handler);
}

/* -------------------------------------------------------------------------- */

void neo_led_next_mode(void)
{
    _color_index = (_color_index + 1) % ARRAY_SIZE(_color_list);
}

/* -------------------------------------------------------------------------- */
