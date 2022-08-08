include ../starlet.mk

CFLAGS += -fpic
LDSCRIPT = stub.ld
LIBS = -lgcc

TARGET = bootmii.elf
TARGET_BIN = bootmii.bin
OBJS = start.o stub.o loader.o utils.o

include ../common.mk

all: $(TARGET_BIN)

$(TARGET_BIN): $(TARGET)
	@echo  "  OBJCPY    $@"
	@$(OBJCOPY) -O binary $< $@

loader.o : ../loader/bootmii.elf
	@echo  "  BIN2O     $@"
	@$(BIN2S) -a 32 $< | $(AS) -o $(@)

clean: myclean

myclean:
	-rm -f $(TARGET_BIN)

