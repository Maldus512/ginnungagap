#ifndef RP2040_COMMON_H_INCLUDED
#define RP2040_COMMON_H_INCLUDED


#include <stdint.h>


#define MMIO32(Addr)                  ((volatile uint32_t *)(Addr))
#define MASKSHIFT(Value, Mask, Shift) (((Value) & (Mask)) << (Shift))


#endif
