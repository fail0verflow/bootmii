/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	dummy reset stub

Copyright (C) 2008, 2009	Sven Peter <sven@svenpeter.dev>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

.global _start

.arm
_start:
	# enable boot0 memory mapping
	ldr	r0, =0x0d80018c
	ldr	r1, =0x1000
	ldr	r2, [r0]
	bic 	r2, r1
	str	r2, [r0]

	# setup boot0 memory mirror to x'ffff_0000
	ldr	r0, =0x0d800060
	mov	r2, #0x20
	ldr	r1, [r0]
	bic	r1, r2
	str	r1, [r0]

	# jump to boot0
	ldr	r0, =0xffff0000
	bx	r0
