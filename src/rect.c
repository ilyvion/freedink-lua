/**
 * Rectangle structure to make hardness maths

 * Copyright (C) 2007, 2009  Sylvain Beucler

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

#include "rect.h"

/* For NULL: */
#include "stdlib.h"

int
rect_set(rect *rect, int left, int top, int right, int bottom)
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
rect_offset(rect *rect, int tx, int ty)
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
rect_copy(rect *dst, rect *src)
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
rect_inflate(rect *rect, int dw, int dh)
{
  if (rect == NULL)
    return 0;
  rect->left   -= dw;
  rect->top    -= dh;
  rect->right  += dw;
  rect->bottom += dh;
  return 1;
}
