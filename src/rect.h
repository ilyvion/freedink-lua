/**
 * Rectangle structure to make hardness maths

 * Copyright (C) 2007  Sylvain Beucler

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

#ifndef _RECT_H
#define _RECT_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct rect {
  int left, top, right, bottom;
} rect;

extern int rect_set(rect *rect, int left, int top, int right, int bottom);
extern int rect_offset(rect *rect, int tx, int ty);
extern int rect_copy(rect *src, rect *dst);
extern int rect_inflate(rect *rect, int dw, int dh);

#ifdef __cplusplus
}
#endif

#endif
