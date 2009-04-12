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

#ifndef _LOG_H
#define _LOG_H

extern char last_debug[200];
extern int debug_mode;

enum log_priority
  {
    LOG_PRIORITY_ALL,   // catch-all
    LOG_PRIORITY_TRACE, // verbose debug
    LOG_PRIORITY_DEBUG, // for devs
    LOG_PRIORITY_INFO,  // about program execution
    LOG_PRIORITY_WARN,  // possible error
    LOG_PRIORITY_ERROR, // recoverable error
    LOG_PRIORITY_FATAL, // unrecoverable error
    LOG_PRIORITY_OFF    // don't log anything
  };

#define log_trace(...) log_output(LOG_PRIORITY_TRACE, __VA_ARGS__)
#define log_debug(...) log_output(LOG_PRIORITY_DEBUG, __VA_ARGS__)
#define log_info(...)  log_output(LOG_PRIORITY_INFO,  __VA_ARGS__)
#define log_warn(...)  log_output(LOG_PRIORITY_WARN,  __VA_ARGS__)
#define log_error(...) log_output(LOG_PRIORITY_ERROR, __VA_ARGS__)
#define log_fatal(...) log_output(LOG_PRIORITY_FATAL, __VA_ARGS__)

extern void log_debug_on(void);
extern void log_debug_off(void);
extern void log_set_priority(enum log_priority priority);
extern void log_output(enum log_priority priority, char *fmt, ...);

#endif
