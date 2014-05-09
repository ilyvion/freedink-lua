/**
 * libzip data source for SDL_RWops

 * Copyright (C) 2001  Guido Draheim <guidod@gmx.de>
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

/*
 *      (this example uses errno which might not be multithreaded everywhere)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SDL_rwops_libzip.h"
#include <zip.h>
#include <string.h> /* strchr */
#include <errno.h>

#include "log.h"

#if 0
static int rwops_libzip_seek(SDL_RWops *context, int offset, int whence)
{
  printf("rwops_libzip_seek(context=%p, offset=%d, whence=", context, offset);
  switch(whence)
    {
    case SEEK_SET:
      printf("SEEK_SET");
      break;
    case SEEK_CUR:
      printf("SEEK_CUR");
      break;
    case SEEK_END:
      printf("SEEK_END");
      break;
    }
  printf(");\n");
  return 0; /* ignored */
}

static int rwops_libzip_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
  printf("rwops_libzip_read(context=%p, ptr=%p, size=%d, maxnum=%d);\n", context, ptr, size, maxnum);
  return zip_fread(context->hidden.unknown.data1, ptr, size*maxnum) / size;
}

static int rwops_libzip_write(SDL_RWops *context, const void *ptr, int size, int num)
{
  return -1; /* not supported */
}

static int rwops_libzip_close(SDL_RWops *context)
{
  printf("rwops_libzip_close(context=%p);\n", context);

  if (context == NULL)
    return 0; /* may be SDL_RWclose is called by atexit */
  
  zip_fclose(context->hidden.unknown.data1);
  /* SDL_FreeRW (context); */
  /* TODO: zip_close(zarchive); */
  return 0;
}
#endif


SDL_RWops *SDL_RWFromZIP(const char* archivename, const char* filename)
{
    SDL_RWops* rwops;
    struct zip* zarchive;
    struct zip_file* zfile;
    int errorp = 0;

    zarchive = zip_open(archivename, ZIP_CHECKCONS, &errorp);
    if (errorp != 0)
      {
	char *errorbuf = NULL;
	int len = 1;
	errorbuf = malloc(len);
	len = zip_error_to_str(errorbuf, len, errorp, errno);
	errorbuf = realloc(errorbuf, len + 1);
	len = zip_error_to_str(errorbuf, len, errorp, errno);
	log_warn("zip_open: %s", errorbuf);
	free(errorbuf);
	return NULL;
      }
    zfile = zip_fopen(zarchive, filename, 0);
    if (zfile == NULL)
      {
	log_error("zip_open: %s", zip_strerror(zarchive));
	zip_close(zarchive);
	return NULL;
      }

    /* libzip does not support seek()ing in file, so we can't use it -
       TTF_OpenFontRW needs it to read data at the end of files.  */
#if 0
    rwops = SDL_AllocRW();
    if (rwops == NULL)
      { errno=ENOMEM;
	zip_close(zarchive);
	return NULL;
      }

    rwops->hidden.unknown.data1 = zfile;
    rwops->read = rwops_libzip_read;
    rwops->write = rwops_libzip_write;
    rwops->seek = rwops_libzip_seek;
    rwops->close = rwops_libzip_close;
    return rwops;
#endif

    struct zip_stat zfilestat;
    if (zip_stat(zarchive, filename, 0, &zfilestat) < 0)
      {
	log_warn("zip_open: %s", zip_strerror(zarchive));
	zip_close(zarchive);
	return NULL;
      }

    char *content = malloc(zfilestat.size);
    zip_fread(zfile, content, zfilestat.size);
    rwops = SDL_RWFromMem(content, zfilestat.size);
    zip_fclose(zfile);
    zip_close(zarchive);

    return rwops;
}
