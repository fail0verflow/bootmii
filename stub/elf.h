/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	ELF structures

Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcan.st>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#ifndef __ELF_H__
#define __ELF_H__

#include "types.h"

#define EI_NIDENT 16

typedef struct {
	unsigned char   e_ident[EI_NIDENT];
	u16 e_type;
	u16 e_machine;
	u32 e_version;
	void *e_entry;
	u32 e_phoff;
	u32 e_shoff;
	u32 e_flags;
	u16 e_ehsize;
	u16 e_phentsize;
	u16 e_phnum;
	u16 e_shentsize;
	u16 e_shnum;
	u16 e_shtrndx;
} Elf32_Ehdr;

typedef struct {
	u32 p_type;
	u32 p_offset;
	void *p_vaddr;
	void *p_paddr;
	u32 p_filesz;
	u32 p_memsz;
	u32 p_flags;
	u32 p_align;
} Elf32_Phdr;

#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_SHLIB    5
#define PT_PHDR     6

#endif
