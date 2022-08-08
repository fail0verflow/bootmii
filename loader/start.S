/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	system startup

Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcan.st>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

.arm

.extern _main
.extern __got_start
.extern __got_end
.extern __bss_start
.extern __bss_end
.extern __stack_addr
.globl _start
.globl debug_output

.section .init

_start:
	@ Get loader base from parent
	mov	r4, r0

	@ Output 0x42 to the debug port
	mov	r0, #0x42
	bl	debug_output
	
	@ Set up a stack
	ldr	sp, =__stack_addr
	
	@ Output 0x43 to the debug port
	mov	r0, #0x43
	bl	debug_output

	@ clear BSS
	ldr	r1, =__bss_start
	ldr	r2, =__bss_end
	mov	r3, #0
bss_loop:
	@ check for the end
	cmp	r1, r2
	bge	done_bss
	@ clear the word and move on
	str	r3, [r1]
	add	r1, r1, #4
	b	bss_loop

done_bss:
	mov	r0, #0x44
	bl	debug_output
	@ take the plunge
	mov	r0, r4
	ldr	r1, =_main+1
	mov	lr, pc
	bx	r1
	@ _main returned! Go to whatever address it returned...
	mov	pc, r0

.pool

debug_output:
	@ load address of port
	mov	r3, #0xd800000
	@ load old value
	ldr	r2, [r3, #0xe0]
	@ clear debug byte
	bic	r2, r2, #0xFF0000
	@ insert new value
	and	r0, r0, #0xFF
	orr	r2, r2, r0, LSL #16
	@ store back
	str	r2, [r3, #0xe0]
	bx	lr

