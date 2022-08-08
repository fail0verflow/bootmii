/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	ELF loader stub

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
#include "elf.h"

typedef struct {
	u32 hdrsize;
	u32 loadersize;
	u32 elfsize;
	u32 argument;
} ioshdr;

static inline void _memcpy(void *dst, const void *src, size_t len)
{
        size_t i;

        for (i = 0; i < len; i++)
                ((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
}

static inline int memcmp(const char *s1, const u8 *s2, size_t len)
{
        size_t i;

        for (i = 0; i < len; i++)
                if (s1[i] != s2[i]) return s1[i] - s2[i];
        
        return 0;
}

void *loadelf(const u8 *elf) {
	if(memcmp("\x7F" "ELF\x01\x02\x01",elf,7)) {
		panic(0xE3);
	}
	
	Elf32_Ehdr *ehdr = (Elf32_Ehdr*)elf;
	if(ehdr->e_phoff == 0) {
		panic(0xE4);
	}
	int count = ehdr->e_phnum;
	Elf32_Phdr *phdr = (Elf32_Phdr*)(elf + ehdr->e_phoff);
	while(count--)
	{
		if(phdr->p_type == PT_LOAD) {
			const void *src = elf + phdr->p_offset;
			_memcpy(phdr->p_paddr, src, phdr->p_filesz);
		}
		phdr++;
	}
	return ehdr->e_entry;
}

static inline void mem_setswap(void)
{
	u32 d = read32(HW_MEMMIRR);
	if(!(d & 0x20))
		write32(HW_MEMMIRR, d | 0x20);
}

extern const u8 bootmii_elf[];
extern const u8 bootmii_elf_end[];
extern const u32 bootmii_elf_size;

#define GP_POWER 0x000001

void *_main(void *base)
{
	ioshdr *hdr = (ioshdr*)base;
	u8 *elf;
	void *entry;

	elf = (u8*) base;
	elf += hdr->hdrsize + hdr->loadersize;

	debug_output(0xF1);
	mem_setswap();

	write32(HW_IRQENABLE, 0);

	set32(HW_GPIO1ENABLE, GP_POWER);
	clear32(HW_GPIO1DIR, GP_POWER);
	clear32(HW_GPIO1OWNER, GP_POWER);
	set32(HW_GPIO1INTLVL, GP_POWER);
	udelay(1000);
	set32(HW_GPIO1INTFLAG, GP_POWER);
	udelay(400000);
	if(read32(HW_GPIO1INTFLAG) & GP_POWER) {
		debug_output(0xC0);
		loadelf(elf);
		debug_output(0xC1);
		return (void *) 0xFFFF0000;
	}

	// NOTE: END CRITICAL ZONE

	debug_output(0xF2);
	entry = loadelf(bootmii_elf);
	debug_output(0xF3);

	return entry;
}
