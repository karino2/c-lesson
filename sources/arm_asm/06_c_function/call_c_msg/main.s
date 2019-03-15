.globl _start
_start:
    ldr r13,=0x07FFFFFF
    // TODO: ここに何かを書いて、下のprint_msgで
    // Hello Worldと表示するようにしてください。
    // 
    bl print_msg
loop:
    b loop
msg:
    .asciz "Hello World\n"
