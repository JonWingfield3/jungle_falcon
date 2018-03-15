BUILD_DIR = build
CC = /usr/local/riscv/bin/riscv64-unknown-elf-gcc
LD = /usr/local/riscv/bin/riscv64-unknown-elf-ld
OBJCOPY = /usr/local/riscv/bin/riscv64-unknown-elf-objcopy
OBJDUMP = /usr/local/riscv/bin/riscv64-unknown-elf-objdump

#CC_FLAGS = -nostartfiles -nostdlib --save-temps -mabi=ilp32 -march=rv32i
CC_FLAGS = -nostdlib --save-temps -mabi=ilp32 -march=rv32i

#LD_FLAGS = -Wl,-Ttext-segment,0, Wl,-e_start -T riscv.ld
LD_FLAGS = -T riscv.lds

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
	rm -f *.o *.i *.out

.PHONY : clean
clean :
	rm -f ./$(BUILD_DIR)/*

%.out: %.c
	$(CC) -o $(BUILD_DIR)/$@ $< start.s $(LD_FLAGS) $(CC_FLAGS) 

%.bin : %.out
	python3 linker.py $(BUILD_DIR)/$<

%.hex : %.bin
	xxd -e $(BUILD_DIR)/$< > $(BUILD_DIR)/$@

%.objdump : %.out
	$(OBJDUMP) $(BUILD_DIR)/$< -t -d -r > $(BUILD_DIR)/$@

%.disasm : %.out
	$(OBJDUMP) $(BUILD_DIR)/$< -D > $(BUILD_DIR)/$@

