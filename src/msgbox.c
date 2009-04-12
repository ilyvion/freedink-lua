/**
 * Emergency message boxes

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

#include <stdio.h>
#include <stdlib.h>

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
/* MessageBox */
#include <windows.h>
#else
#  ifdef HAVE_EXECLP
/* fork, waitpid, execlp */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#  endif
#endif

#include "SDL.h"
#include "SDL_image.h"

#include "gfx.h"
#include "gfx_fonts.h"
#include "game_engine.h"

/**
 * Display a message on the current SDL screen
 */
void msgbox_sdl(char* msg)
{
  SDL_FillRect(GFX_lpDDSBack, NULL, SDL_MapRGB(GFX_lpDDSBack->format, 100, 100, 100));

  {
    int margin_h = 50;
    int margin_v = 20;
    rect dst = {margin_h, margin_v, 640-margin_h, margin_v+20}; /* top line */
    FONTS_SetTextColor(255, 255, 255); /* white */
    print_text_wrap("FreeDink failed to start!", &dst, 1, 0, FONT_SYSTEM);
  }
  {
    int margin_h = 50;
    int margin_v = 100;
    SDL_Rect dst2 = {margin_h-1, margin_v-1, 640-(margin_h*2)+2, 480-(margin_v*2)+2};
    SDL_FillRect(GFX_lpDDSBack, &dst2, SDL_MapRGB(GFX_lpDDSBack->format, 200, 200, 200));
    SDL_Rect dst3 = {margin_h, margin_v, 640-(margin_h*2), 480-(margin_v*2)};
    SDL_FillRect(GFX_lpDDSBack, &dst3, SDL_MapRGB(GFX_lpDDSBack->format, 255, 255, 255));

    /* Display error message */
    int border = 5;
    rect dst = {margin_h+border, margin_v+border, 640-margin_h-border, 480-margin_v-border}; /* centered with margin */
    FONTS_SetTextColor(0, 0, 0); /* black */
    if (msg != NULL)
      print_text_wrap(msg, &dst, 0, 0, FONT_SYSTEM);
  }
  {
    int margin_h = 50;
    int margin_v = 20;
    rect dst = {margin_h, 480-margin_v-20, 640-margin_h, 480-margin_v}; /* bottom line */
    FONTS_SetTextColor(255, 255, 255); /* white */
    print_text_wrap("Press ESC to exit", &dst, 1, 0, FONT_SYSTEM);
  }

  SDL_Flip(GFX_lpDDSBack);

  /* Wait for user to press a key */
  if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0
      && SDL_InitSubSystem(SDL_INIT_JOYSTICK) != -1
      && SDL_NumJoysticks() > 0)
    jinfo = SDL_JoystickOpen(0);
  if (jinfo != NULL)
    SDL_JoystickEventState(SDL_ENABLE);

  SDL_Event e;
  while (SDL_WaitEvent(&e))
    {
      if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
	{
	  if (e.key.keysym.sym == SDLK_ESCAPE
	      || e.key.keysym.sym == SDLK_SPACE
	      || e.key.keysym.sym == SDLK_RETURN
	      || e.key.keysym.sym == 'q')
	    break;
	}
      else if (e.type == SDL_QUIT)
	{
	  break;
	}
      else if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP)
	{
	  break;
	}
    }
}


/**
 * Emergency message box for when SDL is not even starting
 */
void msgbox_os(char *msg)
{
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__

  /* WIN32 API */
  MessageBox(NULL, msg, PACKAGE_NAME, MB_OK);

#else
#  ifdef HAVE_EXECLP
  /* 'xmessage' basic (and ugly) X utility */
  pid_t pid = 0;
  if ((pid = fork()) < 0)
    perror("fork");
  else if (pid == 0)
    {
      /* child */
      /* Don't display xmessage errors, this would be misleading */
      fclose(stdout);
      fclose(stderr);
      if (execlp("xmessage", "xmessage", "-center", "-buttons", "OK:0", msg, NULL) < 0)
	perror("execlp");
      exit(EXIT_FAILURE);
    }
  else
    {
      /* father */
      pid_t child_pid = pid;
      int status = 0;
      waitpid(child_pid, &status, 0);
    }
#  else
  /* Add more OS-specific fallbacks here. */
#  endif
#endif
}


void msgbox(char* msg)
{
  /* Try initializing graphics if not already */
  int graphics_on = 0;
  switch (gfx_get_init_state())
    {
    /* No screen */
    case GFX_NOT_INITIALIZED:
    case GFX_INITIALIZING_VIDEO:
      graphics_on = !(gfx_init_failsafe() < 0);
      break;

    /* Screen initialized, no fonts */
    case GFX_INITIALIZING_FONTS:
      graphics_on = !(gfx_fonts_init_failsafe() < 0);
      break;

    /* Screen and fonts initialized */
    case GFX_INITIALIZED:
      graphics_on = 1;
      break;

    /* Unknown state, internal error */
    default:
      graphics_on = 0;
      break;
    }

  /* Display a SDL message box if possible, otherwise fall back to a
     system message box */
  if (graphics_on)
    msgbox_sdl(msg);
  else
    msgbox_os(msg);
}


/**
 * Display an error for the user's immediate attention, during
 * initialization (so we can use the SDL window if needed)
 */
void msgbox_init_error(char* fmt, ...)
{
  va_list ap;

  char *buf = NULL;
  va_start(ap, fmt);

  if (fmt == NULL)
    fmt = "Unknown error!\n"
      "This means there's an internal error in FreeDink.\n"
      "Please report this bug to " PACKAGE_BUGREPORT " .";
  vasprintf(&buf, fmt, ap);
  va_end(ap);

  fprintf(stderr, "%s\n", buf);
  msgbox(buf);

  free(buf);
}
