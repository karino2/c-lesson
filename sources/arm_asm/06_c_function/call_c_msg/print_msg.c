/*
 arm-none-eabi-gcc print_msg.c -nostdlib -o print_msg.o
 arm-none-eabi-as main.s -o main.o
 arm-none-eabi-ld main.o print_msg.o -N -Ttext 0x00010000 -o print_all.elf
 arm-none-eabi-objcopy print_all.elf -O binary print_all.bin
 qemu-system-arm -M versatilepb -m 128M -nographic -kernel print_all.bin -serial mon:stdio
*/

#define UART ((volatile char *)0x101f1000)

void print_msg(char *s) {
    while(*s) {
        *UART = *s++;
    }
}
