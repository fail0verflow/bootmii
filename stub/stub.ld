/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	ELF loader stub linker script
	
Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcansoft.com>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

OUTPUT_FORMAT("elf32-bigarm")
OUTPUT_ARCH(arm)
EXTERN(_start)
ENTRY(_start)

__base_addr	=	0;

SECTIONS
{
	. = __base_addr;
	.header :
	{
		__header = .;
		/* Entry point (offset) */
		LONG(__code_start);
		/* Loader size */
		LONG(__loader_size);
		/* ELF size */
		LONG(0);
		/* Boot argument? */
		LONG(0);
		. = ALIGN(16);
	}
	
	__code_start = .;
	
	.init :
	{
		*(.init)
		. = ALIGN(4);
	}
	
	.got :
	{
		__got_start = .;
		*(.got.*)
		*(.got)
		. = ALIGN(4);
		__got_end = . ;
	}
	
	.text :
	{
		*(.text)
		*(.text.*)
		*(.gnu.warning)
		*(.gnu.linkonce.t*)
		*(.glue_7)
		*(.glue_7t)
		. = ALIGN(4);
	}

	__text_end = . ;

	.rodata :
	{
		*(.rodata)
		*all.rodata*(*)
		*(.roda)
		*(.rodata.*)
		*(.gnu.linkonce.r*)
		. = ALIGN(4);
	}
	
	.data :
	{
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
		. = ALIGN(4);
	}
	
	.bss :
	{
		__bss_start = . ;
		*(.dynbss)
		*(.gnu.linkonce.b*)
		*(.bss*)
		*(.sbss*)
		*(COMMON)
		. = ALIGN(32);
		__bss_end = . ;
	}

}

__stack_end = (__bss_end);
__stack_addr = (__bss_end + 0x100);

__end = __stack_addr ;
__loader_size = __end - __code_start;

PROVIDE (__stack_end = __stack_end);
PROVIDE (__stack_addr = __stack_addr);
PROVIDE (__got_start = __got_start);
PROVIDE (__got_end = __got_end);
PROVIDE (__bss_start = __bss_start);
PROVIDE (__bss_end = __bss_end);
