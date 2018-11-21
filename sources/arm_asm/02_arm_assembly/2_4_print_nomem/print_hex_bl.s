.globl _start
_start:

  mov r0, r15
  bl print_hex

  mov r0, #0x68
  bl print_hex

busy:
  b busy


print_hex:
  // TODO: write here!
