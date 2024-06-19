#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "rp2040/rp2040.h"
#include "rp2040/clocks.h"
#include "rp2040/resets/resets.h"
#include "rp2040/sio/sio.h"
#include "rp2040/gpio/gpio.h"
#include "rp2040/uart/uart.h"
#include "bsp/clock.h"


static void delay(unsigned long ticks) {
    for (volatile unsigned long i = 0; i < ticks; i++)
        ;
}


unsigned long end = 0;


int main(void) {
    uint32_t clock_freq = bsp_clock_init();

    resets_clear_and_wait(RP2040_RESETS_BIT_IO_BANK0);

    // set the function select to SIO (software controlled I/O)
    *RP2040_GPIO_CTRL(RP2040_GPIO_25) = RP2040_GPIO_FUNC_SIO;

    // output enable
    *RP2040_SIO_GPIO_OE_SET = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);

    uart_init(RP2040_UART_0, RP2040_GPIO_0, RP2040_GPIO_1);
    rp2040_uart_config_t config = {
        .baudrate           = 115200,
        .clk_peri_frequency = clock_freq,
        .data_bits          = RP2040_UART_DATA_BITS_8,
        .stop_bits          = 1,
    };
    uart_config(RP2040_UART_0, config);

    for (;;) {
        // turn on the led
        *RP2040_SIO_GPIO_OUT_SET = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
        delay(100000);
        //  turn off the led
        *RP2040_SIO_GPIO_OUT_CLR = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
        delay(100000);

        uart_write(RP2040_UART_0, (uint8_t *)"Hello\n\r", 6);

        printf("I'm printf %i\n\r", 42);
        assert(1);
    }

    return 0;
}
