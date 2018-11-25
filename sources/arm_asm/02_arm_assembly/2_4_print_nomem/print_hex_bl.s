.globl _start
_start:

  mov r0, r15
  bl print_hex

  mov r0, #0x68
  bl print_hex

end:
  b end


print_hex:
  // TODO: write here!
