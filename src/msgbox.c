/**
 * Emergency message boxes

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
/* MessageBox */
#include <windows.h>
#else
/* fork, waitpid, execlp */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_image.h"

#include "gfx.h"
#include "gfx_fonts.h"


/**
 * Display a message on the current SDL screen
 */
void msgbox_sdl(char* msg)
{
  SaySmall(msg,
	   0, 0,          /* position */
	   255, 255, 255  /* color */
	   );
  SaySmall("Press any key",
	   0, 400,        /* position */
	   255, 255, 255  /* color */
	   );
  SDL_Flip(GFX_lpDDSBack);
  SDL_Event e;
  while (SDL_WaitEvent(&e))
    {
      if (e.type == SDL_KEYDOWN
	  || e.type == SDL_KEYUP
	  || e.type == SDL_QUIT)
	break;
    }
}


/**
 * Emergency message box for when SDL is not even starting
 */
void msgbox_os(char *msg)
{
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__

  /* WIN32 API */
  MessageBox(hwnd, msg, PACKAGE_NAME, MB_OK);

#else

  /* 'xmessage' basic (and ugly) X utility */
  pid_t pid = 0;
  if ((pid = fork()) < 0)
    perror("fork");
  else if (pid == 0)
    {
      /* child */
      if (execlp("xmessage", "xmessage", msg, NULL) < 0)
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

#endif
}


void msgbox(char* msg)
{
  if (GFX_lpDDSBack != NULL)
    {
      msgbox_sdl(msg);
    }
  else
    {
      msgbox_os(msg);
    }
}


/**
 * Display an error for the user's immediate attention, during
 * initialization (so we can use the SDL window if needed)
 */
void msgbox_init_error(char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  char *buf = NULL;
  vasprintf(&buf, fmt, ap);

  fprintf(stderr, "%s\n", buf);
  msgbox(buf);

  free(buf);
}
