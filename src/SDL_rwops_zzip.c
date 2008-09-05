/*
 *      Copyright (c) 2001 Guido Draheim <guidod@gmx.de>
 *      Copyright (C) 2007  Sylvain Beucler
 *      Use freely under the restrictions of the ZLIB License
 *
 *      (this example uses errno which might not be multithreaded everywhere)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SDL_rwops_zzip.h"
#include <SDL_rwops.h>

/* Order headers appropriately:
   http://lists.gnu.org/archive/html/bug-gnulib/2007-12/msg00152.html */
#include <string.h> /* strchr */
#include <zzip/zzip.h>

/* MSVC can not take a casted variable as an lvalue ! */
#define SDL_RWOPS_ZZIP_DATA(_context) \
             ((_context)->hidden.unknown.data1)
#define SDL_RWOPS_ZZIP_FILE(_context)  (ZZIP_FILE*) \
             ((_context)->hidden.unknown.data1)

static int rwops_zzip_seek(SDL_RWops *context, int offset, int whence)
{
    return zzip_seek(SDL_RWOPS_ZZIP_FILE(context), offset, whence);
}

static int rwops_zzip_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
    return zzip_read(SDL_RWOPS_ZZIP_FILE(context), ptr, size*maxnum) / size;
}

static int rwops_zzip_write(SDL_RWops *context, const void *ptr, int size, int num)
{
    return 0; /* ignored */
}

static int rwops_zzip_close(SDL_RWops *context)
{
    if (! context) return 0; /* may be SDL_RWclose is called by atexit */

    zzip_close (SDL_RWOPS_ZZIP_FILE(context));
    SDL_FreeRW (context);
    return 0;
}

SDL_RWops *SDL_RWFromZZIP(const char* file, const char* mode)
{
    register SDL_RWops* rwops;
    register ZZIP_FILE* zzip_file;

    if (! strchr (mode, 'r'))
	return SDL_RWFromFile(file, mode);

/*     zzip_file = zzip_fopen (file, mode); */
    zzip_strings_t xor_fileext[] = { "", 0 };
    zzip_file = zzip_open_ext_io(file,
      0, ZZIP_CASELESS|ZZIP_ONLYZIP,
      xor_fileext, 0);
    if (! zzip_file) return 0;

    rwops = SDL_AllocRW ();
    if (! rwops) { errno=ENOMEM; zzip_close (zzip_file); return 0; }

    SDL_RWOPS_ZZIP_DATA(rwops) = zzip_file;
    rwops->read = rwops_zzip_read;
    rwops->write = rwops_zzip_write;
    rwops->seek = rwops_zzip_seek;
    rwops->close = rwops_zzip_close;
    return rwops;
}
