/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	random utilities

Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcan.st>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#include "types.h"
#include "utils.h"
#include "hollywood.h"
#include "start.h"

void delay(u32 d)
{
	write32(HW_TIMER, 0);
	while(read32(HW_TIMER) < d);
}

void panic(u8 v)
{
	while(1) {
		debug_output(v);
		udelay(500000);
		debug_output(0);
		udelay(500000);
	}
}

