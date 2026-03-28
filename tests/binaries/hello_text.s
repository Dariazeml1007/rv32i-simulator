.globl _start
_start:
    li a0, 1
    la a1, msg
    li a2, 14
    li a7, 64
    ecall

    li a0, 0
    li a7, 93
    ecall

msg:
    .string "Hello, World!\n"
