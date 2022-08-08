/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	Hollywood register definitions

Copyright (C) 2008, 2009	Haxx Enterprises <bushing@gmail.com>
Copyright (C) 2008, 2009	Sven Peter <sven@svenpeter.dev>
Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcan.st>
Copyright (C) 2008, 2009	John Kelley <wiidev@kelley.ca>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#ifndef __HOLLYWOOD_H__
#define __HOLLYWOOD_H__

/* Hollywood Registers */

#define		HW_REG_BASE			0xd800000
#define		HW_TIMER			(HW_REG_BASE + 0x010)
#define		HW_IRQENABLE		(HW_REG_BASE + 0x03c)
#define		HW_MEMMIRR			(HW_REG_BASE + 0x060)
#define		HW_GPIO1ENABLE		(HW_REG_BASE + 0x0dc)
#define		HW_GPIO1OUT			(HW_REG_BASE + 0x0e0)
#define		HW_GPIO1DIR			(HW_REG_BASE + 0x0e4)
#define		HW_GPIO1IN			(HW_REG_BASE + 0x0e8)
#define		HW_GPIO1INTLVL		(HW_REG_BASE + 0x0ec)
#define		HW_GPIO1INTFLAG		(HW_REG_BASE + 0x0f0)
#define		HW_GPIO1OWNER		(HW_REG_BASE + 0x0fc)

#endif
