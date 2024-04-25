#include "gpio.h"
#include "../rp2040.h"


void gpio_set_sio(rp2040_gpio_t gpio) {
    *RP2040_GPIO_CTRL(gpio) = RP2040_GPIO_FUNC_SIO;
}
