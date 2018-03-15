BUILD_DIR = build
CC = /usr/local/riscv/bin/riscv64-unknown-elf-gcc
LD = /usr/local/riscv/bin/riscv64-unknown-elf-ld
OBJCOPY = /usr/local/riscv/bin/riscv64-unknown-elf-objcopy
OBJDUMP = /usr/local/riscv/bin/riscv64-unknown-elf-objdump

#CC_FLAGS = -nostartfiles -nostdlib --save-temps -mabi=ilp32 -march=rv32i
CC_FLAGS = -nostdlib --save-temps -mabi=ilp32 -march=rv32i

#LD_FLAGS = -Wl,-Ttext-segment,0, Wl,-e_start -T riscv.ld
LD_FLAGS = -T riscv.ld 

C_FILES = $(shell ls *.c)

OBJ_FILENAMES = $(C_FILES:.c=.o)
OUT_FILENAMES = $(C_FILES:.c=.out)
BIN_FILENAMES = $(C_FILES:.c=.bin)
HEX_FILENAMES = $(C_FILES:.c=.hex)
OBJDUMP_FILENAMES = $(C_FILES:.c=.objdump)
DISASM_FILENAMES = $(C_FILES:.c=.disasm)
ALL_FILENAMES = $(OBJ_FILENAMES) $(OUT_FILENAMES)\
                $(BIN_FILENAMES) $(HEX_FILENAMES)\
                $(OBJDUMP_FILENAMES) $(DISASM_FILENAMES)

OBJ_FILES = $(addprefix $(BUILD_DIR)/,$(OBJ_FILENAMES))
OUT_FILES = $(addprefix $(BUILD_DIR)/,$(OUT_FILENAMES))
BIN_FILES = $(addprefix $(BUILD_DIR)/,$(BIN_FILENAMES))
HEX_FILES = $(addprefix $(BUILD_DIR)/,$(HEX_FILENAMES))
OBJDUMP_FILES = $(addprefix $(BUILD_DIR)/,$(OBJDUMP_FILENAMES))
DISASM_FILES = $(addprefix $(BUILD_DIR)/,$(DISASM_FILENAMES))
ALL = $(OBJ_FILES) $(OUT_FILES) $(MAP_FILES)\
      $(BIN_FILES) $(HEX_FILES) $(OBJDUMP_FILES)\
      $(DISASM_FILES)

.PHONY : all
all : $(ALL_FILENAMES)
	mv $(ALL_FILENAMES) $(BUILD_DIR)/
	mv *.i *.s  $(BUILD_DIR)/
#	mv $(BUILD_DIR)/start.S .

.PHONY : clean
clean :
	rm -f ./$(BUILD_DIR)/*

%.out: %.c
	$(CC) -o $@ $< start.S $(LD_FLAGS) $(CC_FLAGS) 

#%.out: %.o start.o
#	$(LD) -o $@ $(LD_FLAGS) $^

%.bin : %.out
	python3 linker.py $<

%.hex : %.bin
	xxd -e $< > $@

%.objdump : %.out
	$(OBJDUMP) $< -t -d -r > $@

%.disasm : %.out
	$(OBJDUMP) $< -D > $@

