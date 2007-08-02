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

/* holds the base scene */
SDL_Surface *GFX_lpDDSTwo = NULL;
/* Beuc: apparently used for the scrolling screen transition */
SDL_Surface *GFX_lpDDSTrick = NULL;
SDL_Surface *GFX_lpDDSTrick2 = NULL;
