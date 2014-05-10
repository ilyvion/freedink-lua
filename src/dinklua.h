/**
 * Header for Lua integration

 * Copyright (C) 2013  Alexander Krivács Schrøder

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

#ifndef _DINKLUA_H
#define _DINKLUA_H

#include <lua.h>

#include "scripting.h"

struct luainfo
{
  int script_loaded;
};

extern int dinklua_enabled;
extern struct script_engine *lua_engine;

extern void dinklua_initialize(struct script_engine *script_engine);
extern void dinklua_quit();
extern int dinklua_get_current_line(lua_State *l);

#endif
