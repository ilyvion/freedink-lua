/**
 * Fonts

 * Copyright (C) 2007, 2008  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _GFX_FONTS_H
#define _GFX_FONTS_H

#include "SDL_ttf.h"
#include "rect.h"


typedef enum enum_font
{
    FONT_DIALOG = 0,
    FONT_SYSTEM,
} FONT_TYPE;


extern int gfx_fonts_init(void);
extern int gfx_fonts_init_failsafe(void);
extern void gfx_fonts_init_colors(void);
extern void gfx_fonts_quit(void);
extern void FONTS_SetTextColor(Uint8 r, Uint8 g, Uint8 b);
extern void FONTS_SetTextColorIndex(int no);
extern int print_text_wrap(char *str, rect * box, /*bool*/int hcenter, int calc_only, FONT_TYPE font_type);
extern void print_text_wrap_debug(char *str, int x, int y);


extern void SaySmall(char thing[500], int px, int py, int r, int g, int b);
extern void Say(char thing[500], int px, int py);

/* DinkC binding */
extern int initfont(char* fontname);
extern void set_font_color(int no, int red, int green, int blue);
#endif
