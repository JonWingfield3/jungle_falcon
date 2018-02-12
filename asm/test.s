.section .text
.globl _start
_start:
	sra x7, x22, x8
    lwu x19, -37(x9)
    lui x17, 0x8437b
    jalr x4, -216(x20)
