#include "sio.h"
#include "../gpio/gpio.h"

/*

void rp2040_sio_set_gpio_mode(rp2040_gpio_t gpio, rp2040_gpio_mode_t mode) {
    switch (mode) {
        case RP2040_GPIO_MODE_OUTPUT:
            RP2040_SIO_GPIO_OE_SET = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
            break;
        default:
            break;
    }
}

void rp2040_sio_set_gpio_level(rp2040_gpio_t gpio, uint8_t level) {
    if (level) {
        RP2040_SIO_GPIO_OUT_SET = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
    } else {
        RP2040_SIO_GPIO_OUT_CLR = RP2040_SIO_GPIO_BIT(RP2040_GPIO_25);
    }
}
*/
