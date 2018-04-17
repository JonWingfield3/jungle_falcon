BUILD_DIR = build

AS = /usr/local/riscv/bin/riscv64-unknown-elf-as
LD = /usr/local/riscv/bin/riscv64-unknown-elf-ld
OBJCOPY = /usr/local/riscv/bin/riscv64-unknown-elf-objcopy
OBJDUMP = /usr/local/riscv/bin/riscv64-unknown-elf-objdump

AS_FLAGS = 
LD_FLAGS = 
ASM_FILES = $(shell ls *.s)

OBJ_FILENAMES = $(ASM_FILES:.s=.o)
OUT_FILENAMES = $(ASM_FILES:.s=.out)
BIN_FILENAMES = $(ASM_FILES:.s=.bin)
HEX_FILENAMES = $(ASM_FILES:.s=.hex)
OBJDUMP_FILENAMES = $(ASM_FILES:.s=.disasm)
ALL_FILENAMES = $(OBJ_FILENAMES) $(OUT_FILENAMES)\
                $(BIN_FILENAMES) $(HEX_FILENAMES)\
                $(OBJDUMP_FILENAMES)

OBJ_FILES = $(addprefix $(BUILD_DIR)/,$(OBJ_FILENAMES))
OUT_FILES = $(addprefix $(BUILD_DIR)/,$(OUT_FILENAMES))
BIN_FILES = $(addprefix $(BUILD_DIR)/,$(BIN_FILENAMES))
HEX_FILES = $(addprefix $(BUILD_DIR)/,$(HEX_FILENAMES))
OBJDUMP_FILES = $(addprefix $(BUILD_DIR)/,$(OBJDUMP_FILENAMES))
ALL = $(OBJ_FILES) $(OUT_FILES) $(MAP_FILES)\
      $(BIN_FILES) $(HEX_FILES) $(OBJDUMP_FILES)

.PHONY : all
all : $(ALL_FILENAMES)
 
.PHONY : clean
clean :
	rm -f ./$(BUILD_DIR)/*

%.o: %.s
	$(AS) $(AS_FLAGS) $< -o $(BUILD_DIR)/$@

%.out : %.o
	$(LD) $(LD_FLAGS) $(BUILD_DIR)/$< -o $(BUILD_DIR)/$@

%.bin : %.out
	$(OBJCOPY) -O binary $(BUILD_DIR)/$< $(BUILD_DIR)/$@

%.hex : %.bin
	xxd -e $(BUILD_DIR)/$< > $(BUILD_DIR)/$@

%.disasm : %.out
	$(OBJDUMP) $(BUILD_DIR)/$< -D > $@
	python3 disasm_parser.py $@ > $(BUILD_DIR)/$@
	rm $@

