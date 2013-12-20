/**
 * Hash table to be used by DinkC's 'sp_custom' function

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

#ifndef SP_CUSTOM_HASH
#define SP_CUSTOM_HASH

#include "hash.h"

typedef Hash_table* sp_custom;

extern sp_custom sp_custom_new();
extern void sp_custom_free(sp_custom hash);
extern void sp_custom_set(sp_custom hash, char key[200], int val);
extern int sp_custom_get(sp_custom hash, char key[200]);
extern void sp_custom_clear(sp_custom hash);

#endif
