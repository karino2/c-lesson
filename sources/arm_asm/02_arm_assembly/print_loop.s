/*

 arm-none-eabi-as print_loop.s -o print_loop.o
 arm-none-eabi-ld print_loop.o -Ttext 0x00010000 -o print_loop.elf
 arm-none-eabi-objcopy print_loop.elf -O binary print_loop.bin
 qemu-system-arm -M versatilepb -m 128M -nographic -kernel print_loop.bin -serial mon:stdio


*/
.globl _start
_start:
    ldr r0,=0x101f1000
    ldr r1,=message
    ldr r3,[r1]    
loop:
    str r3,[r0]
    add r1, r1, #1
    ldr r3,[r1]
    cmp r3,#0
    bne loop

message:
    .asciz  "hello, world\n"
