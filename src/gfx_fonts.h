/**
 * Fonts

 * Copyright (C) 2007  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with program; see the file COPYING. If not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.

 * I made this header because I though update_frame had to be compiled
 * separately - actually it was also included in dink.cpp. Might still
 * come in handy.
 */

#ifndef _GFX_FONTS_H
#define _GFX_FONTS_H

#include "SDL_ttf.h"
#include "rect.h"

/* extern HFONT hfont_small; */
extern TTF_Font *FONTS_hfont_small;

extern void FONTS_init(void);
extern int FONTS_initfonts(char* fontname);
/* extern int print_text_wrap (TTF_Font * font, char *str, struct rect * box, SDL_Color color, */
/* 			    /\*bool*\/int hcenter, /\*bool*\/int vcenter); */
extern int print_text_wrap (char *str, rect * box,
			    /*bool*/int hcenter, /*bool*/int vcenter, int calc_only);
/* extern void initfonts(char fontname[255]); */
extern void kill_fonts(void);
extern void FONTS_SetTextColor(Uint8 r, Uint8 g, Uint8 b);
extern void FONTS_SetFont(TTF_Font *font);

extern void SaySmall(char thing[500], int px, int py, int r,int g,int b);
extern void Say(char thing[500], int px, int py);

#endif
