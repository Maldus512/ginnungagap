#include <stdlib.h>
#include "rp2040/uart/uart.h"


_ssize_t _write(int file, const void *ptr, size_t len) {
    (void)file;
    uart_write(RP2040_UART_0, (uint8_t*)ptr, len);
    return len;
}
