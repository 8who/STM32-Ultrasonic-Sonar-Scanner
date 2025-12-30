#include "stm32f4xx.h"
#include <stdbool.h>

#ifndef GPIO_H
#define GPIO_H

#define GPIO_PIN_SET true
#define GPIO_PIN_RESET false

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} GPIO_PIN_t;

void GPIO_Init(void);
void GPIO_Write(GPIO_PIN_t pin, bool state);
void GPIO_Mode(GPIO_TypeDef *port, uint16_t pin, uint16_t mode);
void GPIO_EnablePortA();
void GPIO_EnablePortB();
void GPIO_EnablePortD();
void GPIO_EnablePortE();
void nop_delay();

#endif
