/**
 * SDL mouse+keyboard test

 * Copyright (C) 2008  Sylvain Beucler

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

#include "SDL.h"

int main(int argc, char*argv[])
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *screen = SDL_SetVideoMode(320, 200, 0, 0);
  /* EnableUNICODE needs to be placed after SDL_Init, and it does not
     matter whether it's placed before or after SetVideoMode) */
  SDL_EnableUNICODE(1);

  SDL_Event e;
  while (SDL_WaitEvent(&e))
    {
      if (e.type == SDL_KEYDOWN)
	{
	  
	  printf("code=%d(keyname'%s')\tu=%d(ascii'%c')\n",
		 e.key.keysym.sym,
		 SDL_GetKeyName(e.key.keysym.sym),
		 e.key.keysym.unicode,
		 e.key.keysym.unicode);
	  fflush(stdout);
	  if (e.key.keysym.sym == SDLK_ESCAPE)
	    break;
	}
    }
}
