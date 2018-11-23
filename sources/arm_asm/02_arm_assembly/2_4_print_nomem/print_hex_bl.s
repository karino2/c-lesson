.globl _start
_start:
	ldr r0,=0x101f1000

	mov r1, r15
	mov r3, #28
	bl print_hex

	mov r1, #0x68
	mov r3, #28
	bl print_hex

busy:
  b busy


# レジスタの値を16進数で表示する
print_hex:
    lsr r2, r1, r3
    and r2, r2, #0x0f
    cmp r2, #0x0a
    blt over_ten
    add r2, r2, #0x07

over_ten:
    add r2, r2, #0x30
    str r2, [r0]

    sub r3, r3, #4

    cmp r3, #0
    bge print_hex
    mov r15, r14

# >>0001000C00000068
# 改行されていないので、0001000C と00000068 が出力
