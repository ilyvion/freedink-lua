/**
 * Fadedown / fadeup that doesn't touch white pixels

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

unsigned short **cache = NULL;

int main(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *screen = SDL_SetVideoMode(640, 480, 16, SDL_NOFRAME);
  SDL_Surface *trick2 = SDL_DisplayFormat(screen);

  Uint32
    Rmask = screen->format->Rmask,
    Gmask = screen->format->Gmask,
    Bmask = screen->format->Bmask,
    Amask = screen->format->Bmask;
  Uint8
    Rshift = screen->format->Rshift,
    Gshift = screen->format->Gshift,
    Bshift = screen->format->Bshift,
    Ashift = screen->format->Bshift;
  Uint8
    Rloss = screen->format->Rloss,
    Gloss = screen->format->Gloss,
    Bloss = screen->format->Bloss,
    Aloss = screen->format->Bloss;
  printf("SDL depth: %d\n", screen->format->BitsPerPixel);
  printf("%x %x %x %x\n", Rmask, Gmask, Bmask, Amask);
  printf("%d %d %d %d\n", Rshift, Gshift, Bshift, Ashift);
  printf("%d %d %d %d\n", Rloss, Gloss, Bloss, Aloss);

  SDL_Surface *pic = SDL_LoadBMP("pic.bmp");
  if (pic == NULL)
    {
      fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
      exit(1);
    }

  int truecolor_fade_lasttick = SDL_GetTicks();
  double truecolor_fade_brightness = 256;
  int process_upcycle = 0;
  int process_downcycle = 1;
  
  /* How about using a precomputed cache as in v1.08? In practice it
     performs poorly for 24&32bits:
     
[32bits: FPS]
256th, no cache: 55
256th, no cache, loop unwinding: 55
256th, pixel masks: 59
32th, no cache: 55
32th, cache 256<<5: 46
32th, cache [32][256]: 45
32th, cache [256][32]: 40

   unsigned char cache8[32+1][256];
   {
     int i, j;
     for (i = 0; i <= 32; i++)
       {
	 for (j = 0; j < 256; j++)
	   {
	     cache8[i][j] = i * j / 32;
	   }
       }
   }
  */

  /* 2MB cache for 15&16bits - performs quite well:

[16bit: FPS]
if 0xFFFF: 78
decomp RGB: 83
cache <<5: 127
cache [32+1][65536]: 127
cache [65536][32+1]: 118

  */
  int cstart = SDL_GetTicks();
  /* For each discrete fade value, try to make cache values
     contiguous */
  /* 32 _+1_ because in this test we deal with full brightness (in the
     game, we just avoid fading entirely in that case. */
  cache = malloc((32+1)*sizeof(unsigned short*));
  int i, j;
  for (i = 0; i <= 32; i++)
    {
      cache[i] = malloc(65536*sizeof(unsigned short));
      if (screen->format->BitsPerPixel == 16
	   && screen->format->Gmask == 0x07E0)
	{
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
/*   unsigned short cache[(32+1)*65536]; */
/*   int i, j; */
/*   for (i = 0; i <= 32; i++) */
/*     { */
/*       for (j = 0; j < 65536-1; j++) */
/* 	{ */
/* 	  Uint8 r, g, b; */
/* 	  r = (((j&0xF800)>>11)<<3) * i / 32; */
/* 	  g = (((j&0x07E0)>>5)<<2) * i / 32; */
/* 	  b = ((j&0x001F)<<3) * i / 32; */

/* 	  cache[i<<16|j] = ((r>>3)<<11)|((g>>2)<<5)|(b>>3); */
/* 	} */
/*       // keep white; 0xFFFF = 65535 */
/*       cache[i<<16|0xFFFF] = 0xFFFF; */
/*     } */
  printf("Cache computed in: %dms\n", SDL_GetTicks() - cstart);

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


      /* Start real work */
      {
	pos_x ++;
	pos_y ++;
	if (pos_x > 640)
	  pos_x = 0;
	if (pos_y > 480)
	  pos_y = 0;
	SDL_FillRect(trick2, NULL, SDL_MapRGB(trick2->format, 0, 255, 255));
	SDL_Rect square = {200, 200, 240, 80};
	SDL_FillRect(trick2, &square, SDL_MapRGB(trick2->format, 255, 255, 255));
	SDL_Rect square2 = {100, 100, 50, 50};
	SDL_FillRect(trick2, &square2, SDL_MapRGB(trick2->format, 255, 255, 0));
	SDL_Rect square3 = {150, 150, 50, 50};
	SDL_FillRect(trick2, &square3, SDL_MapRGB(trick2->format, 255, 0, 255));
	SDL_Rect square4 = {639, 0, 1, 480};
	SDL_FillRect(trick2, &square4, SDL_MapRGB(trick2->format, 255, 255, 255));
	SDL_Rect dst;
	dst.x = pos_x, dst.y = pos_y;
	SDL_BlitSurface(pic, NULL, trick2, &dst);
	SDL_BlitSurface(trick2, NULL, screen, NULL);
      }
      
      int delta = SDL_GetTicks() - truecolor_fade_lasttick;
      truecolor_fade_lasttick = SDL_GetTicks();
      double incr = delta * 256 / 1000.0;

      if (process_downcycle == 1)
	{
	  truecolor_fade_brightness -= incr;
	  if (truecolor_fade_brightness <= 0)
	    {
	      truecolor_fade_brightness = 0;
	      process_downcycle = 0;
	      process_upcycle = 1;
	    }
	}
      else if (process_upcycle == 1)
	{
	  truecolor_fade_brightness += incr;
	  if (truecolor_fade_brightness >= 256)
	    {
	      truecolor_fade_brightness = 256;
	      process_downcycle = 1;
	      process_upcycle = 0;
	    }
	}
      int brightness = (int)truecolor_fade_brightness;
      //printf("%d\n", brightness);


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
	    Uint32 *p_screen = screen->pixels;
	    int height = screen->h;
	    while (height--)
	      {
		int x;
		for (x = 0; x < screen->w; x++)
		  {
/* 		    Uint8 r, g, b; */
/* 		    r = (((*p_screen)&Rmask)>>Rshift); */
/* 		    g = (((*p_screen)&Gmask)>>Gshift); */
/* 		    b = (((*p_screen)&Bmask)>>Bshift); */
/* 		    if (r != 255 || g != 255 || b != 255) // skip white */
/* 		      *p_screen = */
/* 			((r * brightness >> 8) <<Rshift) */
/* 			|((g * brightness >> 8)<<Gshift) */
/* 			|((b * brightness >> 8)<<Bshift); */
/* 		    p_screen++; */

// Assume that pixel order is RGBA
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		    if (*p_screen != 0x00FFFFFF) // skip white
#else
		    if (*p_screen != 0xFFFFFF00) // TODO: I need a PPC tester for this
#endif
		      {
			*((Uint8*)p_screen)   = *((Uint8*)p_screen)   * brightness >> 8;
			*((Uint8*)p_screen+1) = *((Uint8*)p_screen+1) * brightness >> 8;
			*((Uint8*)p_screen+2) = *((Uint8*)p_screen+2) * brightness >> 8;
		      }
		    p_screen++;

/* 		    *(Uint8*)p_screen = cache8[brightness][*(Uint8*)p_screen]; */
/* 		    *((Uint8*)p_screen+1) = cache8[brightness][*((Uint8*)p_screen+1)]; */
/* 		    *((Uint8*)p_screen+2) = cache8[brightness][*((Uint8*)p_screen+1)]; */
/*  		    p_screen++; */
		  }

		p_screen += remainder;
	      }
	  }
	  break;
	case 24:
	  {
	    /* Progress per byte rather than per pixel */
	    int remainder = screen->pitch - (screen->w * screen->format->BytesPerPixel);
	    Uint8 *p_screen = screen->pixels;
	    int height = screen->h;
	    while (height--)
	      {
		int x;
		for (x = 0; x < screen->w; x++)
		  {
		    /* Don't distinguish r,g,b to avoid mask&shift jobs */
		    Uint8 c1, c2, c3;
		    c1 = *(p_screen);
		    c2 = *(p_screen+1);
		    c3 = *(p_screen+2);
		    if (c1 != 255 || c2 != 255 || c3 != 255) // skip white
		      {
			*(p_screen)   = c1 * brightness >> 8;
			*(p_screen+1) = c2 * brightness >> 8;
			*(p_screen+2) = c3 * brightness >> 8;
		      }
		    p_screen += 3;
		  }
		p_screen += remainder;
	      }
	  }
	  break;
	case 16: /* RGB 565 */
	case 15: /* RGB 555 */
	  {
	    /* Progress per byte rather than per pixel */
	    int remainder = (screen->pitch - (screen->w * screen->format->BytesPerPixel))
	      / screen->format->BytesPerPixel;
	    Uint16 *p_screen = screen->pixels;
	    int height = screen->h;
	    brightness /= 256/32; /* cached units of 32th rather than computed 256th */
	    unsigned short* cur_cache = cache[brightness];
	    while (height--)
	      {
		int x;
		for (x = 0; x < screen->w; x++)
		  {
/* 		    if (*p_screen != 0xFFFF) // skip white */
/* 		      { */
/* 			Uint8 r, g, b; */
/* 			r = (((*p_screen&0xF800)>>11)<<3) * brightness >> 8; */
/* 			g = (((*p_screen&0x07E0)>>5)<<2) * brightness >> 8; */
/* 			b = ((*p_screen&0x001F)<<3) * brightness >> 8; */
/* 			*p_screen = ((r>>3)<<11)|((g>>2)<<5)|(b>>3); */
/* 		      } */

/* 		    Uint8 r, g, b; */
/* 		    r = (((*p_screen&0xF800)>>11)<<3); */
/* 		    g = (((*p_screen&0x07E0)>>5)<<2); */
/* 		    b = ((*p_screen&0x001F)<<3); */
/* 		    //printf("%d %d %d\n", r, g, b); */
/* 		    if (r != 255 || g != 255 || b != 255) // skip white // doesn't work */
/* 		      *p_screen = ((r * brightness >> 8>>3)<<11)|((g * brightness >> 8>>2)<<5)|(b * brightness >> 8>>3); */

/* 		    *p_screen = cache[brightness<<16|*p_screen]; */
		    *p_screen = cur_cache[*p_screen];


		    p_screen ++;
		  }
		p_screen += remainder;
	      }
	  }
	  break;
	}
      SDL_UnlockSurface(screen);

      SDL_Flip(screen);

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
