/**
 * Graphics - sprites management

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2007, 2008, 2009  Sylvain Beucler

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

#include "gfx_sprites.h"

#include <stdlib.h>
#include <string.h>
#include "SDL_image.h"

#include "gfx.h"
#include "dinkvar.h"

#include "fastfile.h"
#include "io_util.h"
#include "log.h"
#include "paths.h"
#include "dinkini.h"

/*
  External global variables in use:
  seq[], GFX_k[], k[], no_running_main
*/

/* TODO: get rid of either k or GFX_k */
struct pic_info     k[MAX_SPRITES];       // Sprite data
struct GFX_pic_info GFX_k[MAX_SPRITES];   // Sprite data (SDL)

struct sequence seq[MAX_SEQUENCES];


static int please_wait = 0;

/**
 * Free memory used by sprites. It's not much useful in itself, since
 * it's only called when we're exiting the game, but it does avoid
 * memory leak warnings when FreeDink is analyzed by Valgrind or other
 * memory checkers. It also supports loading and unloading the
 * subsystem several times.
 */
void sprites_unload(void)
{
  int i = 0;
  for (i = 0; i < MAX_SPRITES; i++)
    {
      if (GFX_k[i].k != NULL)
	SDL_FreeSurface(GFX_k[i].k);
      GFX_k[i].k = NULL;
    }
  for (i = 0; i < MAX_SEQUENCES; i++)
    {
      if (seq[i].ini != NULL)
	free(seq[i].ini);
      seq[i].ini = NULL;
    }
}


/**
 * Display a flashing "Please Wait" anim directly on the screen, just
 * before switching to a screen that requires loading new graphics
 * from the disk.
 */
static void draw_wait()
{
  if (seq[423].frame[8] != 0)
    {

      if (please_wait)
	{
	  SDL_Rect dst = {232, 0, GFX_k[seq[423].frame[7]].k->w, GFX_k[seq[423].frame[7]].k->h};
	  SDL_BlitSurface(GFX_k[seq[423].frame[7]].k, NULL, GFX_lpDDSBack, &dst);
	  SDL_UpdateRects(GFX_lpDDSBack, 1, &dst);
	  please_wait = 0;
	}
      else
	{
	  SDL_Rect dst = {232, 0, GFX_k[seq[423].frame[8]].k->w, GFX_k[seq[423].frame[8]].k->h};
	  SDL_BlitSurface(GFX_k[seq[423].frame[8]].k, NULL, GFX_lpDDSBack, &dst);
	  SDL_UpdateRects(GFX_lpDDSBack, 1, &dst);
	  please_wait = 1;
	}
    }
}


/**
 * Return the next available graphic slot
 */
static int next_slot()
{
  /* Start index at 1 instead of 0. A few parts in the game rely on
     this (e.g. if getpic(...) < 1). */
  /* TODO: I noticed that sprite in slot 0 (by default, this would be
     a small white square) would be displayed temporarily on the
     screen in some situations.. */
  int i = 1;
  while (i < MAX_SPRITES && GFX_k[i].k != NULL)
    i++;
  return i;
  /* Callee will need to check if i >= MAX_SPRITES and fail if
     necessary */
}

/**
 * Free all graphic slots used by given sequence
 */
static void free_seq(int seq_no)
{
  int i = 1;
  int slot_index = -1;
  while (i < MAX_FRAMES_PER_ABUSED_SEQUENCE+1
	 && (slot_index = seq[seq_no].frame[i]) != 0)
    {
      SDL_FreeSurface(GFX_k[slot_index].k);
      GFX_k[slot_index].k = NULL;
      i++;
    }
  /* 0 means end-of-sequence, no more frames */
  if (i == MAX_FRAMES_PER_ABUSED_SEQUENCE+1)
    log_error("Invalid sequence %d, just avoided a buffer overflow\n", seq_no);
}


