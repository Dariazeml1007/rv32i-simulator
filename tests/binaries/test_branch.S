
.global _start

_start:
    li   x10, 10
    li   x11, 10
    li   x12, 0

    beq  x10, x11, taken
    ecall

taken:
    li   x12, 1
    ecall

