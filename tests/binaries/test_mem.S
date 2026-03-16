ти
.global _start

_start:
    li   x10, 0x1000
    li   x11, 0x12345678

    sw   x11, 0(x10)
    lw   x12, 0(x10)

    ecall
