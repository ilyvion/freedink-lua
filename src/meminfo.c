/**
 * Display malloc stats

 * Copyright (C) 2009  Sylvain Beucler

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

#ifdef HAVE_MALLINFO
#include <malloc.h>
#include "log.h"
#endif

void meminfo_log_mallinfo()
{
#ifdef HAVE_MALLINFO
  struct mallinfo mi;
  mi = mallinfo();
  log_debug("arena: %d", mi.arena);
  log_debug("uordblks: %d", mi.uordblks);
  log_debug("fordblks: %d", mi.fordblks);
#endif
}
