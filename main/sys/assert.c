#include <stdio.h>
#include "rp2040/uart/uart.h"


void __assert_func(const char *file_name, int line, const char *function_name, const char *error) {
    printf("Assert failed at %s:%i, while executing %s:\n\r%s\n\r", file_name, line, function_name, error);
    for (;;) {}
}
