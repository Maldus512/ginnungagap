#ifndef RP2040_SIO_H_INCLUDED
#define RP2040_SIO_H_INCLUDED


#include "../_common.h"
#include "../gpio/gpio.h"


#define RP2040_SIO_BASE         (0xD0000000)
#define RP2040_SIO_GPIO_OUT     MMIO32(RP2040_SIO_BASE + 0x10)
#define RP2040_SIO_GPIO_OUT_SET MMIO32(RP2040_SIO_BASE + 0x14)
#define RP2040_SIO_GPIO_OUT_CLR MMIO32(RP2040_SIO_BASE + 0x18)
#define RP2040_SIO_GPIO_OUT_XOR MMIO32(RP2040_SIO_BASE + 0x1c)
#define RP2040_SIO_GPIO_OE      MMIO32(RP2040_SIO_BASE + 0x20)
#define RP2040_SIO_GPIO_OE_SET  MMIO32(RP2040_SIO_BASE + 0x24)
#define RP2040_SIO_GPIO_OE_CLR  MMIO32(RP2040_SIO_BASE + 0x28)
#define RP2040_SIO_GPIO_OE_XOR  MMIO32(RP2040_SIO_BASE + 0x2c)

#define RP2040_SIO_GPIO_BIT(Gpio) (1 << (Gpio))


void rp2040_sio_set_gpio_mode(rp2040_gpio_t gpio, rp2040_gpio_mode_t mode);
void rp2040_sio_set_gpio_level(rp2040_gpio_t gpio, uint8_t level);


#endif