void load_sprite_pak(char seq_path_prefix[100], int seq_no, int delay, int xoffset, int yoffset,
		     rect hardbox, int flags, /*bool*/int samedir)
{
  char fname[20];
  char crap[200];

  int notanim = 0, black = 0, leftalign = 0;
  if ((flags & DINKINI_NOTANIM) == DINKINI_NOTANIM)
    notanim = 1;
  if ((flags & DINKINI_BLACK) == DINKINI_BLACK)
    black = 1;
  if ((flags & DINKINI_LEFTALIGN) == DINKINI_LEFTALIGN)
    leftalign = 1;

  /* If the sequence already exists, free it first */
  free_seq(seq_no);

  if (no_running_main)
    draw_wait();

  char *seq_dirname = pdirname(seq_path_prefix);
  int n = strlen(seq_path_prefix) - strlen(seq_dirname)-1;
  char *fullpath = NULL;
  strcpy(fname, &seq_path_prefix[strlen(seq_path_prefix) - n]);
  sprintf(crap, "%s/dir.ff", seq_dirname);
  if (samedir)
    fullpath = paths_dmodfile(crap);
  else
    fullpath = paths_fallbackfile(crap);

  if (!FastFileInit(fullpath, 5))
    {
      log_error("Could not load dir.ff art file %s", crap);
      free(fullpath);
      free(seq_dirname);
      return;
    }
  free(seq_dirname);
  /* keep 'fullpath' for error messages, free() it later */


  int oo;
  for (oo = 1; oo <= MAX_FRAMES_PER_SEQUENCE; oo++)
    {
      int myslot = next_slot();
      if (myslot >= MAX_SPRITES)
	{
	  log_error("No sprite slot available! Index %d out of %d.",
		    myslot, MAX_SPRITES);
	  break;
	}

      char *leading_zero = NULL;
      //load sprite
      if (oo < 10) leading_zero = "0"; else leading_zero = "";
      sprintf(crap, "%s%s%d.bmp", fname, leading_zero, oo);

      HFASTFILE pfile = FastFileOpen(crap);

      if (pfile == NULL)
	/* File not present in this fastfile - either missing file or
	   end of sequence */
	break;
      
      // GFX
      SDL_RWops *rw = FastFileLock(pfile);
      if (rw == NULL)
	{
	  /* rwops error? */
	  log_error("Failed to open '%s' in fastfile '%s'", crap, fullpath);
	}
      else
	{
	  /* We use IMG_Load_RW instead of SDL_LoadBMP because there
	     is no _RW access in plain SDL. However there is no
	     intent to support anything else than 8bit BMPs. */
	  GFX_k[myslot].k = IMG_Load_RW(rw, 1); // auto free()
	  if (GFX_k[myslot].k == NULL)
	    log_error("Failed to load %s from fastfile %s: %s", crap, fullpath, SDL_GetError());
	}
      if (GFX_k[myslot].k == NULL)
	{
	  log_error("Failed to load %s from fastfile %s (see error above)", crap, fullpath);
	  FastFileClose(pfile);
	  break;
	}
      if (GFX_k[myslot].k->format->BitsPerPixel != 8)
	{
	  log_error("Failed to load %s from fastfile %s:"
		    " only 8bit paletted bitmaps are supported in dir.ff archives.",
		    crap, fullpath);
	  SDL_FreeSurface(GFX_k[myslot].k);
	  continue;
	}
      
      // Palettes and transparency
      
      /* Note: in the original engine, for efficiency, no palette
	 conversion was done for sprite paks - they needed to use the
	 Dink Palette (otherwise weird colors would appear!)) */
      /* The engine suffered a DX limitation: palette indexes 0 and
	 255 fixed fixed to black and white respectively. This is also
	 the opposite of the Dink BMP palette indexes. This causes
	 troubles when skipping palette conversion (here), during
	 fade_down()/fade_up() (255/white pixels can't be darkened)
	 and in DinkC's fillcolor(index). This is why this function
	 replaced black with brighter black and white with darker
	 white. */
      /* In FreeDink palette conversion is done in load_bmp_internal,
	 so we mainly care about avoiding white pixels during
	 fade_down(), and only because we reproduced the palette
	 limitation so as to support dynamic palette-changing tricks
	 (cf. Lyna's Story) as well as having readable white text
	 during fade_down(). Maintaining compatibility with
	 fillcolor() is also important (although forcing 0 and 255
	 indexes could be done in that function only). But we might
	 consider getting rid of it entirely. We just have to make
	 sure a dir.ff LEFTALIGN has no transparency, otherwise the
	 experience counter digits in the status bar will become
	 transparent. */
      /* Not doing the conversion in truecolor mode, because at least
	 one D-Mod (The Blacksmith's Trail) ships with a 24bit, hence
	 palette-less, tiles/ts01.bmp, and this doesn't mess up
	 graphics in v1.08. */

      if (!truecolor)
	SDL_SetPalette(GFX_k[myslot].k, SDL_LOGPAL, GFX_real_pal, 0, 256);

      Uint8 *p = (Uint8 *)GFX_k[myslot].k->pixels;
      Uint8 *last = p + GFX_k[myslot].k->h * GFX_k[myslot].k->pitch;

      if (leftalign)
	{
	  // brighten black and darken white
	  while (p < last)
	    {
	      if (*p == 0)        // white
		*p = 30;          // darker white
	      else if (*p == 255) // black
		*p = 249;         // brighter black
	      p++;
	    }
	}
      else if (black)
	{
	  // darken white and set black as transparent
	  while (p < last)
	    {
	      if (*p == 0) // white
		*p = 30;   // darker white
	      p++;
	    }
	  SDL_SetColorKey(GFX_k[myslot].k, SDL_SRCCOLORKEY|SDL_RLEACCEL, 255);
	  /* Force RLE encoding now to save memory space */
	  SDL_BlitSurface(GFX_k[myslot].k, NULL, GFX_lpDDSTrick2, NULL);
	}
      else
	{
	  // brighten black and set white as transparent
	  while (p < last)
	    {
	      if (*p == 255) // black in Dink palette
		*p = 249;    // brighter black
	      p++;
	    }
	  SDL_SetColorKey(GFX_k[myslot].k, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0);
	  /* Force RLE encoding now to save memory space */
	  SDL_BlitSurface(GFX_k[myslot].k, NULL, GFX_lpDDSTrick2, NULL);
	}
      
      if (truecolor)
	{
	  /* We may want to convert to truecolor for possibly faster
	     blits. However I didn't notice a significant increase or
	     decrease in performances, but there's a higher memory
	     usage (+10MB for base Dink, more for D-Mods), so let's
	     not do it. */
	  /* 	  SDL_Surface *temp = SDL_DisplayFormat(GFX_k[myslot].k); */
	  /* 	  SDL_FreeSurface(GFX_k[myslot].k); */
	  /* 	  GFX_k[myslot].k = temp; */
	}


      k[myslot].box.top = 0;
      k[myslot].box.left = 0;
      k[myslot].box.right = GFX_k[myslot].k->w;
      k[myslot].box.bottom = GFX_k[myslot].k->h;
      
      /* Define the offsets / center of the image */
      if (yoffset > 0)
	{
	  // explicitely set center
	  k[myslot].yoffset = yoffset;
	}
      else
	{
	  if (oo > 1 && notanim)
	    // copy first frame info
	    k[myslot].yoffset = k[seq[seq_no].frame[1]].yoffset;
	  else
	    // compute default center
	    k[myslot].yoffset = (k[myslot].box.bottom - (k[myslot].box.bottom / 4))
	      - (k[myslot].box.bottom / 30);
	}
      
      if (xoffset > 0)
	{
	  // explicitely set center
	  k[myslot].xoffset = xoffset;
	}
      else
	{
	  if (oo > 1 && notanim)
	    // copy first frame info
	    k[myslot].xoffset = k[seq[seq_no].frame[1]].xoffset;
	  else
	    // compute default center
	    k[myslot].xoffset = (k[myslot].box.right - (k[myslot].box.right / 2))
	      + (k[myslot].box.right / 6);
	}
      //ok, setup main offsets, lets build the hard block
      
      if (hardbox.right > 0)
	{
	  //forced setting
	  k[myslot].hardbox.left = hardbox.left;
	  k[myslot].hardbox.right = hardbox.right;
	}
      else
	{
	  //guess setting
	  int work = k[myslot].box.right / 4;
	  k[myslot].hardbox.left -= work;
	  k[myslot].hardbox.right += work;
	}
      
      if (hardbox.bottom > 0)
	{
	  k[myslot].hardbox.top = hardbox.top;
	  k[myslot].hardbox.bottom = hardbox.bottom;
	}
      else
	{
	  int work = k[myslot].box.bottom / 10;
	  k[myslot].hardbox.top -= work;
	  k[myslot].hardbox.bottom += work;
	}
      FastFileClose(pfile);

      seq[seq_no].frame[oo] = myslot;
      seq[seq_no].delay[oo] = delay;
    }
  FastFileFini();

  /* Mark end-of-sequence */
  seq[seq_no].frame[oo] = 0;
  /* Length: inaccurate if 'set_frame_frame' is used */
  seq[seq_no].len = oo - 1;
  
  if (oo == 1)
    log_error("Sprite_load_pak error:  Couldn't load %s in %s.", crap, fullpath);
  free(fullpath);
}


