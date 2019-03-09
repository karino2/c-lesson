.globl _start
_start:
    ldr r13,=0x07FFFFFF
    bl hello_c
loop:
    b loop
