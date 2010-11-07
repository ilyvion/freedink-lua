#include "SDL.h"
int main(int argc, char* argv[]) {
  SDL_Surface* screen = SDL_SetVideoMode(640, 480, 8, SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_HWPALETTE);
  SDL_Surface* refpal = SDL_LoadBMP("pic.bmp");
  SDL_Surface* bnwpal = SDL_LoadBMP("bnw.bmp");

  /* Normal blit */
  SDL_SetPalette(screen, SDL_LOGPAL,  refpal->format->palette->colors, 0, 256);
  SDL_SetPalette(screen, SDL_PHYSPAL, refpal->format->palette->colors, 0, 256);
  SDL_BlitSurface(refpal, NULL, screen, NULL);
  SDL_Flip(screen);
  SDL_Delay(1000);

  /* Blit on B&W physical palette: everything is B&W */
  SDL_SetPalette(screen, SDL_PHYSPAL, bnwpal->format->palette->colors, 0, 256);
  SDL_BlitSurface(refpal, NULL, screen, NULL);
  SDL_Flip(screen);
  SDL_Delay(1000);

  /* Dither (DisplayFormat) when screen physical palette is changed:
     screen logical palette (same as pic.bmp) is used, no change */
  SDL_Surface* testorig = SDL_LoadBMP("pic.bmp");
  SDL_SetPalette(screen, SDL_PHYSPAL, bnwpal->format->palette->colors, 0, 256);
  SDL_Surface* testblit = SDL_DisplayFormat(testorig);
  SDL_SetPalette(screen, SDL_PHYSPAL, refpal->format->palette->colors, 0, 256);
  SDL_BlitSurface(testblit, NULL, screen, NULL);
  SDL_Flip(screen);
  SDL_Delay(1000);
  SDL_FreeSurface(testblit);

  /* Dither (DisplayFormat) when screen logical palette is changed:
     screen logical palette (same as bnw.bmp) is used, the heart is
     B&W, rest of the screen unchanged */
  SDL_SetPalette(screen, SDL_LOGPAL, bnwpal->format->palette->colors, 0, 256);
  testblit = SDL_DisplayFormat(testorig);
  SDL_SetPalette(screen, SDL_PHYSPAL, refpal->format->palette->colors, 0, 256);
  SDL_BlitSurface(testblit, NULL, screen, NULL);
  SDL_Flip(screen);
  SDL_Delay(1000);
  SDL_FreeSurface(testblit);
}

/**
 * Local Variables:
 * compile-command: "gcc -g paltest.c `sdl-config --cflags --libs` -lSDL_mixer"
 * End:
 */
