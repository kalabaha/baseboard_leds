#include "config.h"

#include <Arduino.h>

#include "button.h"

/* -------------------------------------------------------------------------- */

#define TASK_BUTTON_STACK_SIZE          (3000)
#define TASK_BUTTON_PRIORITY            (configMAX_PRIORITIES - 6)  

/* -------------------------------------------------------------------------- */

static TaskHandle_t _button_task_handler = nullptr;
static xQueueHandle _button_queue_handle = nullptr;
static void (*_cb)(uint32_t click_cnt) = nullptr;

/* -------------------------------------------------------------------------- */

static void IRAM_ATTR _button_timer_isr()
{
    static uint16_t history = 0;
    static uint32_t click_cnt = 0;
    static uint32_t hold_cnt = 0;

    history <<= 1;
    history |= digitalRead(CONFIG_BUTTON_PIN);

    const uint16_t pattern = history & 0xF00F;

    switch (pattern)
    {
    /* btn press event */
    case 0xF000:
        history = 0x0000;
        ++click_cnt;
        hold_cnt = 0;
        break;

    /* btn release event */
    case 0x000F:
        history = 0xFFFF;
        if (hold_cnt == UINT32_MAX)
        {
            hold_cnt = BUTTON_HOLD_RELEASE;
            xQueueSendFromISR(_button_queue_handle, &hold_cnt, NULL);
        }
        hold_cnt = 0;
        break;

    /* btn is kept released */ 
    case 0xF00F:
        if (click_cnt)
        {
            ++hold_cnt;
            if (hold_cnt > 20)
            {
                xQueueSendFromISR(_button_queue_handle, &click_cnt, NULL);
                click_cnt = 0;
            }
        }
        break;
    
    /* btn is kept pressed */
    case 0x0000:
        if (hold_cnt != UINT32_MAX)
        {
            ++hold_cnt;
            if (hold_cnt == 40)
            {
                hold_cnt = BUTTON_HOLD;
                click_cnt = 0;
                xQueueSendFromISR(_button_queue_handle, &hold_cnt, NULL);
            }
        }
        break;
    }
}

/* -------------------------------------------------------------------------- */

static void button_task(void *parameter)
{
    for (;;)
    {
        uint32_t click_cnt;
        xQueueReceive(_button_queue_handle, &click_cnt, portMAX_DELAY);
        if (_cb)
        {
            log_i("button click: %d", click_cnt);
            _cb(click_cnt);
        }
    }
}

/* -------------------------------------------------------------------------- */

void button_init(void (*cb)(uint32_t clk_cnt))
{
    _cb = cb;

    _button_queue_handle = xQueueCreate(2, sizeof(uint32_t));

    pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);

    hw_timer_t* timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &_button_timer_isr, true);
    timerAlarmWrite(timer, 10000, true);
    timerAlarmEnable(timer);

    xTaskCreate(button_task,
                "button",
                TASK_BUTTON_STACK_SIZE,
                NULL,
                TASK_BUTTON_PRIORITY,
                &_button_task_handler);
}

/* -------------------------------------------------------------------------- */
