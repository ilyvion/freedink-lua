/**
 * Graphics

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

#include <string.h>
#include "SDL.h"
#include "io_util.h"
#include "gfx.h"

// // DELETEME
// LPDIRECTDRAW            lpDD = NULL;           // DirectDraw object
// //LPDIRECTDRAWSURFACE     lpDDSOne;       // Offscreen surface 1

// LPDIRECTDRAWSURFACE     lpDDSPrimary = NULL;   // DirectDraw primary surface
// LPDIRECTDRAWSURFACE     lpDDSBack = NULL;      // DirectDraw back surface

// LPDIRECTDRAWSURFACE     lpDDSTwo = NULL;       // Offscreen surface 2
// LPDIRECTDRAWSURFACE     lpDDSTrick = NULL;       // Offscreen surface 2
// LPDIRECTDRAWSURFACE     lpDDSTrick2 = NULL;       // Offscreen surface 2


SDL_Surface *GFX_lpDDSBack = NULL; /* Backbuffer and link to physical
				      screen*/

/* GFX_lpDDSTwo: holds the base scene */
/* Rationale attempt :*/
/* lpDDSTwo contains the background, which is reused for each new
   frame. It is overwritten when switching to another screen. However,
   it can change during a screen: 1) animated tiles (water & fire) 2)
   when a sprite is written on the background (eg when an enemy dies)
   3) with various hacks such as fill_screen() (and maybe
   copy_bmp_to_screen()). */
/* Those changes may conflict with each other (eg: an animated tile
   overwrites half the carcass of a dead enemy - which I think is used
   in one of Mike Snyder's DMods to make a slayer carcass disappear
   when killed on a weird alien floor; or maybe that was a real
   bug..). I think that some alternate versions of Dink considered
   those conflicts as bugs (not features) and fixed them. Maybe we
   should do so as well. */
/* After the background is done, all the other operations are applied
   on lpDDSBack, the double buffer which is directly used by the
   physical screen. (Note: in DX, that's the case; in SDL, the double
   buffer is included in lpDDSPrimary, so we need to get rid of the
   useless lpDDSBack intermediary - but later. */
SDL_Surface *GFX_lpDDSTwo = NULL;

/* Beuc: apparently used for the scrolling screen transition and more
   generaly as temporary buffers. Only used by the game, not the
   editor. */
/* Used in freedink.cpp only + as a local/independent temporary buffer
   in show_bmp&copy_bmp&process_show_bmp */
SDL_Surface *GFX_lpDDSTrick = NULL;
/* Used in freedink.cpp and update_frame.cpp */
SDL_Surface *GFX_lpDDSTrick2 = NULL;


/* Reference palette: this is the canonical Dink palette, loaded from
   TS01.bmp (for freedink) and esplash.bmp (for freedinkedit). The
   physical screen may be changed (e.g. show_bmp()), but this
   canonical palette will stay constant. */
/* PALETTEENTRY  real_pal[256]; */
SDL_Color GFX_real_pal[256];

/** Game-specific **/
/* Palette change: with SDL, SDL_SetColors (aka
   SDL_SetPalette(SDL_PHYSPAL)) apparently triggers a Flip, which
   displays weird colors on the screen for a brief but displeasing
   moment. Besides, SDL_Flip() does not refresh the hardware palette,
   so update the physical palette needs to be done manually - but only
   when the surface is already in its final form. The palette may need
   to be changed before the screen content is ready, so we'll make the
   engine know when he needs to refresh the physical palette: */
/* Tell flip_it* to install the new palette */
int trigger_palette_change = 0;
SDL_Color cur_screen_palette[256];

/* Schedule a change to the physical screen's palette for the next
   frame */
