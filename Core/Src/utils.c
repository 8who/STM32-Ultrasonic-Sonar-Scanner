#include "utils.h"
#include "pins.h"
#include "tim.h"
#include <stdbool.h>

void delay_us(uint32_t us_time) {
    TIM_Set_Counter(tim_delay, 0);
    TIM_Enable(tim_delay, true);
    while (tim_delay->CNT < us_time);
    TIM_Enable(tim_delay, false);
}

void delay_ms(uint32_t ms) {
    while (ms--) delay_us(1000);
}
