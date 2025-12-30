#include "gpio.h"

void GPIO_Init(void) {
    GPIO_EnablePortA();
    GPIO_EnablePortB();
    GPIO_EnablePortD();
    GPIO_EnablePortE();

    // TRIG PD9 as output (push-pull), ensure LOW
    GPIOD->MODER &= ~(GPIO_MODER_MODER9_Msk);
    GPIOD->MODER |= GPIO_MODER_MODER9_0;
    GPIOD->OTYPER &= ~(GPIO_OTYPER_OT9_Msk);
    GPIOD->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR9);
    GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPDR9);
    GPIOD->BSRR = (1 << (9 + 16));

    // ECHO PD10 as input with pull-down
    GPIOD->MODER &= ~(GPIO_MODER_MODER10_Msk);
    GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPDR10);
    /* GPIOD->PUPDR |= GPIO_PUPDR_PUPDR10_1; // enable internal pull-down */

    // PE0-PE3 data pins for LCD as output
    /* GPIOE->MODER &= ~(GPIO_MODER_MODER0_Msk | */
    /*                   GPIO_MODER_MODER1_Msk | */
    /*                   GPIO_MODER_MODER2_Msk | */
    /*                   GPIO_MODER_MODER3_Msk); */
    /**/
    /* GPIOE->MODER |= (GPIO_MODER_MODER0_0 | */
    /*                  GPIO_MODER_MODER1_0 | */
    /*                  GPIO_MODER_MODER2_0 | */
    /*                  GPIO_MODER_MODER3_0); */

    // PE4(RS), PE5(E)
    /* GPIOE->MODER &= ~(GPIO_MODER_MODER4_Msk | GPIO_MODER_MODER5_Msk); */
    /* GPIOE->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0); */

    // LED PD12 as output
    /* GPIOD->MODER &= ~(GPIO_MODER_MODER12_Msk); */
    /* GPIOD->MODER |= GPIO_MODER_MODER12_0; */
}

void GPIO_Write(GPIO_PIN_t pin, bool state) {
    if (state) {
        pin.port->ODR |= (1U << pin.pin);
    } else {
        pin.port->ODR &= ~(1U << (pin.pin));
    }
}

void GPIO_EnablePortA() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
}

void GPIO_EnablePortB(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
}

void GPIO_EnablePortD() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
}

void GPIO_EnablePortE() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
}

void nop_delay() {
    for (volatile int i = 0; i < 1000000; i++);
}