void change_screen_palette(SDL_Color* new_palette) {
  {
    /* Now this one is tricky: DX/Woe has a "feature" where palette
       indexes 0 and 255 are fixed to black and white,
       respectively. This is the opposite of the default Dink palette
       - which is why fill_screen(0) is black and not white as in the
       Dink palette. This also makes "Lyna's Story"'s palette change a
       bit ugly, because pure black and white colors are not reversed
       when you enter "negative" color mode. This does not affect
       other indexes. Technically this happens when you get a palette
       from GetEntries(), and when you CreatePalette() without
       specifying DDPCAPS_ALLOW256 (so respectively, in
       change_screen_palette() and load_palette_from_*()). But well,
       reproducing the bug is important for backward compatibility. */

    memcpy(cur_screen_palette, new_palette, sizeof(cur_screen_palette));

    cur_screen_palette[0].r = 0;
    cur_screen_palette[0].g = 0;
    cur_screen_palette[0].b = 0;
    cur_screen_palette[255].r = 255;
    cur_screen_palette[255].g = 255;
    cur_screen_palette[255].b = 255;

    /* Applying the logical palette to the physical screen may trigger
       a Flip, so don't do it right now */
    trigger_palette_change = 1;
  }
}


/* LoadBMP wrapper. Load a new graphic from file, and apply the
   reference palette so that all subsequent blits are faster (color
   convertion is avoided) - although the initial loading time will be
   somewhat longer. */
static SDL_Surface* load_bmp_internal(char *filename, SDL_RWops *rw, int from_mem, int set_pal) {
  SDL_Surface *image, *copy;
  char tmp_filename[PATH_MAX];

  if (from_mem == 1)
    image = SDL_LoadBMP_RW(rw, 0);
  else
    image = SDL_LoadBMP(ciconvertbuf(filename, tmp_filename));

  if (image == NULL)
    {
      fprintf(stderr, "load_bmp_internal: %s\n", SDL_GetError());
      return NULL;
    }

  /* Copy the surface */
  /* TODO: how about using SDL_DisplayFormat()? */
  copy = SDL_ConvertSurface(image, image->format, image->flags);

  if (set_pal == 1)
    {
      SDL_Color palette[256];
      load_palette_from_surface(image, palette);
      change_screen_palette(palette);
      /* Pretend that the image uses the current screen and buffers
	 palette, to avoid color conversion to the reference palette
	 (maintain palette indexes). We maintain palette indexes so
	 that they will match the physical screen's palette, which we
	 just change. */
      /* Note: cur_screen_palette is not exactly the same as palette,
	 because DX reserves some indexes, and FreeDink reimplement
	 this limitation for compatibility. So we still need a blit
	 with color convertion to take reserved indexes into
	 account. Typically skipping this step will reverse black and
	 white (with Dink palette indexes: 255 and 0; with DX reserved
	 indexes: 0 and 255). */
      SDL_SetPalette(image, SDL_LOGPAL, cur_screen_palette, 0, 256);
    }
  else
    {
      /* Prepare a color conversion to the reference palette */
      SDL_SetPalette(image, SDL_LOGPAL, GFX_real_pal, 0, 256);
    }

  /* Blit the copy back to the original, with a potentially different
     palette, which triggers color conversion to image's palette. */
  SDL_BlitSurface(copy, NULL, image, NULL);
  SDL_FreeSurface(copy);
  
  /* In the end, the image must use the reference palette: that way no
     mistaken color conversion will occur during blits to other
     surfaces/buffers. Blits should also be faster(?). */
  SDL_SetPalette(image, SDL_LOGPAL, GFX_real_pal, 0, 256);

  return image;
}

/* LoadBMP wrapper, from file */
SDL_Surface* load_bmp(char *filename)
{
  return load_bmp_internal(filename, NULL, 0, 0);
}

/* LoadBMP wrapper, from memory */
SDL_Surface* load_bmp_from_mem(SDL_RWops *rw)
{
  return load_bmp_internal(NULL, rw, 1, 0);
}

/* LoadBMP wrapper + use as current palette */
SDL_Surface* load_bmp_setpal(char *filename)
{
  return load_bmp_internal(filename, NULL, 0, 1);
}
