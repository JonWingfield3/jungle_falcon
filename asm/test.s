# Section .crt0 is always placed from address 0
	.section .crt0, "x"

#	%assign MISA 769

#Symbol start is used to obtain entry point information
_start:
	.global _start

	addi x5, x0, 1445
	sw x5, 1500(x0)
	nop
	nop
	nop
	lw x6, 1500(x0)
	nop
	nop
	nop
	nop
	nop
	lui x9, 678490
#	ori x10, x10, 23130
	or x9, x9, x10

#	csrrw 769, x4, x9
	li x5, 255
#	csrrsi 769, x4, 31
#	csrrci 769, x4, 21
#	csrrsi 769, x4, 21
#	csrrci 769, x4, 10
#	csrrwi 769, x4, 00
#	csrrwi 769, x4, 31

	not x9, x9
	andi x10, x0, 0
#	ori x10, x10, 2650
	slli x10, x10, 20
	srli x10, x10, 20
	li x10, 0
	li x11, 4
loop:
	addi x10, x10, 1
	ble x10, x11, loop
	li x12, 100
	li x12, 200
	li x12, 300
	mv x13, x12
	mv x14, x12
	li x12, 1
	blez x12, FAIL
	bltz x12, FAIL
	bgez x12, BGEZ1
	nop
	nop
	beqz x0, FAIL
BGEZ1:
	bgtz x12, BGTZ1
	nop
	nop
	nop
	nop
	beqz x0, FAIL
BGTZ1:
	seqz x10, x12
	snez x11, x12
	sgtz x9, x12
	sltz x8, x12
	neg x12, x12
	addi x12, x12, 1
	bgtz x12, FAIL
	sgtz x9, x12
	sltz x8, x12
	li x11, 256
	li x10, 256
	seqz x10, x12
	snez x11, x12
	li x12, -1
	seqz x10, x12
	snez x11, x12
	sgtz x9, x12
	sltz x8, x12
	beqz x12, FAIL
	bgez x12, FAIL
	bgtz x12, FAIL
	bnez x12, BNEZ1
	nop
	nop
	nop
	beqz x0, FAIL
BNEZ1:
	blez x12, BLEZ1
	nop
	nop
	beqz x0, FAIL
BLEZ1:
	bltz x12, BLTZ1
	nop
	nop
	nop
	nop
	beqz x0, FAIL
BLTZ1:
	addi x12, x12, 1
	bnez x12, FAIL
	bltz x12, FAIL
	blez x12, BLEZ2
	nop
	nop
	beqz x0, FAIL
BLEZ2:
	beqz x12, BEQZ1
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	beqz x0, FAIL
BEQZ1:
	bgez x12, BGEZ2
	nop
	nop
	beqz x0, FAIL
BGEZ2:
	nop
	nop
	nop
	nop
	li x12, -1
	li x13, 0
	li x14, 1
	bgt x12, x13, FAIL
	bgt x12, x14, FAIL
	bgt x13, x14, FAIL
	bgt x14, x14, FAIL
	ble x14, x12, FAIL
	ble x14, x13, FAIL
	ble x13, x12, FAIL
	bgtu x13, x12, FAIL
	bgtu x14, x12, FAIL
	bgtu x13, x14, FAIL
	bleu x12, x13, FAIL
	bleu x12, x14, FAIL
	bleu x14, x13, FAIL
	bgt x14, x13, BGT1
	nop
	nop
	beqz x0, FAIL
BGT1:
	bgt x14, x12, BGT2
	nop
	nop
	beqz x0, FAIL
BGT2:
	bgt x13, x12, BGT3
	nop
	nop
	beqz x0, FAIL
BGT3:
	ble x12, x13, BLE1
	nop
	nop
	beqz x0, FAIL
BLE1:
	ble x12, x14, BLE2
	nop
	nop
	beqz x0, FAIL
BLE2:
	ble x13, x14, BLE3
	nop
	nop
	beqz x0, FAIL
BLE3:
	ble x12, x12, BLE4
	nop
	nop
	beqz x0, FAIL
BLE4:
	bgtu x14, x13, BGTU1
	nop
	nop
	beqz x0, FAIL
BGTU1:
	bgtu x12, x13, BGTU2
	nop
	nop
	beqz x0, FAIL
BGTU2:
	bgtu x12, x14, BGTU3
	nop
	nop
	beqz x0, FAIL
BGTU3:
	bleu x13, x14, BLEU1
	nop
	nop
	beqz x0, FAIL
BLEU1:
	bleu x14, x12, BLEU2
	nop
	nop
	beqz x0, FAIL
BLEU2:
	bleu x12, x12, BLEU3
	nop
	nop
	beqz x0, FAIL
BLEU3:
	li x13, 1600
#	li x14, JUMPREG
	sw x14, 1500(x0)
	lw x15, 1500(x0)
	jal x1, JUMPREG
	nop
	nop
	beqz x0, FAIL
	nop
	nop
	nop
	nop
JUMPREG:
	addi x13, x13, -8
	sw x1, 4(x13)
	slt x1, x13, x0
	add x1, x1, x16
	j JUMP2
	nop
	nop
	beqz x0, FAIL
	nop
	nop
JUMP2:
	li x12, 0
	call TEST_ROUTINE
	addi x12, x12, 1
	addi x12, x12, 1
#	li x15, TEST_ROUTINE2
#	call_reg x15
	addi x12, x12, 1
	addi x12, x12, 1
	sw x12, 1500(x0)
	nop
	lw x13, 1500(x0)
	nop
	jal x1, ROUTINE
	addi x14, x0, 30
	addi x15, x0, 5
	sub x16, x14, x15
	nop
	sub x16, x16, x15
	add x16, x16, x15
	nop
	nop
	add x16, x15, x16
	lb x15, 1601(x0)
	add x16, x16, x15
	lb x15, 1600(x0)
	add x16, x15, x16
	nop
	nop
	lui x10, 3939
	srli x10, x10, 12
	sh	x10, 1616(x0)
	lui x10, 9393
	srli x10, x10, 12
	sh x10, 1618(x0)
	lh x11, 1616(x0)
	lh x12, 1618(x0)
	lw x10, 1600(x0)
	addi x0, x0, 0
	bltu x0, x10, SKIP
	nop
	nop
	#halt
	nop
	nop
	nop
	nop
	nop
SKIP:
	addi x1, x0, 4
	li x7, -100
	sb x7, 1600(x0)
	lb x8, 1600(x0)
	lbu x9, 1600(x0)

	nop
	nop
	nop
#	li x1, FINISH
	nop
	jalr x1, 0(x1)
	nop
	nop
	nop
	#halt
	nop
	nop
	nop
	nop

ROUTINE:
	addi x15, x0, 15
	addi x9, x0, 1
	nop
	sb x9, 1600(x0)
	addi x9, x0, 2
	sb x9, 1601(x0)
	addi x9, x0, 3
	nop
	nop
	sb x9, 1602(x0)
	addi x9, x0, 4
	nop
	nop
	sb x9, 1603(x0)
	lb x10, 1600(x0)
	lb x11, 1601(x0)
	lb x12, 1602(x0)
	lb x13, 1603(x0)
	ret
	nop
	nop
	nop
	nop
	nop
	#halt
	nop
	nop
	nop


TEST_ROUTINE:
	ret
	nop
	nop
	nop

TEST_ROUTINE2:
	ret
	nop
	nop
	nop

FAIL:
	nop
	nop
	nop
	nop
	nop
	#halt
	nop
	nop
	nop
	nop

FINISH:
	nop
	#halt
	nop
	nop
	nop
	nop




