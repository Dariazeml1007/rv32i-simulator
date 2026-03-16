# tests/binaries/test_add_qemu_2.S
.global _start

_start:
    li a0, 5
    li a1, 3
    add a0, a0, a1


    li a7, 93
    ecall
