#ifndef RP2040_UART_H_INCLUDED
#define RP2040_UART_H_INCLUDED


#include <stdlib.h>
#include "../_common.h"
#include "../gpio/gpio.h"
#include "../rp2040.h"


#define RP2040_UART_BASE(Uart)      (0x40034000 + 0x4000 * (Uart))
#define RP2040_UART_UARTDR(Uart)    MMIO32(RP2040_UART_BASE(Uart))
#define RP2040_UART_UARTFR(Uart)    MMIO32(RP2040_UART_BASE(Uart) + 0x18)
#define RP2040_UART_UARTIBRD(Uart)  MMIO32(RP2040_UART_BASE(Uart) + 0x24)
#define RP2040_UART_UARTFBRD(Uart)  MMIO32(RP2040_UART_BASE(Uart) + 0x28)
#define RP2040_UART_UARTLCR_H(Uart) MMIO32(RP2040_UART_BASE(Uart) + 0x2C)
#define RP2040_UART_UARTCR(Uart)    MMIO32(RP2040_UART_BASE(Uart) + 0x30)

#define RP2040_UART_UARTLCR_H_STP2_BIT         (1 << 3)
#define RP2040_UART_UARTLCR_H_FEN_BIT          (1 << 4)
#define RP2040_UART_UARTLCR_H_WLEN_BITS(Value) MASKSHIFT((Value), 0x3, 5)

#define RP2040_UART_UARTCR_UARTEN_BIT (1 << 0)
#define RP2040_UART_UARTCR_TXE_BIT    (1 << 8)
#define RP2040_UART_UARTCR_RXE_BIT    (1 << 9)

#define RP2040_UART_UARTFR_TXFF_BIT (1 << 5)


typedef enum {
    RP2040_UART_0 = 0,
    RP2040_UART_1,
#define RP2040_UART_NUM 2
} rp2040_uart_t;


typedef enum {
    RP2040_UART_DATA_BITS_5 = 0,
    RP2040_UART_DATA_BITS_6,
    RP2040_UART_DATA_BITS_7,
    RP2040_UART_DATA_BITS_8,
} rp2040_uart_data_bits_t;


typedef struct {
    uint32_t                clk_peri_frequency;
    uint32_t                baudrate;
    rp2040_uart_data_bits_t data_bits;
    uint8_t                 stop_bits;
} rp2040_uart_config_t;


void uart_init(rp2040_uart_t uart, rp2040_gpio_t tx, rp2040_gpio_t rx);
void uart_config(rp2040_uart_t uart, rp2040_uart_config_t config);
void uart_write(rp2040_uart_t uart, uint8_t *data, size_t len);


#endif
