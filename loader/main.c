/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	Main loader code

Copyright (C) 2008, 2009	Haxx Enterprises <bushing@gmail.com>
Copyright (C) 2008, 2009	Sven Peter <sven@svenpeter.dev>
Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcan.st>
Copyright (C) 2009			Andre Heider "dhewg" <dhewg@wiibrew.org>
Copyright (C) 2009		John Kelley <wiidev@kelley.ca>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#include "types.h"
#include "utils.h"
#include "start.h"
#include "hollywood.h"
#include "sdhc.h"
#include "string.h"
#include "memory.h"
#include "elf.h"
#include "gecko.h"
#include "ff.h"
#include "lcd.h"
#include "gpio.h"

#define BOOT_FILE "/bootmii/armboot.bin"

extern void *__end;
extern const char *bootmii_version;

static FATFS fatfs;

static void hexline(void *addr, int len)
{
	u8 *p = (u8*)addr;
	while(len--) {
		gecko_printf("%02x",*p++);
	}
}

static void printhdr(ioshdr *hdr) {
	gecko_printf("ARMBOOT header (@%p):\n",hdr);
	gecko_printf(" Header size: %08x\n", hdr->hdrsize);
	gecko_printf(" Loader size: %08x\n", hdr->loadersize);
	gecko_printf(" ELF size: %08x\n", hdr->elfsize);
	gecko_printf(" Argument: %08x\n", hdr->argument);
	gecko_printf(" ELF at %p\n", (u8 *) hdr + hdr->hdrsize + hdr->loadersize);
}

static void loadelf(u8 *elf) {
	gecko_puts("Loading ELF...\n");

	if(memcmp("\x7F" "ELF\x01\x02\x01\x61\x01",elf,9)) {
		gecko_printf("Invalid ELF header:\n");
		hexline(elf, 9);
		gecko_printf("\n");
		panic(0xE3);
	}

	Elf32_Ehdr *ehdr = (Elf32_Ehdr*)elf;
	if(ehdr->e_phoff == 0) {
		gecko_printf("ELF has no program headers!\n");
		panic(0xE4);
	}

	int count = ehdr->e_phnum;
	Elf32_Phdr *phdr = (Elf32_Phdr*)(elf + ehdr->e_phoff);
	gecko_printf("PHDRS at %p\n",phdr);
	while(count--)
	{
		if(phdr->p_type != PT_LOAD) {
			gecko_printf(" Skipping PHDR of type %d\n",phdr->p_type);
		} else {
			void *src = elf + phdr->p_offset;
			gecko_printf(" LOAD %p -> %x [0x%x]\n",src, phdr->p_paddr, phdr->p_filesz);
			memcpy((void *)phdr->p_paddr, src, phdr->p_filesz);
		}
		phdr++;
	}
}

static void turn_stuff_on(void)
{
	clear32(HW_GPIO1OUT, 0x10);
	udelay(100);
	set32(HW_RESETS, 0x7ffffcf);
}

static void reset_audio(void)
{
	mask32(HW_DIFLAGS, 0x180, 0x100);

	clear32(0xd8001d0, 0x80000000);
	udelay(2);
	clear32(0xd8001d0, 0x40000000);

	clear32(0xd8001d0, 0x10000000);
	mask32(0xd8001cc, 0x7ffffff, 0x4b0ffce);

	udelay(10);
	set32(0xd8001d0, 0x40000000);
	udelay(500);
	set32(0xd8001d0, 0x80000000);
	udelay(2);
}

static void boot2_init1()
{
	//func_ffff5d08
	u32 reg = read32(0xd8001c8);

	if((reg & 0xc0000000) == 0xc0000000)
		return;

	clear32(0xd8001c8, 0x80000000);
	udelay(2);
	clear32(0xd8001c8, 0x40000000);
	udelay(10);
	set32(0xd8001c8, 0x40000000);
	udelay(50);
	set32(0xd8001c8, 0x80000000);
	udelay(2);
}