/* Load sprite, either from a dir.ff pack (delegated to
   load_sprite_pak), either from a BMP file */
/* - seq_path_prefix: path to the file, relative to the current game (dink or dmod) */
/* - not_anim: reuse xoffset and yoffset from the first frame of the animation (misnomer) */
void load_sprites(char seq_path_prefix[100], int seq_no, int delay, int xoffset, int yoffset,
		  rect hardbox, int flags)
{
  char crap[200];
  char *fullpath = NULL;
  int use_fallback = 0;

  int notanim = 0, black = 0;
  if ((flags & DINKINI_NOTANIM) == DINKINI_NOTANIM)
    notanim = 1;
  if ((flags & DINKINI_BLACK) == DINKINI_BLACK)
    black = 1;

  if (no_running_main)
    draw_wait();


  /* Order: */
  /* - dmod/.../dir.ff */
  /* - dmod/.../...01.BMP */
  /* - ../dink/.../dir.ff */
  /* - ../dink/.../...01.BMP */
  char *seq_dirname = pdirname(seq_path_prefix);
  int exists = 0;

  if (!exists)
    {
      sprintf(crap, "%s/dir.ff", seq_dirname);
      fullpath = paths_dmodfile(crap);
      exists = exist(fullpath);
      free(fullpath);
      if (exists)
	{
	  free(seq_dirname);
	  load_sprite_pak(seq_path_prefix, seq_no, delay, xoffset, yoffset,
			  hardbox, flags, /*true*/1);
	  return;
	}
    }
  
  if (!exists)
    {
      sprintf(crap, "%s01.BMP",seq_path_prefix);
      fullpath = paths_dmodfile(crap);
      exists = exist(fullpath);
      free(fullpath);
    }

  if (!exists)
    {
      sprintf(crap, "%s/dir.ff",  seq_dirname);
      fullpath = paths_fallbackfile(crap);
      exists = exist(fullpath);
      free(fullpath);
      if (exists)
	{
	  free(seq_dirname);
	  load_sprite_pak(seq_path_prefix, seq_no, delay, xoffset, yoffset,
			  hardbox, flags, /*false*/0);
	  return;
	}
    }

  if (!exists)
    {
      /* Let's look for the BMP below */
      use_fallback = 1;
    }

  free(seq_dirname);


  /* If the sequence already exists, free it first */
  free_seq(seq_no);

  /* Load the whole sequence (prefix-01.bmp, prefix-02.bmp, ...) */
  int oo;
  for (oo = 1; oo <= MAX_FRAMES_PER_ABUSED_SEQUENCE; oo++)
    {
      int myslot = next_slot();
      if (myslot >= MAX_SPRITES)
	{
	  log_error("No sprite slot available! Index %d out of %d.",
		    myslot, MAX_SPRITES);
	  break;
	}

      FILE *in = NULL;
      char *leading_zero = NULL;
      if (oo < 10) leading_zero = "0"; else leading_zero = "";
      sprintf(crap, "%s%s%d.bmp", seq_path_prefix, leading_zero, oo);

      /* Set the pixel data */
      if (use_fallback)
	in = paths_fallbackfile_fopen(crap, "rb");
      else
	in = paths_dmodfile_fopen(crap, "rb");

      GFX_k[myslot].k = load_bmp_from_fp(in);

      if (GFX_k[myslot].k == NULL)
	{
	  // end of sequence
	  break;
	}

      /** Configure current frame **/
      
      /* Disable alpha in 32bit BMPs, like the original engine */
      SDL_SetAlpha(GFX_k[myslot].k, 0, SDL_ALPHA_OPAQUE);
      /* Set transparent color: either black or white */
      if (black)
	SDL_SetColorKey(GFX_k[myslot].k, SDL_SRCCOLORKEY|SDL_RLEACCEL,
			SDL_MapRGB(GFX_k[myslot].k->format, 0, 0, 0));
      else
	SDL_SetColorKey(GFX_k[myslot].k, SDL_SRCCOLORKEY|SDL_RLEACCEL,
			SDL_MapRGB(GFX_k[myslot].k->format, 255, 255, 255));
      
      /* Force RLE encoding now to save memory space */
      SDL_BlitSurface(GFX_k[myslot].k, NULL, GFX_lpDDSTrick2, NULL);
      /* Note: there is definitely a performance improvement when
	 using RLEACCEL under truecolor mode (~80%CPU -> 70%CPU) */


      /* Fill in .box; this was previously done in DDSethLoad; in
	 the future we could get rid of the .box field and rely
	 directly on SDL_Surface's .w and .h fields instead: */
      k[myslot].box.top = 0;
      k[myslot].box.left = 0;
      k[myslot].box.right = GFX_k[myslot].k->w;
      k[myslot].box.bottom = GFX_k[myslot].k->h;
      
      /* Define the offsets / center of the image */
      if (yoffset > 0)
	{
	  // explicitely set center
	  k[myslot].yoffset = yoffset;
	}
      else
	{
	  if (oo > 1 && notanim)
	    // copy first frame info
	    k[myslot].yoffset = k[seq[seq_no].frame[1]].yoffset;
	  else
	    // compute default center
	    k[myslot].yoffset = (k[myslot].box.bottom - (k[myslot].box.bottom / 4))
	      - (k[myslot].box.bottom / 30);
	}
      
      if (xoffset > 0)
	{
	  // explicitely set center
	  k[myslot].xoffset = xoffset;
	}
      else
	{
	  if (oo > 1 && notanim)
	    // copy first frame info
	    k[myslot].xoffset = k[seq[seq_no].frame[1]].xoffset;
	  else
	    // compute default center
	    k[myslot].xoffset = (k[myslot].box.right - (k[myslot].box.right / 2))
	      + (k[myslot].box.right / 6);
	}
      //ok, setup main offsets, lets build the hard block
      
      if (hardbox.right > 0)
	{
	  //forced setting
	  k[myslot].hardbox.left = hardbox.left;
	  k[myslot].hardbox.right = hardbox.right;
	}
      else
	{
	  //default setting
	  int work = k[myslot].box.right / 4;
	  k[myslot].hardbox.left -= work;
	  k[myslot].hardbox.right += work;
	}
      
      if (hardbox.bottom > 0)
	{
	  //forced setting
	  k[myslot].hardbox.top = hardbox.top;
	  k[myslot].hardbox.bottom = hardbox.bottom;
	}
      else
	{
	  //default setting
	  /* eg: graphics\dink\push\ds-p2- and
	     graphics\effects\comets\sm-comt2\fbal2- */
	  int work = k[myslot].box.bottom / 10;
	  k[myslot].hardbox.top -= work;
	  k[myslot].hardbox.bottom += work;
	}
      
      seq[seq_no].frame[oo] = myslot;
      seq[seq_no].delay[oo] = delay;
    }

  /* Mark end-of-sequence */
  seq[seq_no].frame[oo] = 0;
  /* Length: inaccurate if 'set_frame_frame' is used */
  seq[seq_no].len = oo - 1;
  
  /* oo == 1 => not even one sprite was loaded, error */
  /* oo > 1 => the sequence ends */

  if (oo == 1)
    {
      /* First frame didn't load! */
      log_error("load_sprites: anim '%s' not found: couldn't open '%s'", seq_path_prefix, crap);
    }
}

/**
 * Set the dink.ini / init() line for this sequence.
 */
void seq_set_ini(int seq_no, char *line)
{
  /* Check if we are not attempting to replace a line by itself
     (e.g. when a sequence is lazy-loaded) and free previous line. */
  if (seq[seq_no].ini != line)
    {
      if (seq[seq_no].ini != NULL)
	free(seq[seq_no].ini);
      seq[seq_no].ini = strdup(line);
    }
}
