/**
 * Header for graphics

 * Copyright (C) 2007  Sylvain Beucler

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

#ifndef _GFX_H
#define _GFX_H

#include "SDL.h"

enum gfx_init_state
{
    GFX_NOT_INITIALIZED = 0,
    GFX_INITIALIZING_VIDEO,
    GFX_INITIALIZING_FONTS,
    GFX_INITIALIZED,
    GFX_QUITTING,
};

enum gfx_windowed_state
{
    GFX_WINDOWED = 0,
    GFX_FULLSCREEN,
};

extern int truecolor;
extern SDL_Surface *GFX_lpDDSBack;
extern SDL_Surface *GFX_lpDDSTrick;
extern SDL_Surface *GFX_lpDDSTrick2;
extern SDL_Surface *GFX_lpDDSTwo;

extern int trigger_palette_change;
extern SDL_Color GFX_real_pal[256];
extern SDL_Color cur_screen_palette[256];

extern enum gfx_init_state gfx_get_init_state(void);

extern int gfx_init(enum gfx_windowed_state);
extern int gfx_init_failsafe();
extern void gfx_quit(void);
extern void change_screen_palette(SDL_Color* new_palette);
extern SDL_Surface* load_bmp(char *filename);
extern SDL_Surface* load_bmp_from_fp(FILE* in);
extern SDL_Surface* load_bmp_from_mem(SDL_RWops *rw);
extern SDL_Surface* load_bmp_setpal(FILE *in);
extern int gfx_blit_nocolorkey(SDL_Surface *src, SDL_Rect *src_rect, SDL_Surface *dst, SDL_Rect *dst_rect);
extern int gfx_blit_stretch(SDL_Surface *src, SDL_Rect *src_rect, SDL_Surface *dst, SDL_Rect *dst_rect);

#endif
