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

#include "SDL.h"
#include "SDL_ttf.h"
/* Msg */
#include "dinkvar.h"
#include "gfx.h"
#include "init.h"

/* The goal is to replace freedink and freedinkedit's doInit() by a
   common init procedure. This procedure will also initialize each
   subsystem as needed (eg InitSound) */
int init(void) {
  /* Init timer subsystem */
  if (SDL_Init(SDL_INIT_TIMER) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return 0;
    }

  /* TODO: move to gfx.cpp */
  /* Init graphics subsystem */
  if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return 0;
    }

  {
    SDL_Surface *icon;
    SDL_WM_SetCaption("GNU FreeDink", NULL);
    if ((icon = SDL_LoadBMP("../dink.bmp")) == NULL)
      fprintf(stderr, "Error loading ../dink.bmp\n");
    else
      SDL_WM_SetIcon(icon, NULL); /* TODO: support transparency */
  }

  /* TODO: is that portable? */
  putenv("SDL_VIDEO_CENTERED=1");

  /* SDL_HWSURFACE is supposed to give direct memory access */
  /* SDL_HWPALETTE makes sure we can use all the colors we need
     (override system palette reserved colors?) */
  /* SDL_DOUBLEBUF is supposed to enable hardware double-buffering
     and is a pre-requisite for SDL_Flip to use hardware, see
     http://www.libsdl.org/cgi/docwiki.cgi/FAQ_20Hardware_20Surfaces_20Flickering */
  if (windowed)
    GFX_lpDDSPrimary = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF);
  else
    GFX_lpDDSPrimary = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  if (GFX_lpDDSPrimary == NULL)
    {
      fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
      exit(1);
    }
  if (GFX_lpDDSPrimary->flags & SDL_HWSURFACE)
    printf("INFO: Using hardware video mode.\n");
  else
    printf("INFO: Not using a hardware video mode.\n");

  // GFX
  GFX_lpDDSBack = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 8,
  				       0, 0, 0, 0);

  // lpDDSTwo/Trick/Trick2 are initialized by loading SPLASH.BMP in
  // doInit()

  /* Maybe use SDL_QuitSubSystem instead */
  atexit(SDL_Quit);

  /* Hide mouse */
  SDL_ShowCursor(SDL_DISABLE);

  /* Disable Alt-Tab and any other window-manager shortcuts */
  /* SDL_WM_GrabInput(SDL_GRAB_ON); */


  // FONTS
  /* TODO: create a separate initialization procedure */
  TTF_Init();


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


  /* JOY */
  /* Joystick initialization never makes Dink fail for now. */
  /* Note: joystick is originaly only used by the game, not the
     editor. */
  if (joystick == 1)
    {
      if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == -1)
	{
	  Msg("Error initializing joystick, skipping: %s\n", SDL_GetError());
	  joystick = 0;
	}
      else
	{
	  /* first tests if a joystick driver is present */
	  /* if TRUE it makes certain that a joystick is plugged in */
	  if (SDL_NumJoysticks() > 0)
	    {
	      int i;
	      printf("%i joysticks were found.\n", SDL_NumJoysticks());
	      printf("The names of the joysticks are:\n");
	      for (i=0; i < SDL_NumJoysticks(); i++)
		printf("    %s\n", SDL_JoystickName(i));
	      printf("Picking the first one...\n", SDL_NumJoysticks());
	      jinfo = SDL_JoystickOpen(0);
	      /* Don't activate joystick events, Dink polls joystick
		 manually. */
	      /* SDL_JoystickEventState(SDL_ENABLE); */
	      
	      if (jinfo) {
		printf("Name: %s\n", SDL_JoystickName (0));
		printf("Number of axes: %d\n", SDL_JoystickNumAxes(jinfo));
		printf("Number of buttons: %d\n", SDL_JoystickNumButtons(jinfo));
		printf("Number of balls: %d\n", SDL_JoystickNumBalls(jinfo));
		printf("Number of hats: %d\n", SDL_JoystickNumHats(jinfo));
		
		/* Flush stacked joystick events */
		{
		  SDL_Event event;
		  while (SDL_PollEvent(&event));
		}
		
		joystick = 1;
	      } else {
		printf("Couldn't open Joystick 0");
		joystick = 0;
	      }
	    }
	}
    }

  return 1;
}
