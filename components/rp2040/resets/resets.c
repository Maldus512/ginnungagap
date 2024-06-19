#include "resets.h"
#include "../rp2040.h"


void resets_clear_and_wait(uint32_t mask) {
    *RP2040_CLEAR(RP2040_RESETS_RESET) = mask;

    // wait for reset to be done
    while ((*RP2040_RESETS_RESET_DONE & mask) == 0)
        ;

}
