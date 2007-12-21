/**
 * Compute and store the search paths

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

#ifndef _PATH_H
#define _PATH_H

#ifdef __cplusplus
extern "C"
{
#endif

  extern void paths_init(char *refdir_opt, char *dmoddir_opt);
  extern const char *paths_dmoddir(void);
  extern const char *paths_pkgdatadir(void);

#ifdef __cplusplus
}
#endif

#endif
