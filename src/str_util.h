/**
 * Header for strings functions

 * Copyright (C) 2005, 2007, 2008, 2009  Sylvain Beucler

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

  extern void strtoupper(char *s);
  extern void strcpy_nooverlap(char *dst, char* src);
  extern int asprintf_append(char **strp, const char* fmt, ...);
  extern void reverse(char *st);
  extern void strchar(char *string, char ch);
  extern char* separate_string (char* str, int num, char sep);
  extern char* get_word(char* line, int word);
  extern /*bool*/int compare(char *orig, char *comp);
  extern void replace(const char* find, const char* repl, char** line_p);
  extern void replace_norealloc(const char* find, const char* repl, char* line);
  extern char* latin1_to_utf8(char* source);

#ifdef __cplusplus
}
#endif

#endif
