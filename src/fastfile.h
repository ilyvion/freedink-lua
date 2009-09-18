/**
 * Free fastfile.cpp replacement prototypes

 * Copyright (C) 2003  Shawn Bett
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

#ifndef _FASTFILE_H
#define _FASTFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

  typedef void *HFASTFILE;

  extern int FastFileInit(char *filename, int max_handles);
  extern void FastFileFini(void);
  extern HFASTFILE FastFileOpen(char *name);
  extern int FastFileClose(void *pfe);
  extern SDL_RWops* FastFileLock(void *pfe);
  extern int FastFileLen(void *i);
#if 0
  extern int FastFileUnlock(void *pfe, int off, int len);
  extern int FastFileRead (void *pfh, void *ptr, int size);
  extern int FastFileSeek (void *pfe, int off, int how);
  extern long FastFileTell (void *pfe);
#endif

#ifdef __cplusplus
}
#endif

#endif
