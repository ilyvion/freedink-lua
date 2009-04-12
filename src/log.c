/**
 * Different ways to give information to the user

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "io_util.h"
#include "paths.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

char last_debug[200];
int debug_mode = 0;
static enum log_priority cur_priority  = LOG_PRIORITY_ERROR;
static enum log_priority orig_priority = LOG_PRIORITY_ERROR;
FILE* out = NULL;

char* priority_names[LOG_PRIORITY_OFF] = {
  "", /* ALL */
  "[trace] ",
  "[debug] ",
  "[info ] ",
  "[warn ] ",
  "[ERROR] ",
  "[FATAL] ",
};

void log_debug_on()
{
  debug_mode = 1;
  orig_priority = cur_priority;
  log_set_priority(LOG_PRIORITY_DEBUG);
  out = paths_dmodfile_fopen("DEBUG.TXT", "ab");
  /* out might be NULL, e.g. permissions problem */
  if (out != NULL)
    {
      /* setlinebuf(stdout); */
      /* This one is equivalent and works with mingw: */
      setvbuf(stdout, NULL, _IOLBF, 0);
    }
}

void log_debug_off()
{
  strcpy(last_debug, "");
  cur_priority = orig_priority;
  if (out != NULL)
    fclose(out);
  out = NULL;
  debug_mode = 0;
}

void log_set_priority(enum log_priority priority)
{
  if (cur_priority >= LOG_PRIORITY_ALL && cur_priority <= LOG_PRIORITY_OFF)
    cur_priority = priority;
}

void log_output(enum log_priority priority, char *fmt, ...)
{
  if (priority < cur_priority
      || priority <= LOG_PRIORITY_ALL
      || priority >= LOG_PRIORITY_OFF)
    return;

  char* buf = NULL;

  // format message
  va_list ap;
  va_start(ap, fmt);
  int res = vasprintf(&buf, fmt, ap);
  va_end(ap);
  if (res < 0)
    return;

  // displayed on screen if user switches to debug mode
  if (debug_mode)
    {
      strcpy(last_debug, priority_names[priority]);
      strncat(last_debug, buf, sizeof(last_debug) - strlen(priority_names[priority]) - 1);
    }

  // write to DEBUG.TXT
  if (out != NULL)
    {
      fputs(priority_names[priority], out);
      fputs(buf, out);
      fputc('\n', out);
    }

  // write to standard output
  fputs(priority_names[priority], stdout);
  fputs(buf, stdout);
  putchar('\n');

  free(buf);
}
