/**
 * Fade down / fade up, preserving white pixels

 * Copyright (C) 2008, 2009  Sylvain Beucler

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gfx_fade.h"

#include <stdlib.h>
#include "SDL.h"
#include "gfx.h"

/* 2MB cache - performs quite well for 15&16bits*/
static unsigned short **cache = NULL;

/**
 * Init cache
 */
void gfx_fade_init()
{
  SDL_Surface *screen = GFX_lpDDSBack;

  if (screen->format->BitsPerPixel != 15
      && screen->format->BitsPerPixel != 16)
    return;

  /* For each discrete fade value, try to make cache values
     contiguous, so [32][65536] rather than [65536][32] */
  cache = malloc(32*sizeof(unsigned short*));

  int i, j;
  for (i = 0; i < 32; i++)
    {
      if (screen->format->BitsPerPixel == 16
	   && screen->format->Gmask == 0x07E0)
	{
	  /* RGB565 */
	  cache[i] = malloc(65536*sizeof(unsigned short));
	  for (j = 0; j < 65536-1; j++)
	    {
	      Uint8 r, g, b;
	      r = (((j&0xF800)>>11)<<3) * i / 32;
	      g = (((j&0x07E0)>>5)<<2) * i / 32;
	      b = ((j&0x001F)<<3) * i / 32;
	      cache[i][j] = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
      	    }
	  // keep white; 0xFFFF = 11111 111111 11111 = 65535
	  cache[i][0xFFFF] = 0xFFFF;
	}
      else if ((screen->format->BitsPerPixel == 15)
	       || (screen->format->BitsPerPixel == 16
		   && screen->format->Gmask == 0x03E0))
	{
	  /* RGB555 */
	  /* In 15bits, SDL returns bbp=_16_ with mask 0x03E0 */
	  cache[i] = malloc(32768*sizeof(unsigned short));
	  for (j = 0; j < 32768-1; j++)
	    {
	      Uint8 r, g, b;
	      r = (((j&0x7C00)>>10)<<3) * i / 32;
	      g = (((j&0x03E0)>>5)<<3) * i / 32;
	      b = ((j&0x001F)<<3) * i / 32;
	      cache[i][j] = ((r>>3)<<10)|((g>>3)<<5)|(b>>3);
	    }
	  // keep white; 0x7FFF = 0 11111 11111 11111 = 32767
	  cache[i][0x7FFF] = 0x7FFF;
	}
    }
}

void gfx_fade_quit()
{
  if (cache != NULL)
    {
      int i;
      for (i = 0; i < 32; i++)
	free(cache[i]);
      free(cache);
    }
}

void gfx_fade_apply(int brightness)
{
  SDL_Surface *screen = GFX_lpDDSBack;
  SDL_LockSurface(screen);
  /* Check SDL_blit.h in the SDL source code for guidance */
  switch (screen->format->BitsPerPixel)
    {
    case 32:
      {
	/* Progress per pixel rather than per byte */
	int remainder = (screen->pitch - (screen->w * screen->format->BytesPerPixel))
	  / screen->format->BytesPerPixel;
	/* Using aligned Uint32 is faster than working with Uint8 values */
	Uint32 *p = screen->pixels;
	int height = screen->h;
	while (height--)
	  {
	    int x;
	    for (x = 0; x < screen->w; x++)
	      {
		/* Assume that pixel order is RGBA */
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		if (*p != 0x00FFFFFF) // skip white
#else
		if (*p != 0xFFFFFF00) // TODO: I need a PPC tester for this
#endif
		  {
		    *((Uint8*)p)   = *((Uint8*)p)   * brightness >> 8;
		    *((Uint8*)p+1) = *((Uint8*)p+1) * brightness >> 8;
		    *((Uint8*)p+2) = *((Uint8*)p+2) * brightness >> 8;
		  }
		p++;
	      }
	    p += remainder;
	  }
      }
      break;
    case 24:
      {
	/* Progress per byte rather than per pixel (can't use Uint_32_ since bbp=24) */
	int remainder = screen->pitch - (screen->w * screen->format->BytesPerPixel);
	Uint8 *p = screen->pixels;
	int height = screen->h;
	while (height--)
	  {
	    int x;
	    for (x = 0; x < screen->w; x++)
	      {
		/* Don't distinguish r,g,b to avoid mask&shift jobs */
		Uint8 c1, c2, c3;
		c1 = *(p);
		c2 = *(p+1);
		c3 = *(p+2);
		if (c1 != 255 || c2 != 255 || c3 != 255) // skip white
		  {
		    *(p)   = c1 * brightness >> 8;
		    *(p+1) = c2 * brightness >> 8;
		    *(p+2) = c3 * brightness >> 8;
		  }
		p += 3;
	      }
	    p += remainder;
	  }
      }
      break;
    case 16: /* RGB 565 */
    case 15: /* RGB 555 */
      {
	/* Progress per byte rather than per pixel */
	int remainder = (screen->pitch - (screen->w * screen->format->BytesPerPixel))
	  / screen->format->BytesPerPixel;
	Uint16 *p = screen->pixels;
	int height = screen->h;
	brightness /= 256/32; /* cached units of 32th rather than computed 256th */
	unsigned short* cur_cache = cache[brightness];
	while (height--)
	  {
	    int x;
	    for (x = 0; x < screen->w; x++)
	      {
		*p = cur_cache[*p];
		p ++;
	      }
	    p += remainder;
	  }
      }
      break;
    }
  SDL_UnlockSurface(screen);
}
