/*
 *      Copyright (c) 2001 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB License
 *
 *      You should be able to drop it in the place of a SDL_RWFromFile. Then
 *      go to X/share/myapp and do `cd graphics && zip -9r ../graphics.zip .`
 *      and rename the graphics/ subfolder - and still all your files
 *      are found: a filepath like X/shared/graphics/game/greetings.bmp 
 *      will open X/shared/graphics.zip and return the zipped file 
 *      game/greetings.bmp in the zip-archive (for reading that is).
 *
 */

#ifndef _SDL_RWops_LIBZIP_h
#define _SDL_RWops_LIBZIP_h

#include <SDL_rwops.h>

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_RWops *SDL_RWFromZIP(const char* archivename, const char* filename);

#ifdef __cplusplus
} /* extern C */
#endif

#endif
