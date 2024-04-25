#include <stdint.h>
#include "rp2040/rp2040.h"
#include "rp2040/resets/resets.h"
#include "rp2040/sio/sio.h"
#include "rp2040/gpio/gpio.h"


static void delay(unsigned long ticks) {
    for (volatile unsigned long i = 0; i < ticks; i++)
        ;
}

int main(void) {
    // release reset on IO_BANK0
    *RP2040_CLEAR(RP2040_RESETS_RESET) = RP2040_RESETS_BIT_IO_BANK0;

    // wait for reset to be done
    while ((*RP2040_RESETS_RESET_DONE & RP2040_RESETS_BIT_IO_BANK0) == 0)
        ;

    // set the function select to SIO (software controlled I/O)
    *RP2040_GPIO_CTRL(RP2040_GPIO_25) = RP2040_GPIO_FUNC_SIO;

    // output enable
    *RP2040_SIO_GPIO_OE_SET = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);

    for (;;) {
        // turn on the led
        *RP2040_SIO_GPIO_OUT_SET = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
        delay(0x10000);
        //  turn off the led
        *RP2040_SIO_GPIO_OUT_CLR = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
        delay(0x10000);
    }

    return 0;
}
