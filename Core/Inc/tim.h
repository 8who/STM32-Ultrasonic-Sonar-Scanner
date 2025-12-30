#include "stm32f407xx.h"
#include <stdbool.h>

#ifndef TIM_H
#define TIM_H

void TIM_Init(TIM_TypeDef *tim, uint32_t prescaler);
void TIM_Enable(TIM_TypeDef *tim, bool enable);
void TIM_Set_Counter(TIM_TypeDef *tim, uint32_t val);

#endif
