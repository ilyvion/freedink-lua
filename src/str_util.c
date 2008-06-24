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

void strchar(char *string, char ch)
/* This acts in the same way as strcat except it combines a string and
a single character, updating the null at the end. */
{
        int last;
        last=strlen(string);
        string[last]=ch;
        string[last+1]=0;

}

/**
 * Split 'str' in words separated by 'liney', and copy the #'num' one
 * to 'return1'. The function does not alter 'str'. Return 1 if field
 * #'num' was present, 0 otherwise.
 */
/*bool*/int separate_string (char str[255], int num, char sep, char *return1)
{
  int l;
  int k;
  int len = 0;

  len = strlen(str);
  l = 1;
  strcpy(return1, "");

  for (k = 0; k < len; k++)
    {
      if (str[k] == sep)
	{
	  if (l == num)
	    break;
	  l++;
	  strcpy(return1, ""); /* reset */
	}
      else /* (str[k] != sep) */
	{
	  char cur_char_as_string[2];
	  cur_char_as_string[0] = str[k];
	  cur_char_as_string[1] = '\0';
	  strcat(return1, cur_char_as_string);
	}
    }

  if (l >= num)
    {
      replace("\r", "", return1); //Take the /r off it.
      replace("\n", "", return1); //Take the /n off it.
      return 1;
    }
  else /* less than 'num' tokens */
    {
      strcpy(return1, "");
      return 0;
    }
}


/**
 * Return whether 'orig' and 'comp' are the same string
 * (case-insensitive comparison)
 */
/*bool*/int compare(char *orig, char *comp)
{

        int len;

        //strcpy(comp, _strupr(comp));
        //strcpy(orig, _strupr(orig));


        len = strlen(comp);
        if (strlen(orig) != len) return(/*false*/0);


        if (strncasecmp(orig,comp,len) == 0)
        {
                return(/*true*/1);
        }

        //Msg("I'm sorry, but %s does not equal %s.",orig, comp);

        return(/*false*/0);
}

void replace(const char *this1, char *that, char *line)
{

        char hold[500];
        char thisup[200],lineup[500];
        int u,i;
        int checker;
start:
        strcpy(hold,"");

        strcpy(lineup,line);
        strcpy(thisup,this1);

        strtoupper(lineup);
        strtoupper(thisup);
        if (strstr(lineup,thisup) == NULL) return;
        checker = -1;
        strcpy(hold,"");
        for (u = 0; u < strlen(line); u++)
        {
                if (checker > -1)
                {
                        if (toupper(line[u]) == toupper(this1[checker]))
                        {
                                if (checker+1 == strlen(this1))
                                {
doit:
                                u = u - strlen(this1);
                                u++;
                                for (i = 0; i < u; i++) hold[i] = line[i];
                                for (i = 0; i < strlen(that); i++) hold[(u)+i]=that[i];
                                hold[strlen(that)+u] = 0;
                                for (i = 0; i < (strlen(line)-u)-strlen(this1); i++)
                                {
                                        hold[(u+strlen(that))+i] = line[(u+strlen(this1))+i];
                                }
                                hold[(strlen(line)-strlen(this1))+strlen(that)] = 0;
                                strcpy(line,hold);
                                goto start;
                                }
                                checker++;
                          } else { checker = -1;    }
                }
                if( checker == -1)
                {
                        if (toupper(line[u]) == toupper(this1[0]))
                        {

                                //      if (line[u] < 97) that[0] = toupper(that[0]);
                                checker = 1;
                                if (strlen(this1) == 1) goto doit;
                        }
                }
        }
}
