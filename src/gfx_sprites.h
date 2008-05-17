/**
 * Graphics - sprites management

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

#ifndef _GFX_SPRITES_H
#define _GFX_SPRITES_H

#include "rect.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Max number of sprites, minus 1 (GFX_k is indexed from 1) */
#define MAX_SPRITES 4000

  extern void sprites_unload(void);
  extern void load_sprite_pak(char org[100], int nummy, int speed, int xoffset, int yoffset,
			      rect hardbox, int notanim, int black, int leftalign, int samedir);
  extern void load_sprites(char org[100], int nummy, int speed, int xoffset, int yoffset,
			   rect hardbox, int notanim, int black, int leftalign);

#ifdef __cplusplus
}
#endif

#endif
