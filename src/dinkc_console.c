/**
 * DinkC interactive console

 * Copyright (C) 2005, 2006  Dan Walma
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

#include "dinkc_console.h"

#include <string.h>
#include "str_util.h"
#include "dinkc.h"

#define NB_LINES 20
#define MAX_LINE_LEN 254

/* Console mode */
int console_active = 0;


/* Last 20 commands entered in the console */
static char history[NB_LINES][MAX_LINE_LEN+1];

/* Current history index */
static int cur_line = 0;

/* Result of the last evaluation */
static int console_return_value = 0;


void dinkc_console_process_key(SDL_KeyboardEvent kev)
{
  if (kev.keysym.sym == SDLK_UP)
    {
      cur_line--;
      /* Using (a+N)%N instead of a%N to avoid negative results */
      cur_line = (cur_line + NB_LINES) % NB_LINES;
    }
  else if(kev.keysym.sym == SDLK_DOWN)
    {
      cur_line++;
      cur_line %= NB_LINES;
    }
  else if (kev.keysym.sym == SDLK_BACKSPACE)
    {
      /* Delete last char */
      int len = strlen(history[cur_line]);
      if (len > 0)
	history[cur_line][len-1] = '\0';
    }
  else if (kev.keysym.sym == SDLK_ESCAPE)
    {
      console_active = 0;
    }
  else if (kev.keysym.unicode == SDLK_RETURN)
    {
      /* Try to parse the string */
      console_return_value = dinkc_execute_one_liner(history[cur_line]);

      /* Go the next line */
      cur_line++;
      cur_line %= NB_LINES;
      int len = strlen(history[cur_line]);
      if (len > 0)
	history[cur_line][0] = '\0';
    }
  else if (kev.keysym.unicode != 0)
    {
      /* Append character to the current line */
      if (strlen(history[cur_line]) < MAX_LINE_LEN)
	strchar(history[cur_line], kev.keysym.unicode);
    }
}

char* dinkc_console_get_cur_line()
{
  return history[cur_line];
}

int dinkc_console_get_last_retval()
{
  return console_return_value;
}
