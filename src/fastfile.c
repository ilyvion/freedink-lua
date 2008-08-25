/**
 * Free fastfile.cpp replacement

 * Copyright (C) 2003  Shawn Betts
 * Copyright (C) 2003, 2004, 2008  Sylvain Beucler

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
/* #define strcasecmp(a,b) stricmp(a,b) */
#else
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#include "str_util.h"

struct FF_Entry
{
  long off;
  char name[13];
};

struct FF_Handle
{
  int alive;
  long pos, off, len;
};

void FastFileFini (void);

static struct FF_Entry *g_Entries = 0;
static struct FF_Handle *g_Handles = 0;

size_t g_FileSize = 0, g_numEntries = 0, g_numHandles = 0;

#ifndef _WIN32
static int g_File = 0;
#else
HANDLE g_File;
HANDLE g_FileMap;
#endif


unsigned char *g_MemMap = 0;

int
FastFileInit(char *filename, int max_handles)
{
  long count = 0;
  unsigned char *buf = NULL;

  FastFileFini ();

#ifndef _WIN32
  /* Open and mmap the file(Unix) */
  g_File = open (filename, O_RDONLY);
  g_FileSize = lseek (g_File, 0, SEEK_END);
  lseek (g_File, 0, SEEK_SET);

  g_MemMap = mmap (NULL, g_FileSize, PROT_READ, MAP_PRIVATE, g_File, 0);
#else
  /* Open and mmap the file(Windows) */
  g_File =
    CreateFile (filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS, 0);

  if (g_File == NULL)
    {
      return FALSE;
    }

  g_FileMap = CreateFileMapping (g_File, NULL, PAGE_READONLY, 0, 0, NULL);
  g_MemMap = MapViewOfFile (g_FileMap, FILE_MAP_READ, 0, 0, 0);
#endif

  /* Get the number of entries (from stored LSB int32) */
  buf = g_MemMap;
  g_numEntries = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
  g_numHandles = max_handles;
  buf += 4;

  /* Allocate the memory */
  g_Entries = calloc(sizeof(struct FF_Entry), g_numEntries);
  g_Handles = calloc(sizeof(struct FF_Handle), max_handles);

  for (count = 0; count < g_numEntries; count++)
    {
      g_Entries[count].off = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
      buf += 4;
      strncpy (g_Entries[count].name, (char*)buf, 13);
      buf += 13;
    }

  return 1;
}


void
FastFileFini (void)
{
  if (g_Entries)
    {
      free (g_Entries);
      g_Entries = 0;
    }
  if (g_Handles)
    {
      free (g_Handles);
      g_Handles = 0;
    }

  if (!g_MemMap)
    return;
#ifndef _WIN32
  /* Unmap and close the file(Unix) */
  munmap (g_MemMap, g_FileSize);
  close (g_File);
  g_File = -1;
#else
  /* Unmap and close the file(Windows) */
  CloseHandle (g_FileMap);
  CloseHandle (g_File);
#endif

  g_MemMap = 0;
}




void *
FastFileOpen (char *name)
{
  struct FF_Handle *i;
  long fCount;
  long hCount;

  /* Check for the file, dont' include directory */
  for (fCount = 0; fCount < (long) g_numEntries - 1; fCount++)
    {
      if (string_icompare (g_Entries[fCount].name, name) == 0)
	{
	  for (hCount = 0; hCount < (long) g_numHandles; hCount++)
	    {
	      i = &g_Handles[hCount];

	      if (!i->alive)
		{
		  i->alive = 1;
		  i->off = g_Entries[fCount].off;
		  i->pos = 0;
		  if (g_Entries[fCount + 1].off != 0)
		    /* Normal offset, tells where next the image bytes
		       start */
		    i->len = g_Entries[fCount + 1].off - i->off;
		  else
		    /* Support badly generated dir.ff such as Mystery Island's */
		    i->len = g_Entries[fCount + 2].off - i->off;
		  /* TODO: watch for buffer overflows - check that
		     'off' is in a reasonable range [0, len(file)]
		     without overlapping existing pictures */
		  return (void *) i;
		}
	    }
	  return 0;
	}
    }
  return 0;
}


int
FastFileClose (struct FF_Handle *i)
{
  if (!i || !g_MemMap)
    return 0;

  i->alive = 0;
  return 1;
}





int
FastFileSeek (struct FF_Handle *i, int offset, int whence)
{
  long oldpos;

  if (!i || !g_MemMap)
    return 0;

  oldpos = i->pos;

  switch (whence)
    {
    case SEEK_SET:
      i->pos = offset;
      break;
    case SEEK_CUR:
      i->pos += offset;
      break;
    case SEEK_END:
      i->pos = i->len - offset;
      break;
    }

  if (i->pos > i->len)
    {
      i->pos = oldpos;
      return 0;
    }

  return 1;
}


int
FastFileRead (struct FF_Handle *i, void *bigBuffer, int size)
{
  unsigned char *srcBuffer;

  if (!i || !bigBuffer || !g_MemMap)
    return 0;
  if (i->pos + size > i->len)
    return 0;

  srcBuffer = g_MemMap;
  srcBuffer += i->pos;
  srcBuffer += i->off;

  memcpy (bigBuffer, srcBuffer, size);

  i->pos += size;

  return 1;

}


long
FastFileTell (struct FF_Handle *i)
{
  if (!i)
    return 0;
  return i->pos;
}



void *
FastFileLock (struct FF_Handle *i, int off, int len)
{
  char *buffer;


  if (!i || !g_MemMap)
    {
      return NULL;
    }
  if (off < 0 || len < 0)
    {
      return NULL;
    }
  if (len > i->len)
    {
      printf("FastFileLock: len = %d > i->len = %ld - exiting.\n", len, i->len);
      fflush(stdout);
      return NULL;
    }

  buffer = (char*)g_MemMap;
  buffer += i->off;
  buffer += off;

  return (void*)buffer;
}


int
FastFileUnlock (struct FF_Handle *i, int off, int len)
{
  return 1;
}

int
FastFileLen (struct FF_Handle *i)
{
  return i->len;
}
