.globl _start
_start:

  ldr r0,=msg1
  b print
  // never back to this line!
  ldr r0,=msg2
  b print

print:
  ldrb r3,[r0]
  ldr r1,=0x101f1000
loop:
  str r3,[r0]
  add r0, r0, #1
  ldrb r3,[r0]
  cmp r3,#0
  bne loop
end:
  b end

msg1: .asciz "First text.\n"
msg2: .asciz "Second text!\n"