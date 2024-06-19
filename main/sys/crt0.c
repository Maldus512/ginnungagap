#include <stdint.h>


extern void main(void);
extern int  __stack_top;

extern int __data_init__;
extern int __data_start__;
extern int __data_size__;
extern int __bss_start__;
extern int __bss_size__;


void nvic_nmi_handler(void);
void nvic_hard_fault_handler(void);
void nvic_svcall_handler(void);
void nvic_pendsv_handler(void);
void nvic_systick_handler(void);

static void reset_handler(void);
static void stub_handler(void);
static void invalid_handler(void);


__attribute__((section(".vector_table"))) uint32_t __vector[48] = {
    (uint32_t)&__stack_top,        (uint32_t)&reset_handler,
    (uint32_t)&nvic_nmi_handler,   (uint32_t)&nvic_hard_fault_handler,
    (uint32_t)&invalid_handler,    (uint32_t)&invalid_handler,
    (uint32_t)&invalid_handler,    (uint32_t)&invalid_handler,
    (uint32_t)&invalid_handler,    (uint32_t)&invalid_handler,
    (uint32_t)&invalid_handler,    (uint32_t)&nvic_svcall_handler,
    (uint32_t)&invalid_handler,    (uint32_t)&invalid_handler,
    (uint32_t)nvic_pendsv_handler, (uint32_t)nvic_systick_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
    (uint32_t)stub_handler,        (uint32_t)stub_handler,
};

extern int __bootrom_vector;

#if 1
__attribute__((section(".debugger_entry_point"))) void _debugger_entry_point(void) {
    /*
    volatile uint32_t *bootrom_vector_table_pointer = (volatile uint32_t *)0x0;

    __asm("msr msp, %0" : : "r"(bootrom_vector_table_pointer[0]));
    __asm("bx %0" : : "r"(bootrom_vector_table_pointer[1]));
    */

    //ldr r1, =(PPB_BASE + M0PLUS_VTOR_OFFSET)
    //str r0, [r1]

    __asm("msr msp, %0" : : "r"(((volatile uint32_t *)&__bootrom_vector)[0]));
    __asm("bx %0" : : "r"(((volatile uint32_t *)&__bootrom_vector)[1]));
}
#endif


__attribute__((weak)) void nvic_nmi_handler(void) {
    for (;;) {}
}


__attribute__((weak)) void nvic_hard_fault_handler(void) {
    for (;;) {}
}


__attribute__((weak)) void nvic_svcall_handler(void) {
    for (;;) {}
}


__attribute__((weak)) void nvic_pendsv_handler(void) {
    for (;;) {}
}


__attribute__((weak)) void nvic_systick_handler(void) {
    // Do nothing
}


static void reset_handler(void) {
    volatile uint32_t *data_ptr      = (uint32_t *)&__data_start__;
    volatile uint32_t *data_init_ptr = (uint32_t *)&__data_init__;
    volatile uint32_t  data_size     = (uint32_t)(uintptr_t)&__data_size__;

    for (uint32_t i = 0; i < data_size; i++) {
        data_ptr[i] = data_init_ptr[i];
    }

    volatile uint32_t *bss_ptr  = (uint32_t *)&__bss_start__;
    volatile uint32_t  bss_size = (uint32_t)(uintptr_t)&__bss_size__;

    for (uint32_t i = 0; i < bss_size; i++) {
        bss_ptr[i] = 0;
    }

    main();
    for (;;) {}
}


static void stub_handler(void) {
    // Do nothing
}


static void invalid_handler(void) {
    for (;;) {}
}
