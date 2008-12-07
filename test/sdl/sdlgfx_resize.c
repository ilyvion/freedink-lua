/**
 * Benchmark to check new SDL_gfx resize algorithms

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
#include "SDL_rotozoom.h"

int main(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *screen = SDL_SetVideoMode(640, 480, 16, SDL_NOFRAME);
  SDL_Surface *trick2 = SDL_DisplayFormat(screen);

  SDL_Surface *pic = SDL_LoadBMP("pic.bmp");
  if (pic == NULL)
    {
      fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
      exit(1);
    }

  int resize_lasttick = SDL_GetTicks();
  double resize_brightness = 256;
  
  int fps = 0;
  int fps_lasttick = SDL_GetTicks();

  int pos_x = 0;
  int pos_y = 0;
  

  int quit = 0;
  while (!quit)
    {
      SDL_Event event;
      SDL_PollEvent(&event);
      {
	switch(event.type)
	  {
	  case SDL_KEYDOWN:
	    switch (event.key.keysym.sym)
	      {
	      case 'q':
	      case SDLK_ESCAPE:
		quit = 1;
		break;
	      case 'f':
		SDL_WM_ToggleFullScreen(screen);
		break;
	      default:
		break;
	      }
	    break;
	    
	  case SDL_QUIT:
	    quit = 1;
	    break;
	  }
      }

      

      int delta = SDL_GetTicks() - resize_lasttick;
      resize_lasttick = SDL_GetTicks();
      //double incr = delta * 256 / 1000.0;

      //SDL_Flip(screen);
      SDL_Surface *scaled = zoomSurface(pic, 3, 3, SMOOTHING_OFF);
      SDL_FreeSurface(scaled);

      fps++;
      if ((SDL_GetTicks() - fps_lasttick) > 1000)
	{
	  printf("FPS: %d\n", fps);
	  fps_lasttick = SDL_GetTicks();
	  fps = 0;
	}
    }

  SDL_Quit();
  return 1;
}

/**
 * Local Variables:
 * compile-command: "gcc -O0 sdlgfx_resize.c -o sdlgfx_resize `sdl-config --cflags --libs` -lSDL_gfx"
 * End:
 */
