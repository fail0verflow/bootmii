/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	reset stub linker script
	
Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcansoft.com>
Copyright (C) 2009			Andre Heider "dhewg" <dhewg@wiibrew.org>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

OUTPUT_FORMAT("elf32-bigarm")
OUTPUT_ARCH(arm)
EXTERN(_start)
EXTERN(__ipc_info)
ENTRY(_start)

MEMORY {
	ram		: ORIGIN = 0x93ff0000, LENGTH = 64K
}

SECTIONS
{
	.init :
	{
		*(.init)
		. = ALIGN(4);
	} >ram

	.text :
	{
		*(.text*)
		*(.text.*)
		*(.gnu.warning)
		*(.gnu.linkonce.t*)
		*(.glue_7)
		*(.glue_7t)
		. = ALIGN(4);
	} >ram

	.rodata :
	{
		*(.rodata)
		*all.rodata*(*)
		*(.roda)
		*(.rodata.*)
		*(.gnu.linkonce.r*)
		. = ALIGN(4);
	} >ram

	.data :
	{
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
		. = ALIGN(4);
	} >ram

	.bss :
	{
		__bss_start = . ;
		*(.dynbss)
		*(.gnu.linkonce.b*)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end = . ;
	} >ram

	/DISCARD/ :
	{
		*(.ARM.exidx*)
		*(.ARM.extab*)
	}

}


