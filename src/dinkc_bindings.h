/**
 * Link game engine and DinkC script engine

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

#ifndef _DINKC_BINDINGS_H
#define _DINKC_BINDINGS_H

enum dinkc_parser_state {
  DCPS_GOTO_NEXTLINE = 0,
  DCPS_CONTINUE = 1,
  DCPS_YIELD = 2,
  DCPS_DOELSE_ONCE = 4,
};

extern void dinkc_bindings_init();
extern void dinkc_bindings_quit();
extern void attach(void);
extern /*bool*/int talk_get(int script);
extern enum dinkc_parser_state process_line (int script, char *s, /*bool*/int doelse);

#endif
