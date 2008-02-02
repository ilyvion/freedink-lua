/**
 * Various string functions mainly used for controlling text output

 * Copyright (C) 2003  Shawn Betts
 * Copyright (C) 2004, 2007  Sylvain Beucler

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

#include <ctype.h>
#include <string.h>
#include <stdlib.h> /* free */
#include <stdio.h> /* vasprintf */
#include "str_util.h"

/**
 * Compare two strings ignoring case
 * TODO: autoconfiscate(strcasecmp)
 */
int
string_icompare (char *s1, char *s2)
{
  while (*s1 && *s2 && toupper (*s1) == toupper (*s2))
    {
      s1++;
      s2++;
    }

  return *s1 - *s2;
}

/**
 * Upcase the string
 */
void
strtoupper (char *s)
{
  for (; *s; s++)
    *s = toupper (*s);
}

/**
 * Copy string w/o memory overlap
 */
void strcpy_nooverlap(char *dst, char* src)
{
  char *tmp = strdup(src);
  strcpy(dst, tmp);
  free(tmp);
}


/**
 * Utility - same as asprint, but appends to the specified buffer
 *
 * If strp points to a NULL pointer, it allocates a new buffer that
 * you'll have to free.
 */
int asprintf_append(char **strp, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  if (*strp == NULL)
    {
      *strp = (char*)malloc(1);
      *strp[0] = '\0';
    }

  char *tmp = NULL;
  int result = vasprintf(&tmp, fmt, ap);
  *strp = realloc(*strp, strlen(*strp) + strlen(tmp) + 1);
  strcat(*strp, tmp);
  free(tmp);
  return result;
}
