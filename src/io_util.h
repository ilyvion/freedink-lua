/**
 * Free fastfile.cpp replacement prototypes

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

#ifndef _IO_UTIL_H
#define _IO_UTIL_H

#include <limits.h>
#include "SDL.h"
#ifndef PATH_MAX
/* Woe */
#define PATH_MAX MAX_PATH
#endif


#ifdef __cplusplus
extern "C"
{
#endif

  char* ciconvert (char *filename);
  char* ciconvertbuf (const char *filename, char *buf);
  SDL_RWops *find_resource_as_rwops(const char *filename);
  char *find_data_file(const char *filename);

#ifdef __cplusplus
}
#endif

#endif
