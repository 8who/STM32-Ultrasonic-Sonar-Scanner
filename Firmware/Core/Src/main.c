#include "main.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

#define GPIO_PIN_SET true
#define GPIO_PIN_RESET false

#define SERVO_LEFT 1000
#define SERVO_CENTER 1500
#define SERVO_RIGHT 2000

volatile uint32_t last_distance = 0;
volatile bool distance_ready = false;

int main(void) {

    SystemClock_Config();
    GPIO_Init();
    USART_Init(USART3);

    // Initialize timers with proper prescalers
    TIM_Init(tim_delay, 84);   // APB1: 84 MHz → 1 MHz (1µs ticks)
    TIM_Init(tim_hcsr04, 168); // APB2: 168 MHz → 1 MHz (1µs ticks)

    enableTIM(tim_servo);       // TIM3 for servo (configured in configServoControlSignal)
    configServoControlSignal(); // This configures TIM3 with PSC=83 (correct for APB1)
    setup_sensor_timer();       // This configures TIM4

    usart_puts("Starting servo test");
    char distance_buf[64];

    for (;;) {
        /* usart_puts("Sweeping LEFT to RIGHT"); */
        for (uint16_t pos = SERVO_LEFT; pos <= SERVO_RIGHT; pos += 10) {
            moveServo(pos);
            delay_ms(20);
            // Print distance if new reading available
            if (distance_ready) {
                distance_ready = false;
                float distance_cm = (float)last_distance / 58.0f;
                int dist10 = (int)(distance_cm * 10.0f + 0.5f);
                snprintf(distance_buf, sizeof(distance_buf), "Distance: %d.%d cm", dist10 / 10, dist10 % 10);
                usart_puts(distance_buf);
            }
        }

        /* usart_puts("Reached RIGHT"); */

        /* usart_puts("Sweeping RIGHT to LEFT"); */
        for (uint16_t pos = SERVO_RIGHT; pos >= SERVO_LEFT; pos -= 10) {
            moveServo(pos);
            delay_ms(20);
            if (distance_ready) {
                distance_ready = false;
                float distance_cm = (float)last_distance / 58.0f;
                int dist10 = (int)(distance_cm * 10.0f + 0.5f);
                snprintf(distance_buf, sizeof(distance_buf), "Distance: %d.%d cm", dist10 / 10, dist10 % 10);
                usart_puts(distance_buf);
            }
        }
        /* usart_puts("Reached LEFT"); */
    }
}

void enableTIM(TIM_TypeDef *TIMx) {
    if (TIMx == TIM1)
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    else if (TIMx == TIM2)
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    else if (TIMx == TIM3)
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    else if (TIMx == TIM4)
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    else if (TIMx == TIM5)
        RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    TIMx->CR1 &= ~TIM_CR1_CEN;
};

void configServoControlSignal() {

    servo_control_signal.port->MODER &= ~GPIO_MODER_MODE6_Msk;
    servo_control_signal.port->MODER |= 0b10 << GPIO_MODER_MODE6_Pos;

    servo_control_signal.port->OTYPER &= ~GPIO_OTYPER_OT6;
    servo_control_signal.port->OSPEEDR &= ~(0b11 << GPIO_OSPEEDR_OSPEED6_Pos);
    servo_control_signal.port->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED6_Pos);
    servo_control_signal.port->PUPDR &= ~(0b11 << GPIO_PUPDR_PUPD6_Pos);

    servo_control_signal.port->AFR[0] &= ~(GPIO_AFRL_AFSEL6_Msk);
    servo_control_signal.port->AFR[0] |= (0b10 << GPIO_AFRL_AFSEL6_Pos);

    tim_servo->PSC = 83;        // this sets each tick to 1us as it's on the APB1 bus with 84MHz
    tim_servo->ARR = 20000 - 1; // number of ticks correspondent to 20ms
    tim_servo->CCR1 = 1000;
    tim_servo->CCMR1 &= ~TIM_CCMR1_OC1M;                     // clear bits
    tim_servo->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // OC1M = 110 -> PWM mode 1
    tim_servo->CCMR1 |= TIM_CCMR1_OC1PE;                     // enable preload
    tim_servo->CCER |= TIM_CCER_CC1E;
    tim_servo->CR1 |= TIM_CR1_ARPE;
    tim_servo->EGR = TIM_EGR_UG;
    tim_servo->CR1 |= TIM_CR1_CEN;
}

void moveServo(uint16_t direction) {
    tim_servo->CCR1 = direction;
}

void TIM4_IRQHandler(void) {
    if (TIM4->SR & TIM_SR_UIF) {
        TIM4->SR &= ~TIM_SR_UIF;

        // read sensor
        uint32_t duration = HCSR04_Read();
        if (duration > 0) {
            last_distance = duration;
            distance_ready = true;
        }
    }
}

void setup_sensor_timer(void) {
    // Enable TIM4 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // Configure for 100ms intervals (10Hz sensor reading)
    TIM4->PSC = 8400 - 1;       // 84MHz/8400 = 10kHz
    TIM4->ARR = 1000 - 1;       // 10kHz/1000 = 10Hz (100ms)
    TIM4->DIER |= TIM_DIER_UIE; // Enable update interrupt
    TIM4->CR1 |= TIM_CR1_CEN;   // Start timer

    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 2);
}

void SystemClock_Config(void) {
    // Enable HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // Config Flash latency
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;

    // Configure PLL
    RCC->PLLCFGR = (7 << 24) | (0 << 16) | (336 << 6) | (8 << 0) | RCC_PLLCFGR_PLLSRC_HSE;

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // Configure prescalers
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB = 168MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4; // APB1 = 42MHz
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; // APB2 = 84MHz

    // Select PLL as system clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    SystemCoreClock = 168000000;
}
