// Why doesn't this app work?


#include <stdint.h>
#include "rp2040/rp2040.h"
#include "rp2040/resets/resets.h"
#include "rp2040/sio/sio.h"
#include "rp2040/gpio/gpio.h"


extern void DELAY(unsigned int);


static void delay(unsigned int ticks) {
    for (volatile unsigned long i = 0; i < 0x100000; i++)
        ;
}


int main(void) {
    //  release reset on IO_BANK0
    rp2040_resets_clear_bank0();

    // set the function select to SIO (software controlled I/O)
    gpio_set_sio(RP2040_GPIO_25);

    // output enable
    RP2040_SIO_GPIO_OE_SET = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);

    for (;;) {
        // turn on the led
        RP2040_SIO_GPIO_OUT_SET = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
        for (unsigned long i = 0; i < 0x100000; i++)
            ;
        //  turn off the led
        RP2040_SIO_GPIO_OUT_CLR = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
        for (unsigned long i = 0; i < 0x100000; i++)
            ;
    }

    return (0);
}
