#include "rp2040/rp2040.h"
#include "rp2040/resets/resets.h"
#include "rp2040/sio/sio.h"
#include "rp2040/gpio/gpio.h"
#include "rp2040/ssi/ssi.h"

static void delay(unsigned long ticks) {
    for (volatile unsigned long i = 0; i < ticks; i++)
        ;
}

void crt0(void) {
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
        delay(0x100000);
    }
}


void configure_ssi(void) {
    // Disable SSI to allow further config
    *RP2040_SSI_SSIENR = 0;

    // Configure CTRL0
    *RP2040_SSI_CTRLR0 =
        RP2040_SSI_CTRLR0_DFS_32_BITS(31) | RP2040_SSI_CTRLR0_TMOD_BITS(RP2040_SSI_CTRLR0_TMOD_EEPROM_READ);

    // Set baud rate
    *RP2040_SSI_BAUDR = 8;

    *RP2040_SSI_SPI_CTRL0 = RP2040_SSI_SPI_CTRLR0_ADDR_L_BITS(6) |
                            RP2040_SSI_SPI_CTRLR0_INST_L_BITS(RP2040_SSI_SPI_CTRLR0_INST_L_8_BITS) |
                            RP2040_SSI_SPI_CTRLR0_XIP_CMD_BITS(0x02);

    *RP2040_SSI_CTRLR1 = 0;

    // Enable SSI
    *RP2040_SSI_SSIENR = 1;
}
