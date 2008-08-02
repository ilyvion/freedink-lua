/**
 * SDL_stretch test

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

#include <stdio.h>
#include "SDL.h"
#include "SDL_stretch/SDL_stretch.h"

int main(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *screen = SDL_SetVideoMode(640, 480, 8, 0);
  SDL_Surface *bmp = SDL_LoadBMP("/usr/local/share/dink/dink/Tiles/Ts01.bmp");
  printf("%p\n", bmp);
  SDL_Surface *test = SDL_CreateRGBSurface(SDL_SWSURFACE, 9*bmp->w, 9*bmp->h, 8, 0, 0, 0, 0);
  SDL_SetColors(test, bmp->format->palette->colors, 0, 256);
  printf("%p - %dx%d\n", test, test->w, test->h);
  //SDL_Rect src = {250, 50, 100, 100};
  SDL_Rect src = {0, 0, bmp->w, bmp->h};
  /* Only operated on surfaces with exactly the same format :/ */
  SDL_StretchSurfaceRect(bmp, &src, test, NULL);
  //SDL_BlitSurface(bmp, NULL, test, NULL);
  SDL_SaveBMP(test, "/tmp/test.bmp");
  /* We now can check that the scaled image doesn't have the 1-pixel
     posponement that SDL_gfx has */
  SDL_Quit();
  return 0;
}


/**
 * Local Variables:
 * compile-command: "gcc -O0 sdl_stretch.c -o sdl_stretch `sdl-config --cflags --libs` `pkg-config SDL_stretch --cflags --libs`"
 * End:
 */
