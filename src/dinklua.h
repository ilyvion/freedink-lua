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

extern void dinklua_init();
extern void lua_dink_quit();
extern int lua_load_script(struct refinfo* rinfo, char* script);
extern int lua_run_script(struct refinfo* rinfo);
extern void lua_kill_script(struct refinfo* rinfo);

#endif
