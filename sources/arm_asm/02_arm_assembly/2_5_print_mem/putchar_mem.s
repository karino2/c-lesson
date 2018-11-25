.globl _start
_start:

  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end

putchar:
  // TODO: fix here!
  ldr r1,=0x101f1000
  ldr r0, [r1]
  mov r15, r14


print:
  // TODO: Fix this function too.
  ldrb r3,[r0]
_loop:  
  

  add r0, r0, #1
  ldrb r3,[r0]
  cmp r3,#0
  bne _loop
  mov r15, r14

msg1: .asciz "First text.\n"
msg2: .asciz "Second text!\n"