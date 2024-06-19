#include "../resets/resets.h"
#include "../gpio/gpio.h"
#include "../rp2040.h"
#include "uart.h"


#define GET_INTEGER_BAUDRATE_DIVISOR(Clock, Baudrate) ((((Clock) / 16) / (Baudrate)) & 0xFFFF)
#define GET_FRACTIONAL_BAUDRATE_DIVISOR(Clock, Baudrate)                                                               \
    (((((((((Clock)*100) / 16) / (Baudrate)) % 100) * 64) + 50) / 100) & 0x1FF)


void uart_init(rp2040_uart_t uart, rp2040_gpio_t tx, rp2040_gpio_t rx) {
    const uint32_t resets_bit_uart[RP2040_UART_NUM] = {
        RP2040_RESETS_BIT_UART0,
        RP2040_RESETS_BIT_UART1,
    };

    resets_clear_and_wait(resets_bit_uart[uart]);

    *RP2040_GPIO_CTRL(tx) = RP2040_GPIO_FUNC_UART;
    *RP2040_GPIO_CTRL(rx) = RP2040_GPIO_FUNC_UART;
}


int puts(const char *s) {
    while (*s != '\0') {
        uart_write(RP2040_UART_0, (uint8_t *)s, 1);
        s++;
    }
    return 0;
}


void uart_config(rp2040_uart_t uart, rp2040_uart_config_t config) {
    *RP2040_UART_UARTIBRD(uart) = GET_INTEGER_BAUDRATE_DIVISOR(config.clk_peri_frequency, config.baudrate);
    *RP2040_UART_UARTFBRD(uart) = GET_FRACTIONAL_BAUDRATE_DIVISOR(config.clk_peri_frequency, config.baudrate);

    if (config.stop_bits >= 2) {
        *RP2040_SET(RP2040_UART_UARTLCR_H(uart)) = RP2040_UART_UARTLCR_H_STP2_BIT;
    } else {
        *RP2040_CLEAR(RP2040_UART_UARTLCR_H(uart)) = RP2040_UART_UARTLCR_H_STP2_BIT;
    }

    *RP2040_SET(RP2040_UART_UARTLCR_H(uart)) = RP2040_UART_UARTLCR_H_FEN_BIT;

    *RP2040_CLEAR(RP2040_UART_UARTLCR_H(uart)) = RP2040_UART_UARTLCR_H_WLEN_BITS(0x3);
    *RP2040_SET(RP2040_UART_UARTLCR_H(uart))   = RP2040_UART_UARTLCR_H_WLEN_BITS(config.data_bits);

    *RP2040_SET(RP2040_UART_UARTCR(uart)) = RP2040_UART_UARTCR_UARTEN_BIT;
    *RP2040_SET(RP2040_UART_UARTCR(uart)) = RP2040_UART_UARTCR_TXE_BIT;
    //*RP2040_SET(RP2040_UART_UARTCR(uart)) = RP2040_UART_UARTCR_RXE_BIT;
}


void uart_write(rp2040_uart_t uart, uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        while ((*RP2040_UART_UARTFR(uart) & RP2040_UART_UARTFR_TXFF_BIT) != 0) {
            // Busy wait
        }
        *RP2040_UART_UARTDR(uart) = data[i];
    }
}
