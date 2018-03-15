.section .start
.globl _start
.globl main
_start:
  li sp, 65535
  j main 
