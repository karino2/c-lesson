/*
 arm-none-eabi-gcc hello.c -nostdlib -o hello.o
 arm-none-eabi-as hello_c.s -o hello_c.o
 arm-none-eabi-ld hello_c.o hello.o -N -Ttext 0x00010000 -o hello_c.elf
 arm-none-eabi-objcopy hello_c.elf -O binary hello_c.bin
 qemu-system-arm -M versatilepb -m 128M -nographic -kernel hello_c.bin -serial mon:stdio

*/
int hello_c() {
    *(volatile char *)0x101f1000 = 'H';
    *(volatile char *)0x101f1000 = 'e';
    *(volatile char *)0x101f1000 = 'l';
    *(volatile char *)0x101f1000 = 'l';
    *(volatile char *)0x101f1000 = 'o';
    *(volatile char *)0x101f1000 = ' ';
    *(volatile char *)0x101f1000 = 'W';
    *(volatile char *)0x101f1000 = 'o';
    *(volatile char *)0x101f1000 = 'r';
    *(volatile char *)0x101f1000 = 'l';
    *(volatile char *)0x101f1000 = 'd';
    *(volatile char *)0x101f1000 = '!';
    *(volatile char *)0x101f1000 = '\n';

    return 1;    
}

