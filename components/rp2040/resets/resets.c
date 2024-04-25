#include "resets.h"
#include "../rp2040.h"


void rp2040_resets_clear_bank0(void) {
    // release reset on IO_BANK0
    *RP2040_CLEAR(RP2040_RESETS_RESET) = RP2040_RESETS_BIT_IO_BANK0;

    // wait for reset to be done
    while ((*RP2040_RESETS_RESET_DONE & RP2040_RESETS_BIT_IO_BANK0) == 0)
        ;
}
