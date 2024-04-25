#ifndef RP2040_H_INCLUDED
#define RP2040_H_INCLUDED


#include "_common.h"


#define RP2040_XOR(Reg)   (MMIO32(((uint32_t)(uintptr_t)(Reg)) + 0x1000UL))
#define RP2040_SET(Reg)   (MMIO32(((uint32_t)(uintptr_t)(Reg)) + 0x2000UL))
#define RP2040_CLEAR(Reg) (MMIO32(((uint32_t)(uintptr_t)(Reg)) + 0x3000UL))


#endif
