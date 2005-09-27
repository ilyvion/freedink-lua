/**
 * Free fastfile.cpp replacement prototypes

 * Copyright (C) 2003  Shawn Betts

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with program; see the file COPYING. If not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifdef __cplusplus
extern "C"
{
#endif

  typedef void *HFASTFILE;

  extern int FastFileInit (char *filename, int max_handles);
  extern void FastFileFini (void);
  extern void *FastFileOpen (char *name);
  extern int FastFileClose (void *pfe);
  extern int FastFileRead (void *pfh, void *ptr, int size);
  extern int FastFileSeek (void *pfe, int off, int how);
  extern long FastFileTell (void *pfe);
  extern void *FastFileLock (void *pfe, int off, int len);
  extern int FastFileUnlock (void *pfe, int off, int len);
  extern int FastFileLen (void *i);

#ifdef __cplusplus
}
#endif
