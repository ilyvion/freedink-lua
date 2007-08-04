/**
 * System initialization, common to FreeDink and FreeDinkEdit

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
/* Msg */
#include "dinkvar.h"
#include "gfx.h"
#include "init.h"

/* The goal is to replace freedink and freedinkedit's doInit() by a
   common init procedure. This procedure will also initialize each
   subsystem as needed (eg InitSound) */
int init(void) {
  /* Init timer subsystem */
  if (SDL_Init(SDL_INIT_TIMER) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return 0;
    }

  /* TODO: move to gfx.cpp */
  /* Init graphics subsystem */
  if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return 0;
    }

  SDL_Surface *test_image;
  
  // GFX_lpDDSPrimary = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF);
  if (windowed)
    {
      GFX_lpDDSPrimary = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE);
      if (GFX_lpDDSPrimary == NULL) {
	fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
	exit(1);
      }
    }
  else
    {
      GFX_lpDDSPrimary = SDL_LoadBMP("tiles/SPLASH.BMP");
    }

  // GFX
  //GFX_lpDDSBack = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 8,
  //				       0, 0, 0, 0);
  /* I can't find a way to make a manually created SDL_Surface
     work. That's pretty weird. So let's initialize it from a
     BMP... */
  GFX_lpDDSBack = SDL_LoadBMP("tiles/SPLASH.BMP");

  // lpDDSTwo/Trick/Trick2 are initialized by loading SPLASH.BMP in
  // doInit()


  // DEBUG
  /* Load the BMP file into a surface */
//   test_image = SDL_LoadBMP("C:/Program Files/Dink Smallwood/dink/graphics/Dink/fall/ds-f2-01.bmp");
//   if (test_image == NULL) {
//     fprintf(stderr, "Couldn't load image: %s\n", SDL_GetError());
//   }
//   SDL_BlitSurface(test_image, NULL, GFX_lpDDSPrimary, NULL);
//   SDL_Flip(GFX_lpDDSPrimary);

  /* Maybe use SDL_QuiSubSystem instead */
  atexit(SDL_Quit);

  return 1;
}