static void boot2_init2(u32 hlywdVerHi)
{
	//func_ffff5c40

	write32(0xd800088, 0xFE);
	udelay(2);

	clear32(0xd8001d8, 0x80000000);
	udelay(2);
	clear32(0xd8001d8, 0x40000000);
	udelay(10);
	set32(0xd8001d8, 0x40000000);
	udelay(50);
	set32(0xd8001d8, 0x80000000);
	udelay(2);

	write32(0xd800088, 0xF6);
	udelay(50);
	write32(0xd800088, 0xF4);
	udelay(1);
	write32(0xd800088, 0xF0);
	udelay(1);
	write32(0xd800088, 0x70);
	udelay(1);
	write32(0xd800088, 0x60);
	udelay(1);
	write32(0xd800088, 0x40);
	udelay(1);
	write32(0xd800088, 0);
	udelay(1);
	write32(0xd0400b4, 0x2214);
	if (hlywdVerHi)
		write32(0xd0400b0, 0x20600);
	else
		write32(0xd0400b0, 0x20400);
	write32(0xd0400a4, 0x26);
	udelay(1);
	write32(0xd0400a4, 0x2026);
	udelay(1);
	write32(0xd0400a4, 0x4026);
	udelay(20);
	write32(0xd0400cc, 0x111);
}

static void setup_gpios()
{
	write32(HW_GPIO1OWNER, GP_OWNER_PPC);
	mask32(HW_GPIO1OUT, GP_ARM_OUTPUTS, GP_ARM_DEFAULT_ON);
	write32(HW_GPIO1DIR, GP_ARM_OUTPUTS);
	mask32(HW_GPIO1BOUT, GP_PPC_OUTPUTS, GP_PPC_DEFAULT_ON);
	write32(HW_GPIO1BDIR, GP_PPC_OUTPUTS);
	write32(HW_GPIO1ENABLE, GP_ALL);
	udelay(2000);
	write32(HW_GPIO1INTLVL, 0);
	write32(HW_GPIO1INTFLAG, GP_ALL);
	write32(HW_GPIO1INTENABLE, 0);
	write32(HW_GPIO1BINTLVL, 0);
	write32(HW_GPIO1BINTFLAG, GP_ALL);
	write32(HW_GPIO1BINTENABLE, 0);
}

static void hardware_setup(void)
{
	u8 hwood_ver, hwood_hi, hwood_lo;
	hwood_ver = read32(HW_VERSION)  & 0xFF;
	hwood_hi = hwood_ver >> 4;
	hwood_lo = hwood_ver & 0xF;

	debug_output(0x03);

	set32(HW_EXICTRL, EXICTRL_ENABLE_EXI);

	mem_protect(1, (void*)0x13420000, (void*)0x1fffffff);

	clear32(HW_EXICTRL, 0x10);
	if(hwood_hi == 0)
		write32(0xd8b0010, 0);
	write32(0xd8b0010, 0);
	if(hwood_hi == 1 && hwood_lo == 0)
		mask32(0xd800140, 0x0000FFF0, 1);

	set32(0xd80018c, 0x400);
	set32(0xd80018c, 0x800);

	reset_audio();
	boot2_init1();
	boot2_init2(hwood_hi);
	setup_gpios();
	turn_stuff_on();

	write32(0xd8001e0, 0x65244A);
	write32(0xd8001e4, 0x46A024);

	debug_output(0x73);

	clear32(HW_GPIO1OWNER, 0x10);
	set32(HW_GPIO1DIR, 0x10);
}

