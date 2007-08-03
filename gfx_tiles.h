/**
 * Draw background from tiles

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

#ifndef _GFX_TILES_H
#define _GFX_TILES_H

#include <ddraw.h>
#include "SDL.h"

extern LPDIRECTDRAWSURFACE tiles[];
extern SDL_Surface *GFX_tiles[];

extern void load_tiles(void);
extern void draw_map_game(void);
extern void draw_map_game_background(void);
extern void process_animated_tiles(void);

#endif
