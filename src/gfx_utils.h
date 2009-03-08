/**
 * Graphics utilities: palettes...

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

#ifndef _GFX_UTILS_H
#define _GFX_UTILS_H

#include "SDL.h"

extern void load_batch(void);
extern int load_palette_from_surface (SDL_Surface *bmp, SDL_Color *palette);
extern int load_palette_from_bmp (char *file, SDL_Color * palette);
extern void setup_palette(SDL_Color *palette);

#endif
