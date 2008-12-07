/**
 * Header for strings functions

 * Copyright (C) 2005  Sylvain Beucler

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

#ifndef _STRING_UTIL_H
#define _STRING_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

  /* TODO: autoconf strncasecmp -> strnicmp/woe */

  extern int string_icompare (char *s1, char *s2);
  extern void strtoupper (char *s);
  extern void strcpy_nooverlap(char *dst, char* src);
  extern int asprintf_append(char **strp, const char* fmt, ...);
  extern void reverse(char *st);
  extern void strchar(char *string, char ch);
  extern /*bool*/int separate_string (char str[255], int num, char liney, char *return1);
  extern /*bool*/int compare(char *orig, char *comp);
  extern void replace(const char *this1, const char *that, char *line);
  extern void latin1_to_utf8(char* source, char* dest, int dest_size);

#ifdef __cplusplus
}
#endif

#endif
