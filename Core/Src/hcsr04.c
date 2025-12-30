#include "hcsr04.h"
#include "gpio.h"
#include "pins.h"
#include "tim.h"
#include "usart.h"

uint32_t HCSR04_Read(void) {
    const uint32_t timeout_us = 30000; // 30 ms
    uint32_t start;
    uint32_t duration;

    // Ensure both timers are stopped first
    TIM_Enable(tim_delay, false);
    TIM_Enable(tim_hcsr04, false);

    /* Ensure ECHO low before trigger; wait up to timeout_us */
    TIM_Set_Counter(tim_delay, 0);
    TIM_Enable(tim_delay, true);

    if (echoPin.port->IDR & (1U << echoPin.pin)) {
        usart_puts("WARNING: ECHO already HIGH before trigger!");
        start = tim_delay->CNT;
        while ((echoPin.port->IDR & (1U << echoPin.pin))) {
            if ((tim_delay->CNT - start) > timeout_us) {
                TIM_Enable(tim_delay, false);
                return 0;
            }
        }
    }

    /* Trigger: 10 µs HIGH pulse */
    GPIO_Write(trigPin, GPIO_PIN_RESET);
    while (tim_delay->CNT < 2); // Wait 2us

    GPIO_Write(trigPin, GPIO_PIN_SET);
    while (tim_delay->CNT < 12); // Wait 10us more (total 12us)

    GPIO_Write(trigPin, GPIO_PIN_RESET);

    /* Wait for ECHO to go HIGH with timeout */
    TIM_Set_Counter(tim_delay, 0);
    start = tim_delay->CNT;
    while (!(echoPin.port->IDR & (1U << echoPin.pin))) {
        if ((tim_delay->CNT - start) > timeout_us) {
            TIM_Enable(tim_delay, false);
            usart_puts("TIMEOUT: ECHO never went HIGH");
            return 0;
        }
    }

    /* ECHO went HIGH - start measurement timer */
    TIM_Set_Counter(tim_hcsr04, 0);
    TIM_Enable(tim_hcsr04, true);

    /* Wait for ECHO to go LOW with timeout */
    start = tim_delay->CNT;
    while (echoPin.port->IDR & (1U << echoPin.pin)) {
        if ((tim_delay->CNT - start) > timeout_us) {
            TIM_Enable(tim_hcsr04, false);
            TIM_Enable(tim_delay, false);
            usart_puts("TIMEOUT: ECHO stayed HIGH too long");
            return 0;
        }
    }

    /* ECHO went LOW - stop timer and read duration */
    TIM_Enable(tim_hcsr04, false);
    TIM_Enable(tim_delay, false);
    duration = tim_hcsr04->CNT;

    return duration;
}
