/**
 * Display text using a font from a .zip happened to the executable

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

#include <unistd.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_rwops_libzip.h"

int main (int argc, char *argv[])
{
  SDL_RWops* rwops;
  SDL_Surface *screen;
  TTF_Font *font;

  /* Init */
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  screen = SDL_SetVideoMode(640, 480, 0, SDL_SWSURFACE);

#ifndef _WIN32
  /* Find myself */
  {
    char buf[1024];

    printf("argv[0] = %s\n", argv[0]);

    /* Try to see where /proc/self/exe points. It doesn't exist with
       upx-linux/elf386 (executed from RAM) */
    int n = readlink("/proc/self/exe", buf, 1024-1);
    if (n < 0)
      perror("readlink(/proc/self/exe)");
    else
      {
	buf[n] = '\0';
	printf("buf = %s\n", buf);
      }

    /* Try /proc/self/maps just in case */
    FILE *f = fopen ("/proc/self/maps", "r");
    if (f == NULL)
      perror("fopen(/proc/self/maps)");
    else
      {
	/* The first entry should contain the executable name. */
	char *result = fgets (buf, 1024, f);
	if (result == NULL) {
	  perror("fgets(/proc/self/maps)");
	}
	printf("maps = %s\n", buf);
	fclose (f);
      }
  }
#endif

  /* Load embedded font */
  {
    char myself[1024]; /* bad! */
    strcpy(myself, argv[0]);
    rwops = SDL_RWFromZIP(myself, "LiberationSans-Regular.ttf");
    /* rwops = SDL_RWFromZZIP("embedded_font/LiberationSans-Regular.ttf", "rb"); */
  }

  if (!rwops)
    {
      perror("SDL_RWFromZZIP");
      exit(1);
    }

  /* Display test text */
  {
    SDL_Surface *tmp;
    SDL_Color grey = {127, 127, 127};
    SDL_Rect dst = {280, 220, -1, -1};
    SDL_Event ev;
    font = TTF_OpenFontRW(rwops, 1, 16);
    if (font == NULL)
      {
	fprintf(stderr, "TTF_OpenFontRW: %s\n", TTF_GetError());
	exit(1);
      }
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    tmp = TTF_RenderText_Solid(font, "Hello, world!", grey);
    if (tmp == NULL)
      fprintf(stderr, "TTF_RenderText_Solid: %s\n", TTF_GetError());
    else
      SDL_BlitSurface(tmp, NULL, screen, &dst);
    SDL_Flip(screen);
    while (SDL_WaitEvent(&ev))
      {
	if (ev.type == SDL_KEYDOWN)
	  break;
      }
  }

  /* Clean-up */
  TTF_CloseFont(font);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);

  return 0;
}
