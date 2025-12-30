#include "gpio.h"
#include "hcsr04.h"
#include "pins.h"
#include "stm32f4xx.h"
#include "tim.h"
#include "usart.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void enableTIM(TIM_TypeDef *TIMx);
void configServoControlSignal();
void moveServo(uint16_t direction);
void SystemClock_Config(void);
void TIM4_IRQHandler(void);
void setup_sensor_timer(void);
