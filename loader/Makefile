include ../starlet.mk

TARGET = bootmii-sym.elf
TARGET_STRIPPED = bootmii.elf

OBJS = start.o main.o vsprintf.o string.o gecko.o memory.o memory_asm.o \
	sdhc.o sdmmc.o utils.o ff.o diskio.o lcd.o version.o

CFLAGS = -mbig-endian -mthumb -march=armv5t
CFLAGS += -fomit-frame-pointer -ffunction-sections
CFLAGS += -Wall -Wextra -Os -pipe

# uncomment to enabled LCD debug output
#DEFINES += -DENABLE_LCD
DEFINES += -DLOADER

LDFLAGS += -Wl,-N
LDSCRIPT = loader.ld
LIBS = -lgcc

include ../common.mk

all: $(TARGET_STRIPPED) loader_version.txt

$(TARGET_STRIPPED): $(TARGET)
	@echo "  STRIP     $@"
	@cp $< $@
	@$(STRIP) -R .comment -R .ARM.attributes -s $@

version.o: version.c ../describe.sh
	@echo "  VERSION   $@"
	@cat $< | sed "s/%VERSION%/`../describe.sh`/" | $(CC) $(CFLAGS) -c -x c - -o $@

loader_version.txt: ../describe.sh
	@../describe.sh > loader_version.txt

clean: myclean

myclean:
	-rm -f $(TARGET_STRIPPED) loader_version.txt

.PHONY: version.o