void *_main(void *base)
{
	FRESULT fres;

	ioshdr *hdr = (ioshdr*)base;
	ioshdr *filehdr;
	u8 *elf;

	u32 bypass = 0;

	elf = (u8*) base;
	elf += hdr->hdrsize + hdr->loadersize;

	debug_output(0xF0);
	lcd_init();
	hardware_setup();
	debug_output(0xF1);

	if (read32(0x150effc) == 0x53544655)
		write32(0x150effc, 0);
	else
		gecko_init();

	lcd_puts("BootMii\n");
	lcd_puts("\n");

	gecko_puts("\n\nBootMii v");
	gecko_puts(bootmii_version);
	gecko_puts("\n");

	gecko_puts("Copyright (C) 2008-2010 Team Twiizers.\n");
	gecko_puts("Licensed under the GNU GPL version 2.\n");

	printhdr(hdr);

	if(hdr->argument == 0x42) {
		bypass = 1;
		goto boot2;
	}

	if (read32(0x150f000) == 0x424d454d) {
		gecko_puts("Trying to boot ELF from memory\n");

		filehdr = (ioshdr *) read32(0x150f004);
		write32(0x150f000, 0);
		write32(0x150f004, 0);

		printhdr(filehdr);
		void *entry = (void*)(((u32)filehdr) + filehdr->hdrsize);

		lcd_puts(" ~ ARMBOOT \n");
		gecko_printf("Launching ARMBOOT @ %p\n\n",entry);

		debug_output(0xF3);

		return entry;
	}

	gecko_puts("Trying to mount SD...\n");

	sdhc_init();
	fres = f_mount(0, &fatfs);

	if(fres != FR_OK) {
		gecko_printf("Error %d while trying to mount SD\n", fres);
		debug_output(0x12);
		debug_output(fres&0xFF);
		goto boot2;
	}

	//debug_output(0xF2);

	gecko_puts("Trying to open SD:" BOOT_FILE "\n");

	FIL fd;
	u32 read;

	fres = f_open(&fd, BOOT_FILE, FA_READ);
	if(fres != FR_OK) {
		gecko_printf("Error %d while trying to open file\n", fres);
		//debug_output(0x13);
		//debug_output(fres&0xFF);
		goto boot2;
	}

	lcd_puts(".");
	debug_output(0xF2);

	filehdr = (ioshdr *)ALIGN_FORWARD(((u32)&__end) + 0x100, 0x100);

	gecko_printf("Trying to read ARMBOOT header to %p\n", filehdr);

	fres = f_read(&fd, filehdr, sizeof(ioshdr), &read);
	if(fres != FR_OK) {
		gecko_printf("Error %d while trying to read file header\n", fres);
		//debug_output(0x14);
		//debug_output(fres&0xFF);
		goto boot2;
	}
	if(read != sizeof(ioshdr)) {
		gecko_printf("Got %d bytes, expected %d\n", read, sizeof(ioshdr));
		//debug_output(0x24);
		goto boot2;
	}

	lcd_puts(".");
	//debug_output(0xF5);

	printhdr(filehdr);

	u32 totalsize = filehdr->hdrsize + filehdr->loadersize + filehdr->elfsize;

	gecko_printf("Total ARMBOOT size: 0x%x\n", totalsize);
	gecko_printf("Trying to read ARMBOOT to %p\n", filehdr);

	fres = f_read(&fd, filehdr+1, totalsize-sizeof(ioshdr), &read);
	if(fres != FR_OK) {
		gecko_printf("Error %d while trying to read file header\n", fres);
		//debug_output(0x15);
		//debug_output(fres&0xFF);
		goto boot2;
	}
	if(read != (totalsize-sizeof(ioshdr))) {
		gecko_printf("Got %d bytes, expected %d\n", read, (totalsize-sizeof(ioshdr)));
		//debug_output(0x25);
		goto boot2;
	}

	lcd_puts(".");

	//debug_output(0xF6);

	gecko_puts("ARMBOOT read\n");

	void *entry = (void*)(((u32)filehdr) + filehdr->hdrsize);

	lcd_puts(" ~ ARMBOOT \n");

	gecko_printf("Launching ARMBOOT @ %p\n\n",entry);

	debug_output(0xF3);

	return entry;

boot2:
	debug_output(0xC8);
	if(bypass) {
		gecko_puts("In bypass mode, loading boot2\n");
	} else {
		gecko_puts("Couldn't load from SD, falling back to boot2\n");
		set32(HW_GPIO1BOUT, GP_SLOTLED);
		udelay(30000);
		clear32(HW_GPIO1BOUT, GP_SLOTLED);
		udelay(50000);
		set32(HW_GPIO1BOUT, GP_SLOTLED);
		udelay(30000);
		clear32(HW_GPIO1BOUT, GP_SLOTLED);
		udelay(50000);
	}
	lcd_puts(" ~ BOOT2 \n");
	loadelf(elf);
	debug_output(0xC9);
	gecko_puts("Starting boot2...\n\n");
	return (void *) 0xFFFF0000;
}
