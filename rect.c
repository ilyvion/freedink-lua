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

#include "rect.h"

/* For NULL: */
#include "stdlib.h"

int
SetRect(RECT *rect, int left, int top, int right, int bottom)
{
  if (rect == NULL)
    return 0;
  rect->left   = left;
  rect->top    = top;
  rect->right  = right;
  rect->bottom = bottom;
  return 1;
}

int
OffsetRect(RECT *rect, int tx, int ty)
{
  if (rect == NULL)
    return 0;
  rect->left   += tx;
  rect->top    += ty;
  rect->right  += tx;
  rect->bottom += ty;
  return 1;
}

int
CopyRect(RECT *src, RECT *dst)
{
  if (src == NULL || dst == NULL)
    return 0;
  dst->left   = src->left;
  dst->top    = src->top;
  dst->right  = src->right;
  dst->bottom = src->bottom;
  return 1;
}

int
InflateRect(RECT *rect, int dw, int dh)
{
  if (rect == NULL)
    return 0;
  rect->right  += dw;
  rect->bottom += dh;
  return 1;
}
