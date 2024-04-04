#include "gpio.h"
#include "uart.h"

#define UART ((NTF_UART_REG*)0x40002000)

typedef struct {
    volatile uint32_t TASKS_STARTRX;
    volatile uint32_t TASKS_STOPRX;
    volatile uint32_t TASKS_STARTTX;
    volatile uint32_t TASKS_STOPTX;
    volatile uint32_t RESERVED0[62];
    volatile uint32_t EVENTS_RXDRDY;
    volatile uint32_t RESERVED1[4];
    volatile uint32_t EVENTS_TXDRDY;
    volatile uint32_t RESERVED2[248];
    volatile uint32_t ENABLE;
    volatile uint32_t RESERVED3;
    volatile uint32_t PSELRTS;
    volatile uint32_t PSELTXD;
    volatile uint32_t PSELCTS;
    volatile uint32_t PSELRXD;
    volatile uint32_t RXD;
    volatile uint32_t TXD;
    volatile uint32_t RESERVED4;
    volatile uint32_t BAUDRATE;
} NTF_UART_REG ;

void uart_init() {
    GPIO->PIN_CNF[6] = 1; //Transmit TXD
    GPIO->PIN_CNF[8] = 0; //Reciecve RXD
    UART->PSELTXD = (1 << 6); //0x06
    UART->PSELRXD = (1 << 8); //0x28
    UART->PSELRTS = 0xFFFFFFFF;
    UART->PSELCTS = 0xFFFFFFFF;
    UART->BAUDRATE = 0x00275000;
    UART->ENABLE = 4;
    UART->TASKS_STARTRX = 1;
}