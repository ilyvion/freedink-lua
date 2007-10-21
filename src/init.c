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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_framerate.h"
#include "binreloc.h"
/* Msg */
#include "dinkvar.h"
#include "gfx.h"
#include "gfx_fonts.h"
#include "input.h"
#include "io_util.h"
#include "init.h"
#include "freedink_xpm.h"


/* The goal is to replace freedink and freedinkedit's doInit() by a
   common init procedure. This procedure will also initialize each
   subsystem as needed (eg InitSound) */
int init(void)
{
  /* BinReloc */
  BrInitError error;
  if (br_init (&error) == 0 && error != BR_INIT_ERROR_DISABLED)
    {
      printf ("Warning: BinReloc failed to initialize (error code %d)\n", error);
      printf ("Will fallback to hardcoded default path.\n");
    }
  
  /* SDL */
  /* Init timer subsystem */
  if (SDL_Init(SDL_INIT_TIMER) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return 0;
    }

  SDL_initFramerate(&framerate_manager);
  /* The official v1.08 .exe runs 50-60 FPS in practice, despite the
     documented intent of running 83 FPS (or 12ms delay). */
  /* SDL_setFramerate(manager, 83); */
  SDL_setFramerate(&framerate_manager, 60);


  /* TODO: move to gfx.cpp */
  /* Init graphics subsystem */
  if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return 0;
    }

  {
    SDL_Surface *icon = NULL;
    SDL_WM_SetCaption(PACKAGE_STRING, NULL);

    if ((icon = IMG_ReadXPMFromArray(freedink_xpm)) == NULL)
      {
	fprintf(stderr, "Error loading icon: %s\n", IMG_GetError());
      }
    else
      {
	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);
      }
  }

  putenv("SDL_VIDEO_CENTERED=1");

  /* SDL_HWSURFACE is supposed to give direct memory access */
  /* SDL_HWPALETTE makes sure we can use all the colors we need
     (override system palette reserved colors?) */
  /* SDL_DOUBLEBUF is supposed to enable hardware double-buffering
     and is a pre-requisite for SDL_Flip to use hardware, see
     http://www.libsdl.org/cgi/docwiki.cgi/FAQ_20Hardware_20Surfaces_20Flickering */
  if (windowed)
    GFX_lpDDSBack = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF);
  else
    GFX_lpDDSBack = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  if (GFX_lpDDSBack == NULL)
    {
      fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
      exit(1);
    }
  if (GFX_lpDDSBack->flags & SDL_HWSURFACE)
    printf("INFO: Using hardware video mode.\n");
  else
    printf("INFO: Not using a hardware video mode.\n");

  // GFX
  /* GFX_lpDDSBack = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 8, */
  /* 				       0, 0, 0, 0); */

  // lpDDSTwo/Trick/Trick2 are initialized by loading SPLASH.BMP in
  // doInit()

  /* Hide mouse */
  SDL_ShowCursor(SDL_DISABLE);

  /* Disable Alt-Tab and any other window-manager shortcuts */
  /* SDL_WM_GrabInput(SDL_GRAB_ON); */


  /* Fonts system */
  FONTS_init();


  /* Mouse */
  /* Center mouse and reset relative positionning */
  SDL_WarpMouse(320, 240);
  SDL_PumpEvents();
  SDL_GetRelativeMouseState(NULL, NULL);


  /* We'll handle those events manually */
  SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
  SDL_EventState(SDL_VIDEORESIZE, SDL_IGNORE);
  SDL_EventState(SDL_VIDEOEXPOSE, SDL_IGNORE);
  SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
  SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
  SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
  SDL_EventState(SDL_KEYUP, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
  /* We still process through a SDL_PollEvent() loop: */
  /* - SDL_QUIT: quit on window close and Ctrl+C */
  /* - SDL_MOUSEBUTTONDOWN: don't miss quick clicks */
  /* - Joystick: apparently we need to keep them, otherwise joystick
       doesn't work at all */


  /* SDL_MouseMotionEvent: If the cursor is hidden (SDL_ShowCursor(0))
     and the input is grabbed (SDL_WM_GrabInput(SDL_GRAB_ON)), then
     the mouse will give relative motion events even when the cursor
     reaches the edge of the screen. This is currently only
     implemented on Windows and Linux/Unix-alikes. */
  /* So it's not portable and it blocks Alt+Tab, so let's try
     something else - maybe enable it as a command line option. */
  /* SDL_WM_GrabInput(SDL_GRAB_ON); */


  /* Joystick */
  input_init();


  /* Maybe use SDL_QuitSubSystem instead */
  atexit(SDL_Quit);


  /* Engine */
  /* Clean the game state structure - done by C++ but not
     automatically done by C, and this causes errors. TODO: fix the
     errors properly instead of using this dirty trick. */
  memset(&play, 0, sizeof(play));

  memset(&hmap, 0, sizeof(hmap));
  memset(&pam, 0, sizeof(pam));

  return 1;
}
