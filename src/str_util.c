/**
 * Various string functions mainly used for controlling text output

 * Copyright (C) 2003  Shawn Betts
 * Copyright (C) 2004, 2007, 2008  Sylvain Beucler

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
#include <strings.h> /* strcasecmp */
#include <stdlib.h> /* free */
#include <stdarg.h> /* va_start */
#include <stdio.h> /* vasprintf */
#include <xalloc.h>
#include "str_util.h"
#include "log.h"

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

  if (*strp == NULL)
    {
      *strp = (char*)malloc(1);
      *strp[0] = '\0';
    }

  char *tmp = NULL;
  va_start(ap, fmt);
  int result = vasprintf(&tmp, fmt, ap);
  va_end(ap);

  *strp = realloc(*strp, strlen(*strp) + strlen(tmp) + 1);
  strcat(*strp, tmp);
  free(tmp);
  return result;
}

void reverse(char *st)
{
        int i,ii;
        char don[255];
        don[0] = 0;
        ii = strlen(st);
        for (i=ii; i > -1; i--)
        {
                strchar(don, st[i]);
        }
        strcpy(st, don);
}

/**
 * This acts in the same way as strcat except it combines a string and
 * a single character, updating the null at the end.
 */
void strchar(char *string, char ch)
{
  int last = strlen(string);
  string[last] = ch;
  string[last+1] = '\0';
}

/**
 * Split 'str' in words separated by _one_ 'sep', and copy the #'num'
 * one to 'return1'. The function does not alter 'str'. Return empty
 * string is not found. Several 'sep' enclose empty words
 * (e.g. separators are not collapsed, unlike 'get_word(...)').
 */
char* separate_string (char* line, int num, char sep)
{
  int l;
  int k;
  int line_len = strlen(line);

  l = 1;
  int start = 0;
  int end = 0;

  for (k = 0; k < line_len; k++)
    {
      if (line[k] == sep)
	{
	  if (l == num)
	    break;
	  l++;
	  start = end = (k + 1);
	}
      else /* (str[k] != sep) */
	{
	  end++;
	}
    }

  if (l >= num)
    {
      int size = end - start;
      char* result = xmalloc(size + 1);
      strncpy(result, line + start, size);
      result[size] = '\0';

      replace_norealloc("\r", "", result); //Take the /r off it.
      replace_norealloc("\n", "", result); //Take the /n off it.
      return result;
    }
  else /* less than 'num' tokens */
    {
      return strdup("");
    }
}

/**
 * Return the word number 'word' present in 'line'. If not present,
 * returns an empty string. Words are separated by one _or more_
 * spaces and count from 1 (i.e. not 0).
 */
char* get_word(char* line, int word)
{
  int cur_word = 1;

  /* find word */
  char* pc = line;
  while (*pc != '\0')
    {
      if (cur_word == word)
	break;
      if (*pc == ' ')
	{
	  cur_word++;
	  while(*pc == ' ' && *pc != '\0')
	    pc++;
	}
      else
	{
	  while(*pc != ' ' && *pc != '\0')
	    pc++;
	}
    }

  /* find end-of-word */
  char* start = pc;
  while(*pc != '\0' && *pc != ' ')
    pc++;

  /* copy word - either we're on the right word and will copy it,
     either we're at the end of string and will copy an empty word */
  int len = pc - start;
  char* result = xmalloc(len + 1);
  memcpy(result, start, len);
  result[len] = '\0';

  return result;
}


/**
 * Return whether 'orig' and 'comp' are the same string
 * (case-insensitive comparison).
 */
/*bool*/int compare(char *orig, char *comp)
{
  return (strcasecmp(orig, comp) == 0);
}

/**
 * Move chars between 'start' and the end of 'line' to the left, with
 * a postponement of 'shift' chars. Copy the trailing '\0'.
 */
static void shift_left(char* line, int start, int shift)
{
  /* Beware of the direction so as not to overwrite */
  int i = start;
  int max = strlen(line);
  for (; i <= max; i++)
    line[i-shift] = line[i];
}

/**
 * Move chars between 'start' and the end of 'line' to the right, with
 * a postponement of 'shift' chars. Copy the trailing '\0'.
 */
