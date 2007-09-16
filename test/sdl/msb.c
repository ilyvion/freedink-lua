/**
 * Cursor and app icon mask tests

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"

/* Cursor part based on
   http://www.libsdl.org/docs/html/sdlcreatecursor.html */

static const char *arrow[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c Transparent",
  "I c Invereted",
  /* pixels */
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X.I..X                          ",
  "X.II..X                         ",
  "X.III..X                        ",
  "X.IIII..X                       ",
  "X.I......X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X                        ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X                       ",
  "      X..X                      ",
  "      X..X                      ",
  "       XX                       ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[])
{
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for ( row=0; row<32; ++row ) {
    for ( col=0; col<32; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[5+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case 'I':
          data[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf(image[5+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}


/* Create a mask in MSB for using r,g,b as the transparent color */
Uint8 *create_mask_msb(SDL_Surface *source, Uint8 r, Uint8 g, Uint8 b) {
  Uint32 transparent;
  Uint32 *pixels;
  SDL_Surface *surface;
  Uint8 *mask;

  /* Convert surface to 32bit to ease parsing the pixel data */
  surface = SDL_CreateRGBSurface(SDL_SWSURFACE, source->w, source->h, 32,
			      0xff000000, 0x00ff0000, 0x0000ff00, 0x00000000);
  if(surface == NULL) {
    fprintf(stderr, "Could not convert surface to 32bit: %s", SDL_GetError());
    return NULL;
  }

  SDL_BlitSurface(source, NULL, surface, NULL);

  transparent = SDL_MapRGB(surface->format, r, g, b);

  if (SDL_MUSTLOCK(surface))
    SDL_LockSurface(surface);
  pixels = (Uint32*) surface->pixels;
  if (SDL_MUSTLOCK(surface))
    SDL_UnlockSurface(surface);
    
  /* 8 bits per Uint8 */
  mask = malloc(ceil(surface->w / 8.0) * surface->h);

  { 
    int i, row, col;
    i = -1;
    for (row = 0; row < surface->h; row++)
      {
	for (col = 0; col < surface->w; col++)
	  {
	    /* Shift to the next mask bit */
	    if (col % 8 == 0)
	      {
		i++;
		mask[i] = 0;
	      }
	    else
	      {
		mask[i] <<= 1;
	      }
	    
	    /* Set the current mask bit */
	    if (pixels[row*surface->w + col] != transparent)
	      mask[i] |= 0x01;
	  }
      }
  }
  SDL_FreeSurface(surface);
  return mask;
}


int main(void)
{
  SDL_Surface *screen;

  if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
    {
      fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
      return 1;
    }

  SDL_WM_SetCaption("Cursor test", NULL);
  SDL_SetCursor(init_system_cursor(arrow));

  {
    SDL_Surface *icon;
    Uint8 *mask;

    icon = SDL_LoadBMP("../../share/freedink/dink.bmp");

    mask = create_mask_msb(icon, 255, 255, 0); /* Yellow */
    if (mask != NULL)
      {
	SDL_WM_SetIcon(icon, mask);
	SDL_FreeSurface(icon);
	free(mask);
      }
  }
  
  screen = SDL_SetVideoMode(640, 480, 0, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF);
  SDL_FillRect(screen, NULL,
	       SDL_MapRGB(screen->format, 0, 255, 255));
  SDL_Flip(screen);

  {
    SDL_Event event;
    int quit = 0;
    while (SDL_WaitEvent(&event) && !quit)
      {
	switch(event.type)
	  {
	  case SDL_QUIT:
	    quit = 1;
	    break;
	  case SDL_KEYDOWN:
	    if (event.key.keysym.sym == 'q'
		|| event.key.keysym.sym == SDLK_ESCAPE)
	      quit = 1;
	    break;
	  }
      }
  }
  return 0;
}
