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
	@ Get real address of _start
	sub	r4, pc, #8
	@ Subtract offset to get the address that we were loaded at
	ldr	r0, =_start
	sub	r4, r4, r0
	@ Output 0x42 to the debug port
	mov	r0, #0x42
	bl	debug_output
	
	@ Set up a stack
	ldr	sp, =__stack_addr
	add	sp, r4
	
	@ perform boot2v3 memory controller poke
	bl memctrl_do_sub_sub_poke

	@ Output 0x43 to the debug port
	mov	r0, #0x43
	bl	debug_output
	
	@ check for a previous GOT relocation
	ldr	r1, [pc, #_is_reloced - . - 8]
	cmp	r1, #0
	bne	done_got
	
	@ relocate the GOT entries
	ldr	r1, =__got_start
	add	r1, r4
	ldr	r2, =__got_end
	add	r2, r4
got_loop:
	@ check for the end
	cmp	r1, r2
	beq	done_got
	@ read the GOT entry
	ldr	r3, [r1]
	@ add our base address
	add	r3, r4
	str	r3, [r1]
	@ move on
	add	r1, r1, #4
	b	got_loop

done_got:

	@ note that we have already reloced GOT 
	mov	r1, #1
	str	r1, [pc, #_is_reloced - . - 8]

	@ clear BSS
	ldr	r1, =__bss_start
	add	r1, r4
	ldr	r2, =__bss_end
	add	r2, r4
	mov	r3, #0
bss_loop:
	@ check for the end
	cmp	r1, r2
	beq	done_bss
	@ clear the word and move on
	str	r3, [r1]
	add	r1, r1, #4
	b	bss_loop

done_bss:
	mov	r0, #0x44
	bl	debug_output
	@ take the plunge
	mov	r0, r4
	bl	_main
	@ _main returned! Go to whatever address it returned...
	mov	r1, r0
	mov	r0, r4
	mov	pc, r1

_is_reloced:
	.long 0

memctrl_do_sub_sub_poke:
	stmdb	sp!, {lr}
	ldr		r0, =0x163	@ reg_address
	mov		r1, #0x4C	@ address
	bl		memctrl_sub_poke
	ldr		r0, =0x163	@ read address back (flush?)
	bl		memctrl_sub_peek
	ldr		r0, =0x162	@ reg_data
	mov		r1, #1		@ data
	bl		memctrl_sub_poke
	ldmia	sp!, {pc}

memctrl_sub_poke:
	ldr		r2, =0xD8B4000
	strh	r0, [r2, #0x74] @ reg_address <= address
	ldrh	r0, [r2, #0x74] @ read reg_address back
	strh	r1, [r2, #0x76] @ reg_data <= data
	mov		pc, lr

memctrl_sub_peek:
	ldr		r2, =0xD8B4000
	strh	r0, [r2, #0x74] @ reg_address <= address
	ldrh	r0, [r2, #0x74] @ read reg_address back
	ldrh	r0, [r2, #0x76] @ data <= reg_data
	mov		pc, lr

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
	mov	pc, lr
