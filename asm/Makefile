BUILD_DIR = build

AS = riscv64-unknown-elf-as
LD = riscv64-unknown-elf-ld
OBJCOPY = riscv64-unknown-elf-objcopy

AS_FLAGS = 
LD_FLAGS := 
ASM_FILES = $(shell ls *.s)

OBJ_FILENAMES = $(ASM_FILES:.s=.o)
OUT_FILENAMES = $(ASM_FILES:.s=.out)
BIN_FILENAMES = $(ASM_FILES:.s=.bin)
HEX_FILENAMES = $(ASM_FILES:.s=.hex)
ALL_FILENAMES = $(OBJ_FILENAMES) $(OUT_FILENAMES)\
                $(BIN_FILENAMES) $(HEX_FILENAMES)

OBJ_FILES = $(addprefix $(BUILD_DIR)/,$(OBJ_FILENAMES))
OUT_FILES = $(addprefix $(BUILD_DIR)/,$(OUT_FILENAMES))
BIN_FILES = $(addprefix $(BUILD_DIR)/,$(BIN_FILENAMES))
HEX_FILES = $(addprefix $(BUILD_DIR)/,$(HEX_FILENAMES))
ALL = $(OBJ_FILES) $(OUT_FILES) $(MAP_FILES)\
      $(BIN_FILES) $(HEX_FILES)

.PHONY : all
all : $(ALL_FILENAMES)
	mv $(ALL_FILENAMES) $(BUILD_DIR)/
 
.PHONY : clean
clean :
	rm -f ./$(BUILD_DIR)/*

%.o: %.s
	$(AS) $(AS_FLAGS) $< -o $@

%.out : %.o
	$(LD) $(LD_FLAGS) -Map=$(BUILD_DIR)/$(basename $(@F)).map $< -o $@

%.bin : %.out
	$(OBJCOPY) -O binary $< $@

%.hex : %.bin
	xxd -e $< > $@

