/*
.type _entry_point,%function
.section .debugger_entry_point, "ax"
.thumb_func
.global _debugger_entry_point
_debugger_entry_point:
    movs r0, #0
    str r0, [r1]
    ldmia r0!, {r1, r2}
    msr msp, r1
    bx r2
    */
