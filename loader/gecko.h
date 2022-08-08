/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	USBGecko support code

Copyright (c) 2008		Nuke - <wiinuke@gmail.com>
Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcan.st>
Copyright (C) 2008, 2009	Sven Peter <sven@svenpeter.dev>
Copyright (C) 2009		Andre Heider "dhewg" <dhewg@wiibrew.org>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#ifndef __GECKO_H__
#define __GECKO_H__

#include "types.h"

void gecko_flush(void);
int gecko_isalive(void);
int gecko_recvbuffer(void *buffer, u32 size);
int gecko_sendbuffer(const void *buffer, u32 size);
int gecko_recvbuffer_safe(void *buffer, u32 size);
int gecko_sendbuffer_safe(const void *buffer, u32 size);
int gecko_putchar(int c);
int gecko_getchar(void);
int gecko_puts(const char *s);
int gecko_printf( const char *fmt, ...);
void gecko_init(void);

#endif
