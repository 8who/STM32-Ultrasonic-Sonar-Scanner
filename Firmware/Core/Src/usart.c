#include "usart.h"
#include "pins.h"

static volatile uint8_t tx_buf[UART_BUF_SIZE];
static volatile uint8_t rx_buf[UART_BUF_SIZE];
static volatile uint16_t tx_head = 0, tx_tail = 0;
static volatile uint16_t rx_head = 0, rx_tail = 0;
GPIO_PIN_t green_led = { GPIOD, 12 };

void USART_Init(USART_TypeDef *usart) {
    uint32_t pclk1 = 42000000U; // APB1 timer clock (PCLK1)
    uint32_t baud = 115200U;

    if (usart == USART2) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    } else if (usart == USART3) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    }

    // Configure PB10/PB11 as AF7 (USART3)
    GPIOB->MODER &= ~((3U << (10 * 2)) | (3U << (11 * 2)));
    GPIOB->MODER |= ((2U << (10 * 2)) | (2U << (11 * 2)));
    GPIOB->OTYPER &= ~((1 << 10) | (1 << 11));             // push-pull
    GPIOB->OSPEEDR |= (2U << (10 * 2)) | (2U << (11 * 2)); // medium/high speed
    GPIOB->PUPDR &= ~(0xF << (10 * 2));

    // AF7 for PB10/PB11
    GPIOB->AFR[1] &= ~((0xF << (4 * (10 - 8))) | (0xF << (4 * (11 - 8))));
    GPIOB->AFR[1] |= ((7U << (4 * (10 - 8))) | (7U << (4 * (11 - 8))));

    // Configure baudrate (integer math)
    // USARTDIV = pclk1 / (16 * baud)
    /* uint32_t usartdiv_times16 = (pclk1 + (baud/2)) / baud; // approx pclk1/baud */
    // better full calc:
    uint32_t mantissa = pclk1 / (16U * baud);
    uint32_t fraction = (uint32_t)(((pclk1 * 1U) / baud) - (mantissa * 16U));
    usart->BRR = (mantissa << 4) | (fraction & 0xF);

    NVIC_EnableIRQ(USART3_IRQn);

    // Enable USART, TX and RX, and RXNE interrupt
    usart->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}

void USART3_IRQHandler(void) {
    GPIO_Write(green_led, true);
    uint32_t sr = USART3->SR;

    // RX handling
    if (sr & USART_SR_RXNE) {
        uint8_t c = (uint8_t)USART3->DR;
        uint16_t next = (rx_head + 1) % UART_BUF_SIZE;
        if (next != rx_tail) {
            rx_buf[rx_head] = c;
            rx_head = next;
        }
    }

    // TX handling
    if (sr & USART_SR_TXE) {
        if (tx_head != tx_tail) {
            USART3->DR = tx_buf[tx_tail];
            tx_tail = (tx_tail + 1) % UART_BUF_SIZE;
        } else {
            USART3->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}

int usart_putchar(int c) {
    uint16_t next = (tx_head + 1) % UART_BUF_SIZE;
    while (next == tx_tail); // wait if buffer full
    tx_buf[tx_head] = (uint8_t)c;
    tx_head = next;
    USART3->CR1 |= USART_CR1_TXEIE; // enable TXE interrupt
    return c;
}

int usart_puts(const char *s) {
    while (*s) {
        usart_putchar(*s++);
    }
    usart_putchar('\r');
    usart_putchar('\n');
    return 1;
}

void usart_puthex(uint32_t value) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11];
    buffer[0] = '0';
    buffer[1] = 'x';
    for (int i = 0; i < 8; i++) {
        buffer[9 - i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    buffer[10] = '\0';
    usart_puts(buffer);
}

int getchar(void) {
    while (rx_head == rx_tail);
    uint8_t c = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % UART_BUF_SIZE;
    return c;
}
