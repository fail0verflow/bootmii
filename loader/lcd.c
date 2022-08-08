/*
	bootmii - a Free Software replacement for the Nintendo/BroadOn boot2.
	GPIO LCD display support

	(yes, that thing in that youtube video)

Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcan.st>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#ifdef ENABLE_LCD

#include "types.h"
#include "lcd.h"
#include "start.h"
#include "vsprintf.h"
#include "string.h"
#include "utils.h"
#include "hollywood.h"

void lcdinit(void);

char ddram[0x80];
u8 ddaddr = 0;
u32 chardelay = 0;

#define WIDTH 16
#define HEIGHT 2

u8 linestarts[HEIGHT] = { 0x00, 0x40 };

u8 cx, cy;

// for 4x20
//u8 linestarts[HEIGHT] = { 0x00, 0x40, 0x14, 0x54 };

#define LCD_HOME_CLEAR		0x01
#define LCD_HOME			0x02
#define LCD_ENTRY_MODE		0x04
# define EM_INC					0x02
# define EM_DEC					0x00
# define EM_SHIFT				0x01
#define LCD_DISPLAY_ONOFF	0x08
# define D_DISPLAY				0x04
# define D_CURSOR				0x02
# define D_BLINK				0x01
#define LCD_SHIFT			0x10
# define SH_SHIFT				0x08
# define SH_CURSOR				0x00
# define SH_RIGHT				0x04
# define SH_LEFT				0x00
#define LCD_FUNCTION_SET	0x20
# define FS_8BIT				0x10
# define FS_4BIT				0x00
# define FS_2LINE				0x08
# define FS_1LINE				0x00
# define FS_5X10				0x04
# define FS_5X8					0x00
#define LCD_CGRAM			0x40
#define LCD_DDRAM			0x80

#define LCD_PORT	HW_GPIO1OUT
#define LCD_MASK	0x00FC0000
#define LCD_E		0x00040000
#define LCD_RS		0x00080000
#define LCD_DSHIFT 20

static void _lcdnybble(char rs, char n)
{
	if(rs)
		mask32(LCD_PORT, LCD_MASK, ((n&0xF) << LCD_DSHIFT) | LCD_RS);
	else
		mask32(LCD_PORT, LCD_MASK, ((n&0xF) << LCD_DSHIFT));
	
	udelay(4);
	set32(HW_GPIO1OUT, LCD_E);
	udelay(4);
	clear32(HW_GPIO1OUT, LCD_E);
	udelay(4);
}

static void _lcdcmd(char c)
{
	_lcdnybble(0, c>>4);
	_lcdnybble(0, c&0x0F);
	udelay(50);
}

static void _lcdwrite(char c)
{
	_lcdnybble(1, c>>4);
	_lcdnybble(1, c&0x0F);
	ddram[ddaddr] = c;
	ddaddr = (ddaddr+1)&0x7F;
	udelay(50);
}

static void _lcdgoto(u8 addr)
{
	_lcdcmd(LCD_DDRAM | addr);
	ddaddr = addr;
}

static void _lcdgotoxy(u8 x, u8 y)
{
	u8 addr = (x + linestarts[y]);
	_lcdcmd(LCD_DDRAM | addr);
	ddaddr = addr;
}

static void _lcdclear(void)
{
	int i;
	_lcdcmd(LCD_HOME_CLEAR);
	udelay(2000);
	for(i=0;i<0x80;i++) {
		ddram[i] = ' ';
	}
}

static void _lcdscroll(u8 count)
{
	int x,y;
	_lcdcmd(LCD_HOME_CLEAR);
	udelay(2000);
	for(y=0;y<(HEIGHT-count);y++) {
		_lcdgotoxy(0,y);
		for(x=0;x<WIDTH;x++) {
			_lcdwrite(ddram[x + linestarts[y+count]]);
		}
	}
	for(;y<HEIGHT;y++) {
		for(x=0;x<WIDTH;x++) {
			ddram[linestarts[y] + x] = ' ';
		}
	}
	_lcdgoto(linestarts[HEIGHT-1]);
}

void lcd_init(void)
{
	cx = cy = 0;
	ddaddr = 0;
	memset(ddram, ' ', 0x80);
	clear32(LCD_PORT, LCD_MASK);
	udelay(100000);
	_lcdnybble(0, (LCD_FUNCTION_SET | FS_8BIT)>>4);
	udelay(10000);
	_lcdnybble(0, (LCD_FUNCTION_SET | FS_8BIT)>>4);
	udelay(1000);
	_lcdnybble(0, (LCD_FUNCTION_SET | FS_8BIT)>>4);
	udelay(1000);
	_lcdnybble(0, (LCD_FUNCTION_SET | FS_4BIT)>>4);
	udelay(1000);
	_lcdcmd(LCD_FUNCTION_SET | FS_4BIT | FS_2LINE | FS_5X8);
	_lcdcmd(LCD_DISPLAY_ONOFF);
	_lcdcmd(LCD_HOME_CLEAR);
	udelay(2000);
	_lcdcmd(LCD_ENTRY_MODE | EM_INC);
	_lcdcmd(LCD_DISPLAY_ONOFF | D_DISPLAY | D_CURSOR);
}

int lcd_putchar(int ic)
{
	char c = ic;
	// defer newlines to keep last line of LCD full
	if(c == '\n') {
		cx = 0;
		cy++;
		if(cy < HEIGHT)
			_lcdgotoxy(cx,cy);
	} else {
		if(cx >= 16) {
			cx = 0;
			cy++;
			if(cy < HEIGHT)
				_lcdgotoxy(cx,cy);
		}
		if(cy >= (2*HEIGHT-1)) {
			_lcdclear();
			_lcdgoto(linestarts[HEIGHT-1]);
			cy = HEIGHT - 1;
			cx = 0;
		} else if(cy >= HEIGHT) {
			_lcdscroll(cy - HEIGHT + 1);
			cy = HEIGHT - 1;
			cx = 0;
		}
		_lcdwrite(c);
		cx++;
	}
	return (u8)c;
}

void lcd_setdelay(u32 delay)
{
	chardelay = delay;
}

int lcd_puts(const char *s)
{
	while(*s) {
		lcd_putchar(*s++);
		udelay(chardelay);
	}
	return 0;
}

int lcd_printf( const char *fmt, ...)
{
	va_list args;
	char buffer[1024];
	int i;

	va_start(args, fmt);
	i = vsprintf(buffer, fmt, args);
	va_end(args);
	lcd_puts(buffer);
	return i;
}

#endif

