#ifndef RP2040_RESETS_H_INCLUDED
#define RP2040_RESETS_H_INCLUDED


#include "../_common.h"


#define RP2040_RESETS_BASE       (0x4000C000)
#define RP2040_RESETS_RESET      MMIO32(RP2040_RESETS_BASE)
#define RP2040_RESETS_RESET_DONE MMIO32(RP2040_RESETS_BASE + 0x8)

#define RP2040_RESETS_BIT_IO_BANK0 (1 << 5)
#define RP2040_RESETS_BIT_UART0    (1 << 22)
#define RP2040_RESETS_BIT_UART1    (1 << 23)


void resets_clear_and_wait(uint32_t mask);


#endif
