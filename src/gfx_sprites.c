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

#include "dinkvar.h"
#include "gfx_sprites.h"


/**
 * Free memory used by sprites. It's not much useful in itself, since
 * it's only called when we're exiting the game, but it does avoid
 * memory leak warnings when FreeDink is analyzed by Valgrind or other
 * memory checkers.
 */
void sprites_unload(void)
{
  int i = 0;
  for (i = 0; i < MAX_SPRITES; i++)
    if (GFX_k[i].k != NULL)
      SDL_FreeSurface(GFX_k[i].k);
}
