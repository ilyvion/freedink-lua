#include <stdio.h>
#include "SDL.h"

void print_info(const SDL_VideoInfo* info)
{
  printf("hw_available: %d\n", info->hw_available);
  printf("wm_available: %d\n", info->wm_available);
  printf("blit_hw: %d\n", info->blit_hw);
  printf("blit_hw_CC: %d\n", info->blit_hw_CC);
  printf("blit_hw_A: %d\n", info->blit_hw_A);
  printf("blit_sw: %d\n", info->blit_sw);
  printf("blit_sw_CC: %d\n", info->blit_sw_CC);
  printf("blit_sw_A: %d\n", info->blit_sw_A);
  printf("blit_fill: %d\n", info->blit_fill);
  printf("video_mem: %d\n", info->video_mem);
  printf("current_w: %d\n", info->current_w);
  printf("current_h: %d\n", info->current_h);
  printf("Recommended depth: %d\n", info->vfmt->BitsPerPixel);
  SDL_Surface *screen = SDL_GetVideoSurface();
  if (screen != NULL)
      printf("Current depth: %d\n", screen->format->BitsPerPixel);
  printf("\n");
}

int main(void)
{
  const SDL_VideoInfo* info = NULL;
  SDL_Init(SDL_INIT_VIDEO);

  info = SDL_GetVideoInfo();
  print_info(info);

  SDL_SetVideoMode(0, 0, info->vfmt->BitsPerPixel, 0);

  info = SDL_GetVideoInfo();
  print_info(info);

  printf("Requesting 8bit:\n");
  SDL_SetVideoMode(800, 600, 8, 0);

  info = SDL_GetVideoInfo();
  print_info(info);

  printf("Requesting 16bit:\n");
  SDL_SetVideoMode(800, 600, 16, 0);

  info = SDL_GetVideoInfo();
  print_info(info);

  printf("Requesting 24bit:\n");
  SDL_SetVideoMode(800, 600, 24, 0);

  info = SDL_GetVideoInfo();
  print_info(info);

  printf("Requesting 32bit:\n");
  SDL_SetVideoMode(800, 600, 32, 0);

  info = SDL_GetVideoInfo();
  print_info(info);

  SDL_Quit();
  return 0;
}
