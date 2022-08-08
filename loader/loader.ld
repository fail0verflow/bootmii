/*
	mini - a Free Software replacement for the Nintendo/BroadOn IOS.
	linker script
	
Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcansoft.com>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

OUTPUT_FORMAT("elf32-bigarm")
OUTPUT_ARCH(arm)
EXTERN(_start)
ENTRY(_start)

__base_addr	=	0x10400000;

PHDRS
{
  init PT_LOAD FLAGS(5);
  text PT_LOAD FLAGS(5);
  data PT_LOAD FLAGS(6);
  bss PT_LOAD FLAGS(6);
}

SECTIONS
{
	. = __base_addr;
	
	.init :
	{
		*(.init)
		*(.init.*)
		. = ALIGN(4);
	} :init
	
	.text :
	{
		*(.text)
		*(.text.*)
		*(.gnu.warning)
		*(.gnu.linkonce.t*)
		*(.glue_7)
		*(.glue_7t)
		. = ALIGN(4);
	} :text

	__text_end = . ;

	.rodata :
	{
		*(.rodata)
		*all.rodata*(*)
		*(.roda)
		*(.rodata.*)
		*(.gnu.linkonce.r*)
		. = ALIGN(4);
	} :data
	
	.data :
	{
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
		. = ALIGN(4);
	} :data
	
	.bss :
	{
		__bss_start = . ;
		*(.dynbss)
		*(.gnu.linkonce.b*)
		*(.bss*)
		*(.sbss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end = . ;
	} :bss

	. = ALIGN(4);
	__stack_end = .;
	__stack_addr = (__stack_end + 0x4000);
	__end = __stack_addr ;
}

PROVIDE (__stack_end = __stack_end);
PROVIDE (__stack_addr = __stack_addr);
PROVIDE (__bss_start = __bss_start);
PROVIDE (__bss_end = __bss_end);
PROVIDE (__end = __end);
