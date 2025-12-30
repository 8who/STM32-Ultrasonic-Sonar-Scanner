#include "tim.h"

void TIM_Init(TIM_TypeDef *tim, uint32_t prescaler) {
    if (tim == TIM1) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    } else if (tim == TIM2) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    }
    tim->CR1 = 0;
    tim->PSC = prescaler - 1;
    if (tim == TIM2) {
        tim->ARR = 0xFFFFFFFF; // TIM2 is 32-bit
    } else {
        tim->ARR = 0xFFFF; // TIM1 is 16-bit
    }
    tim->CNT = 0;
    tim->EGR = TIM_EGR_UG; // update
    tim->CR1 &= ~TIM_CR1_DIR;
    TIM_Enable(tim, false);
}

void TIM_Enable(TIM_TypeDef *tim, bool enable) {
    if (enable)
        tim->CR1 |= TIM_CR1_CEN;
    else
        tim->CR1 &= ~TIM_CR1_CEN;
}

void TIM_Set_Counter(TIM_TypeDef *tim, uint32_t val) {
    tim->CNT = val;
}
