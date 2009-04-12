/**
 * Filesystem helpers

 * Copyright (C) 2007, 2008, 2009  Sylvain Beucler

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

#ifndef _IO_UTIL_H
#define _IO_UTIL_H

#include <limits.h>
#include "SDL.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BOOL_1BYTE char /* Boolean value on 1 byte exactly, used to
			   replace the C++ bool type during the C++=>C
			   conversion. Do not change to int, else
			   player_info (among others) will have a
			   different size and savegame format will
			   change! */

  extern void ciconvert (char *filename);
  extern SDL_RWops *find_resource_as_rwops(const char *filename);
  extern /*bool*/int exist(char *name);
  extern int is_directory(char *name);
  extern char* pdirname(char *filename);
  extern void add_text(char *tex ,char *filename);

  extern int read_lsb_int(FILE *f);
  extern void write_lsb_int(int n, FILE *f);
  extern short read_lsb_short(FILE *f);
  extern void write_lsb_short(short n, FILE *f);

#ifdef __cplusplus
}
#endif

#endif