static void shift_right(char* line, int start, int shift)
{
  /* Beware of the direction so as not to overwrite */
  int i = strlen(line);
  for (; i >= start; i--)
    line[i+shift] = line[i];
}

/**
 * Replace word 'find' by word 'repl' in 'line', as many times as
 * possible.
 * 
 * Note:
 * 
 * - '*line_p' is xrealloc'd only if strlen(repl) > strlen(find), to
      make the calling code easier.
 */
void replace(const char* find, const char* repl, char** line_p)
{
  int len_find = strlen(find);
  int len_repl = strlen(repl);
  char* line = *line_p;
  int len_line = strlen(line);
  
  int u = -1;
  int checker = 0;
  for (u = 0; u < len_line; u++)
    {
      if (toupper(line[u]) == toupper(find[checker]))
	{
	  checker++;
	  if (checker == len_find)
	    {
	      int pos_repl = u + 1 - len_find;

	      int len_newline = len_line + len_repl - len_find;
	      /* Only change line_p if there's need to */
	      if (len_newline > len_line)
		{
		  line = xrealloc(line, len_newline + 1);
		  *line_p = line;
		}

	      /* Move what's after the replacement, if necessary */
	      if (len_repl < len_find)
		shift_left (line, pos_repl + len_find, len_find - len_repl);
	      else if (len_repl > len_find)
		shift_right(line, pos_repl + len_find, len_repl - len_find);

	      /* Actually replace */
	      strncpy(line + pos_repl, repl, len_repl);

	      /* Prepare for next loop */
	      checker = 0;
	      len_line += len_repl - len_find;
	      u += len_repl - len_find;
	    }
	}
      else
	{
	  checker = 0;
	}
    }
}

/**
 * Alias to 'replace' that emphasize that '&line' won't be modified
 * (i.e. realloc'd). Only valid if the replacement is shorter or as
 * long as the search term.
 */
void replace_norealloc(const char* find, const char* repl, char* line)
{
  if (strlen(repl) > strlen(find))
    {
      log_fatal("Internal error: invalid string substitution");
      exit(EXIT_FAILURE);
    }
  replace(find, repl, &line);
}


/**
 * Convert Latin-1-encoded 'source' to UTF-8-encoded. Result will
 * always be NULL-terminated.
 */
char* latin1_to_utf8(char* source)
{
  int cur_alloc_size = 512;
  const int step = 256;
  unsigned char* dest = xmalloc(cur_alloc_size);
  unsigned char* pcs = (unsigned char*) source;
  unsigned char* pcd = (unsigned char*) dest;
  unsigned char* pcd_limit = pcd + cur_alloc_size;
  while(*pcs != '\0')
    {
      if (pcd == pcd_limit)
	{
	  cur_alloc_size += step;
	  dest = xrealloc(dest, cur_alloc_size);
	  pcd = dest + cur_alloc_size - step;
	  pcd_limit = pcd + step;
	}
      if (*pcs < 128)
	{
	  *pcd = *pcs;
	  pcs++;
	  pcd++;
	}
      else
	{
	  *pcd = 0xc2 + ((*pcs - 128) / 64);
	  pcd++;
	  *pcd = 0x80 + ((*pcs - 128) % 64);
	  pcd++;
	  pcs++;
	}
    }
  *pcd = '\0';
  return (char*)dest;
}

/* Here's a small Python script to explain the above formula: */

// # Static charset conversion table from Latin-1 to UTF-8:
// print 'unsigned char conv[][2] = {\n', ',\n'.join(
//     ['\/*%d:\*/ {%s}' % (
//         c,
//         ', '.join(
//             [hex(ord(i)) for i in chr(c).decode('ISO-8859-1').encode('utf-8')]
//             )
//         ) for c in range(128,256)]
//     ), '};'
// 
// # Test computed (!= static) table:
// for c in range(128,256):
//     method1 = [ord(i) for i in chr(c).decode('ISO-8859-1').encode('utf-8')]
//     method2 = [0xc2 + ((c - 128) / 64), 0x80 + ((c - 128) % 64)]
//     #print method1, method2
//     if method1[0] != method2[0] or method1[1] != method2[1]:
//         print "Mismatch at %c"
