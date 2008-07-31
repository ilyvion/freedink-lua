/**
 * DinkC interactive console

 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2008  Sylvain Beucler

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

#ifndef _DINKC_CONSOLE_H
#define _DINKC_CONSOLE_H

#include "SDL.h"

extern int console_active;
extern char* dinkc_console_get_cur_line();
extern int dinkc_console_get_last_retval();
extern void dinkc_console_process_key(SDL_KeyboardEvent kev);

#endif
