#include "gpio.h"
#include "stm32f407xx.h"
#ifndef PINS_H
#define PINS_H

extern GPIO_PIN_t trigPin;
extern GPIO_PIN_t echoPin;
extern GPIO_PIN_t greenLed;
extern GPIO_PIN_t servo_control_signal;

extern TIM_TypeDef *tim_hcsr04;
extern TIM_TypeDef *tim_delay;
extern TIM_TypeDef *tim_servo;

#endif
