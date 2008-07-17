#include <stdio.h>
#include "SDL.h"

int main(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *screen = SDL_SetVideoMode(640, 480, 24, SDL_NOFRAME);
  SDL_Surface *trick2 = SDL_DisplayFormat(screen);

  Uint32
    Rmask = screen->format->Rmask,
    Gmask = screen->format->Gmask,
    Bmask = screen->format->Bmask;
  Uint8
    Rshift = screen->format->Rshift,
    Gshift = screen->format->Gshift,
    Bshift = screen->format->Bshift;
  printf("%x %x %x %x\n", screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
  printf("%d %d %d %d\n", screen->format->Rshift, screen->format->Gshift, screen->format->Bshift, screen->format->Ashift);
  printf("%x %x %x %x\n", trick2->format->Rmask, trick2->format->Gmask, trick2->format->Bmask, trick2->format->Amask);
  printf("%d %d %d %d\n", trick2->format->Rshift, trick2->format->Gshift, trick2->format->Bshift, trick2->format->Ashift);


  SDL_Surface *pic = SDL_LoadBMP("pic.bmp");
  if (pic == NULL)
    {
      fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
      exit(1);
    }

  int update = 1;
  int first = 1;

  int truecolor_fade_lasttick = SDL_GetTicks();
  double truecolor_fade_brightness = 256;
  int process_upcycle = 0;
  int process_downcycle = 1;

  /* How about using a precomputed cache as in v1.08? In practice it
     performs poorly:

[32bits: FPS]
256th, no cache: 55
256th, no cache, loop unwinding: 55
256th, pixel masks: 59
32th, no cache: 55
32th, cache 256<<5: 46
32th, cache [32][256]: 45
32th, cache [256][32]: 40

  //unsigned char cache[32+1][256];
  unsigned char cache[256<<5];
  int i, j;
  for (i = 0; i <= 32; i++)
    {
      for (j = 0; j < 256; j++)
	{
	  cache[(j<<5)|i] = i * j / 32.0;
	  //cache[i][j] = i * j >> 5;
	}
    }
  */

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
      if (update || first)
	{
	  first = 0;
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
	case 16:
	  break;
	case 15:
	  break;
	}
      SDL_UnlockSurface(screen);

      if (update)
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
