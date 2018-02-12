all: test.hexdump

clean:
	rm -f test.o test.bin test.hexdump test.out test.map

test.o: test.s
	~/Code/riscv/bin/riscv64-unknown-elf-as -o $@ $<

test.out : test.o
	riscv64-unknown-elf-ld -o $@  $<

test.map : test.o
	riscv64-unknown-elf-ld -Map=$@ $<

test.bin: test.out
	riscv64-unknown-elf-objcopy -O binary $< $@

test.hexdump: test.bin
	xxd -e $< > $@

