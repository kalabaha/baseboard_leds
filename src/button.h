#pragma once

#include <stdint.h>

#define BUTTON_HOLD             (UINT32_MAX)
#define BUTTON_HOLD_RELEASE     (UINT32_MAX - 1)

void button_init(void (*cb)(uint32_t click_cnt));
