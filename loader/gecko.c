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

#include "types.h"
#include "start.h"
#include "vsprintf.h"
#include "string.h"
#include "utils.h"
#include "hollywood.h"
#include "gecko.h"

static int gecko_active = 0;

static u32 _gecko_command(u32 command)
{
	u32 i;
	// Memory Card Port B (Channel 1, Device 0, Frequency 3 (32Mhz Clock))
	write32(EXI1_CSR, 0xd0);
	write32(EXI1_DATA, command);
	write32(EXI1_CR, 0x19);
	while (read32(EXI1_CR) & 1);
	i = read32(EXI1_DATA);
	write32(EXI1_CSR, 0);
	return i;
}

static u32 _gecko_getid(void)
{
	u32 i;
	// Memory Card Port B (Channel 1, Device 0, Frequency 3 (32Mhz Clock))
	write32(EXI1_CSR, 0xd0);
	write32(EXI1_DATA, 0);
	write32(EXI1_CR, 0x19);
	while (read32(EXI1_CR) & 1);
	write32(EXI1_CR, 0x39);
	while (read32(EXI1_CR) & 1);
	i = read32(EXI1_DATA);
	write32(EXI1_CSR, 0);
	return i;
}

static u32 _gecko_sendbyte(char sendbyte)
{
	u32 i = 0;
	i = _gecko_command(0xB0000000 | (sendbyte<<20));
	if (i&0x04000000)
		return 1; // Return 1 if byte was sent
	return 0;
}

static u32 _gecko_checksend(void)
{
	u32 i = 0;
	i = _gecko_command(0xC0000000);
	if (i&0x04000000)
		return 1; // Return 1 if safe to send
	return 0;
}

int gecko_isalive(void)
{
	u32 i;
	u32 id;
	id = _gecko_getid();
	if(id != 0x00000000)
		return 0;
	i = _gecko_command(0x90000000);
	if ((i&0xFFFF0000) != 0x04700000)
		return 0;
	return 1;
}

void gecko_init(void)
{
	write32(EXI0_CSR, 0);
	write32(EXI1_CSR, 0);
	write32(EXI2_CSR, 0);
	write32(EXI0_CSR, 0x2000);
	write32(EXI0_CSR, 3<<10);
	write32(EXI1_CSR, 3<<10);

	gecko_active = 0;
	if(gecko_isalive())
		gecko_active = 1;
}

#if 0
int gecko_sendbuffer(const void *buffer, u32 size)
{
	u32 left = size;
	char *ptr = (char*)buffer;

	while(left>0) {
		if(!_gecko_sendbyte(*ptr))
			break;
		ptr++;
		left--;
	}
	return (size - left);
}
#endif

int gecko_sendbuffer_safe(const void *buffer, u32 size)
{
	u32 left = size;
	char *ptr = (char*)buffer;
	
	int tries = 10000; // about 200ms of tries

	if((read32(HW_EXICTRL) & EXICTRL_ENABLE_EXI) == 0)
		return left;
	
	while(left>0) {
		if(_gecko_checksend()) {
			if(!_gecko_sendbyte(*ptr))
				break;
			ptr++;
			left--;
		} else {
			// if gecko is hung, time out and disable further attempts
			// only affects gecko users without an active terminal
			if(tries-- == 0) {
				gecko_active = 0;
				break;
			}
		}
	}
	return (size - left);
}

#if 0
int gecko_putchar(int ic)
{
	char b = ic;

	if(!gecko_active)
		return -1;
	return gecko_sendbuffer(&b, 1);
}
#endif

int gecko_puts(const char *s)
{
	if(!gecko_active)
		return -1;
	return gecko_sendbuffer_safe(s, strlen(s));
}

int gecko_printf( const char *fmt, ...)
{
	va_list args;
	char buffer[256];
	int i;

	if(!gecko_active)
		return -1;

	va_start(args, fmt);
	i = vsprintf(buffer, fmt, args);
	va_end(args);
	gecko_puts(buffer);
	return i;
}
