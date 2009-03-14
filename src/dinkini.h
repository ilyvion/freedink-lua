/**
 * Dink.ini - loading graphics

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
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

#ifndef _DINKINI_H
#define _DINKINI_H

#include "rect.h"

enum idata_type { IDATA_EMPTY=0, IDATA_SPRITE_INFO, IDATA_FRAME_SPECIAL,
		  IDATA_FRAME_DELAY, IDATA_FRAME_FRAME };

/* Graphics load flags */
#define DINKINI_BLACK        0x00000001
#define DINKINI_LEFTALIGN    0x00000002
#define DINKINI_NOTANIM      0x00000004
#define DINKINI_COMPAT_DIRFF 0x00000008

extern void dinkini_init(void);
extern void dinkini_quit(void);
extern void make_idata(enum idata_type type, int myseq, int myframe, int xoffset, int yoffset, rect crect);
extern void program_idata(void);
extern void load_batch(void);

#endif
