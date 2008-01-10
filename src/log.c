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

void add_text(char *tex ,char *filename)
{
  FILE *fp;
  char *mode = NULL;
  char *fullpath = paths_dmodfile(filename);
  ciconvert(fullpath);
  
  if (strlen(tex) < 1)
    return;
  
  if (exist(fullpath) == /*FALSE*/0)
    mode = "wb";
  else
    mode = "ab";

  if ((fp = fopen(fullpath, mode)) != NULL)
    {
      fwrite( tex, strlen(tex), 1, fp);       /* current player */
      fclose(fp);
    }
  else
    {
      fprintf(stderr, "add_text: cannot write to %s\n", fullpath);
    }
  free(fullpath);
}

void Msg(char *fmt, ...)
{
    char    buff[350];
    va_list  va;

    va_start(va, fmt);

    //
    // format message with header
    //

    strcpy( buff, "Dink:" );
    vsprintf( &buff[strlen(buff)], fmt, va );
    strcat( buff, "\r\n" );

    vfprintf(stderr, fmt, va);
    fprintf(stderr, "\n");
    //
    // To the debugger unless we need to be quiet
    //



/*         OutputDebugString( buff ); */
        strcpy(last_debug, buff);
        if (debug_mode) add_text(buff, "DEBUG.TXT");


} /* Msg */

void TRACE(char *fmt, ...)
{
    char    buff[350];
    va_list  va;

    va_start(va, fmt);

    //
    // format message with header
    //

    strcpy( buff, "Dink:" );
    vsprintf( &buff[strlen(buff)], fmt, va );
    strcat( buff, "\r\n" );

    //
    // To the debugger unless we need to be quiet
    //



/*         OutputDebugString( buff ); */
        strcpy(last_debug, buff);
        if (debug_mode) add_text(buff, "DEBUG.TXT");


} /* Msg */

