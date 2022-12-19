all: bootmii.bin

.PHONY: bootmii.bin
bootmii.bin:
	@$(MAKE) -C loader
	@$(MAKE) -C stub
	@$(MAKE) -C resetstub
	python3 mini/makebin.py stub/bootmii.bin resetstub/resetstub.elf bootmii.bin

clean:
	@$(MAKE) -C loader clean
	@$(MAKE) -C stub clean
	@$(MAKE) -C resetstub clean

