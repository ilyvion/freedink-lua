/**
 * Graphics

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
 */

#include "SDL.h"
#include "gfx.h"

LPDIRECTDRAW            lpDD = NULL;           // DirectDraw object
//LPDIRECTDRAWSURFACE     lpDDSOne;       // Offscreen surface 1

LPDIRECTDRAWSURFACE     lpDDSPrimary = NULL;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack = NULL;      // DirectDraw back surface

LPDIRECTDRAWSURFACE     lpDDSTwo = NULL;       // Offscreen surface 2
LPDIRECTDRAWSURFACE     lpDDSTrick = NULL;       // Offscreen surface 2
LPDIRECTDRAWSURFACE     lpDDSTrick2 = NULL;       // Offscreen surface 2


SDL_Surface *GFX_lpDDSPrimary = NULL; /* The physical screen */
SDL_Surface *GFX_lpDDSBack = NULL; /* Backbuffer for physical screen -
				      TODO: useless with SDL */

/* GFX_lpDDSTwo: holds the base scene */
/* Rationale attempt :*/
/* lpDDSTwo contains the background, which is reused for each new
   frame. It is overwritten when switching to another screen. However,
   it can change during a screen: 1) animated tiles (water & fire) 2)
   when a sprite is written on the background (eg when an enemy dies)
   3) with various hacks such as fill_screen() (and maybe
   copy_bmp_to_screen()). */
/* Those changes may conflict with each other (eg: an animated tile
   overwrites half the carcass of a dead enemy - which I think is used
   in one of Mike Snyder's DMods to make a slayer carcass disappear
   when killed on a weird alien floor; or maybe that was a real
   bug..). I think that some alternate versions of Dink considered
   those conflicts as bugs (not features) and fixed them. Maybe we
   should do so as well. */
/* After the background is done, all the other operations are applied
   on lpDDSBack, the double buffer which is directly used by the
   physical screen. (Note: in DX, that's the case; in SDL, the double
   buffer is included in lpDDSPrimary, so we need to get rid of the
   useless lpDDSBack intermediary - but later. */
SDL_Surface *GFX_lpDDSTwo = NULL;

/* Beuc: apparently used for the scrolling screen transition and more
   generaly as temporary buffers. Only used by the game, not the
   editor. */
/* Used in freedink.cpp only + as a local/independent temporary buffer
   in show_bmp&copy_bmp&process_show_bmp */
SDL_Surface *GFX_lpDDSTrick = NULL;
/* Used in freedink.cpp and update_frame.cpp */
SDL_Surface *GFX_lpDDSTrick2 = NULL;


/* Palettes */
PALETTEENTRY    real_pal[256];
SDL_Color GFX_real_pal[256];

