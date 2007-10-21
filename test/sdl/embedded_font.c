#include <unistd.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_rwops_zzip.h"

int main (int argc, char *argv[])
{
  SDL_RWops* rwops;
  SDL_Surface *screen;
  TTF_Font *font;

  /* Init */
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  screen = SDL_SetVideoMode(640, 480, 0, SDL_SWSURFACE);

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
  /* Load embedded font */
  {
    char myself[1024]; /* bad! */
    strcpy(myself, argv[0]);
    strcat(myself, "/LiberationSans-Regular.ttf");
    rwops = SDL_RWFromZZIP(myself, "rb");
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
    font = TTF_OpenFontRW(rwops, 1, 17);
    if (font == NULL)
      fprintf(stderr, "TTF_OpenFontRW: %s\n", TTF_GetError());
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
