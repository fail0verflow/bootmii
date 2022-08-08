/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	GPIO LCD display support

	(yes, that thing in that youtube video)

Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcan.st>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#ifndef __LCD_H__
#define __LCD_H__

#ifdef ENABLE_LCD

#include "types.h"

void lcd_init(void);
int lcd_putchar(int c);
int lcd_puts(const char *s);
void lcd_setdelay(u32 delay);
int lcd_printf( const char *fmt, ...);

#else

#define lcd_init(...)
#define lcd_putchar(...)
#define lcd_puts(...)
#define lcd_setdelay(...)
#define lcd_printf(...)

#endif

#endif

