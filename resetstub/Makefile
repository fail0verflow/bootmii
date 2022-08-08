include ../starlet.mk

CFLAGS += -fpic
LDSCRIPT = stub.ld
LIBS =

TARGET = resetstub-sym.elf
TARGET_STRIPPED = resetstub.elf
OBJS = stub.o

include ../common.mk

all: $(TARGET_STRIPPED)

$(TARGET_STRIPPED): $(TARGET)
	@echo  "STRIP	$@"
	@$(STRIP) $< -o $@

clean: myclean

myclean:
	-rm -f $(TARGET) $(TARGET_STRIPPED)

