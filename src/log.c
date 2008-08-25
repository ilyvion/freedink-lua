/**
 * Different ways to give information to the user

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
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

#include "io_util.h"
#include "paths.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char last_debug[200];
/*bool*/int debug_mode = /*false*/0;

void add_text(char *text, char *filename)
{
  if (strlen(text) < 1)
    return;
  
  FILE *fp = paths_dmodfile_fopen(filename, "ab");
  if (fp != NULL)
    {
      fwrite(text, strlen(text), 1, fp); /* current player */
      fclose(fp);
    }
  else
    {
      perror("add_text");
    }
}

void Msg(char *fmt, ...)
{
  char buff[350];
  va_list ap;

  // format message with header
  strcpy(buff, "Dink:");
  va_start(ap, fmt);
  vsprintf(&buff[strlen(buff)], fmt, ap);
  va_end(ap);
  strcat(buff, "\r\n");
  
  // need to reset 'ap' if using it again:
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");

  // displayed on screen if user switches to debug mode
  strcpy(last_debug, buff);

  if (debug_mode)
    add_text(buff, "DEBUG.TXT");
}
