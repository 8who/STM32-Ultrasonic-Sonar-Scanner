#include "pins.h"

TIM_TypeDef *tim_hcsr04 = TIM1;
TIM_TypeDef *tim_delay = TIM2;
TIM_TypeDef *tim_servo = TIM3;

GPIO_PIN_t trigPin = { GPIOD, 9 };
GPIO_PIN_t echoPin = { GPIOD, 10 };
GPIO_PIN_t greenLed = { GPIOD, 12 };

// servo control pin
GPIO_PIN_t servo_control_signal = { GPIOA, 6 };
