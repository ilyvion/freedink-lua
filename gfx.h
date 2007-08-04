/**
 * Header for graphics

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

#ifndef _GFX_H
#define _GFX_H

#include <ddraw.h>
#include "SDL.h"

extern LPDIRECTDRAW lpDD; /* DirectDraw object */
extern LPDIRECTDRAWSURFACE lpDDSPrimary;
extern LPDIRECTDRAWSURFACE lpDDSBack;
extern LPDIRECTDRAWSURFACE lpDDSTrick;
extern LPDIRECTDRAWSURFACE lpDDSTrick2;
extern LPDIRECTDRAWSURFACE lpDDSTwo;

extern SDL_Surface *GFX_lpDDSPrimary;
extern SDL_Surface *GFX_lpDDSBack;
extern SDL_Surface *GFX_lpDDSTrick;
extern SDL_Surface *GFX_lpDDSTrick2;
extern SDL_Surface *GFX_lpDDSTwo;

extern PALETTEENTRY real_pal[256];
extern SDL_Color GFX_real_pal[256];
extern int trigger_palette_change;
extern SDL_Color cur_screen_palette[256];
extern void change_screen_palette(SDL_Color* new_palette);
#endif
