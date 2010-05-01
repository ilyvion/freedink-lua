/**
 * Free fastfile.cpp replacement

 * Copyright (C) 2003  Shawn Betts
 * Copyright (C) 2003, 2004, 2007, 2009  Sylvain Beucler

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#  include <windows.h>
#  include <io.h>
/* #define strcasecmp(a,b) stricmp(a,b) */
#else
#  include <unistd.h>
#  ifdef HAVE_MMAP
#    include <sys/mman.h>
#  endif
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#endif
#include "str_util.h"
#include "io_util.h"
extern void FastFileFini(void);

#define HEADER_NB_ENTRIES_LEN 4

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

static struct FF_Entry *g_Entries = NULL;
static struct FF_Handle *g_Handles = NULL;

static unsigned int g_FileSize = 0;
static unsigned int g_numEntries = 0;
static unsigned int g_numHandles = 0;

#ifdef HAVE_MMAP
static int g_File = 0;
#else
#  ifdef _WIN32
HANDLE g_File;  /* HANDLE == void*, cf. winnt.h */
HANDLE g_FileMap;
#  else
FILE* g_File = NULL;
#  endif
#endif

#if defined HAVE_MMAP || defined _WIN32
unsigned char *g_MemMap = NULL;
#endif

int
FastFileInit(char *filename, int max_handles)
{
  long count = 0;
  FastFileFini();

#if _WIN32 | HAVE_MMAP
  unsigned char *buf = NULL;
#endif

#ifdef HAVE_MMAP
  /* Open and mmap the file (Unix) */
  g_File = open(filename, O_RDONLY);
  if (g_File < 0)
    return 0/*false*/;
  g_FileSize = lseek(g_File, 0, SEEK_END); /* needed by munmap */
  lseek (g_File, 0, SEEK_SET);

  g_MemMap = mmap(NULL, g_FileSize, PROT_READ, MAP_PRIVATE, g_File, 0);
#else
#  ifdef _WIN32
  /* Open and mmap the file (Windows) */

  /* first: file size (for later sanity checks) */
  FILE* t = fopen(filename, "rb");
  if (t == NULL)
    return FALSE;
  fseek(t, 0, SEEK_END);
  g_FileSize = ftell(t);
  fseek(t, 0, SEEK_SET);
  fclose(t);

  g_File =
    CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	       FILE_FLAG_RANDOM_ACCESS, 0);
  if (g_File == NULL)
    return FALSE;

  g_FileMap = CreateFileMapping (g_File, NULL, PAGE_READONLY, 0, 0, NULL);
  g_MemMap = MapViewOfFile (g_FileMap, FILE_MAP_READ, 0, 0, 0);
#  else
  /* C stdio (portable) */
  g_File = fopen(filename, "rb");
  if (g_File == NULL)
    return 0/*false*/;
  
  fseek(g_File, 0, SEEK_END);
  g_FileSize = ftell(g_File);
  fseek(g_File, 0, SEEK_SET);
#  endif
#endif

  g_numHandles = max_handles;
  /* Get the number of entries (from stored LSB int32) */
#if defined HAVE_MMAP || defined _WIN32
  buf = g_MemMap;
  g_numEntries =
    (buf[3] << 24) | (buf[2] << 16)
    | (buf[1] << 8) | (buf[0]);
  buf += 4;
#else
  g_numEntries = read_lsb_int(g_File);
#endif

  /* Allocate the memory */
  g_Entries = calloc(sizeof(struct FF_Entry), g_numEntries);
  g_Handles = calloc(sizeof(struct FF_Handle), max_handles);

  for (count = 0; count < g_numEntries; count++)
    {
#if defined HAVE_MMAP || defined _WIN32
      g_Entries[count].off = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
      buf += 4;
      strncpy (g_Entries[count].name, (char*)buf, 13);
      buf += 13;
#else
      g_Entries[count].off = read_lsb_int(g_File);
      fread(g_Entries[count].name, 13, 1, g_File);
#endif
      /* Ensure string is null-terminated */
      g_Entries[count].name[12] = '\0';
    }

  return 1;
}


void
FastFileFini (void)
{
  if (g_Entries != NULL)
    {
      free(g_Entries);
      g_Entries = NULL;
    }
  if (g_Handles != NULL)
    {
      free(g_Handles);
      g_Handles = NULL;
    }

#ifdef HAVE_MMAP
  /* Unmap and close the file (Unix) */
  if (g_MemMap != NULL)
    {
      munmap(g_MemMap, g_FileSize);
      g_MemMap = NULL;
    }
  if (g_File != 0)
    {
      close(g_File);
      g_File = 0;
    }
#else
#  ifdef _WIN32
  if (g_MemMap != NULL)
    {
      /* Unmap and close the file (Windows) */
      CloseHandle(g_FileMap);
      CloseHandle(g_File);
      g_MemMap = NULL;
    }
#  else
  if (g_File != NULL)
    {
      fclose(g_File);
      g_File = NULL;
    }
#  endif
#endif
}


void *
FastFileOpen(char *name)
{
  struct FF_Handle *i;
  long fCount;
  long hCount;

  /* Check for the file, dont' include directory */
  for (fCount = 0; fCount < (long)g_numEntries - 1; fCount++)
    {
      if (strcasecmp(g_Entries[fCount].name, name) == 0)
	{
	  for (hCount = 0; hCount < (long)g_numHandles; hCount++)
	    {
	      i = &g_Handles[hCount];

	      if (!i->alive)
		{
		  i->alive = 1;
		  i->off = g_Entries[fCount].off;
		  i->pos = 0;
		  /* Normal offset, tells where next the image bytes
		     start */
		  int next_off = g_Entries[fCount + 1].off;
		  if (next_off == 0)
		    /* Support badly generated dir.ff such as Mystery
		       Island's (skip 1 empty entry) */
		    /* TODO: check that fCount+2 is valid. */
		    next_off = g_Entries[fCount + 2].off;
		  /* Watch for buffer overflows - check that 'off' is
		     in a reasonable range [0, len(file)], and doesn't
		     overlap another fastfile */
		  if ((i->off < 0 || i->off > g_FileSize)
		      || i->off > next_off)
		    i->len = 0;
		  else
		    i->len = next_off - i->off;
		  return (void*)i;
		}
	    }
	  return NULL;
	}
    }
  return NULL;
}


int
FastFileClose (struct FF_Handle *i)
{
  if (!i)
    return 0;

#if defined HAVE_MMAP || defined _WIN32
  if (!g_MemMap)
    return 0;
#endif

  i->alive = 0;
  return 1;
}


SDL_RWops*
FastFileLock(struct FF_Handle *i)
{
  if (!i)
    return NULL;

#if defined HAVE_MMAP || defined _WIN32
  char *buffer = NULL;
  if(!g_MemMap)
    return NULL;
#endif

#if defined HAVE_MMAP || defined _WIN32
  buffer = (char*)g_MemMap;
  buffer += i->off;
  return SDL_RWFromMem(buffer, i->len);
#else
  fseek(g_File, i->off, SEEK_SET);
  return SDL_RWFromFP(g_File, /*autoclose=*/0);
#endif
}


int
FastFileLen(struct FF_Handle *i)
{
  return i->len;
}



/* comment out unused functions to ease portability */
#if 0
int
FastFileUnlock(struct FF_Handle *i, int off, int len)
{
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
#endif
