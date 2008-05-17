/**
 * Graphics - sprites management

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

#include "gfx_sprites.h"

#include "gfx.h"
#include "dinkvar.h"

#include "fastfile.h"
#include "io_util.h"
#include "log.h"
#include "paths.h"

/*
  External global variables in use:
  s_index[], seq[], GFX_k[], k[], no_running_main
*/


static int next_slot = 1;
static int please_wait = 0;

/**
 * Free memory used by sprites. It's not much useful in itself, since
 * it's only called when we're exiting the game, but it does avoid
 * memory leak warnings when FreeDink is analyzed by Valgrind or other
 * memory checkers.
 */
void sprites_unload(void)
{
  int i = 0;
  for (i = 0; i < MAX_SPRITES; i++)
    if (GFX_k[i].k != NULL)
      SDL_FreeSurface(GFX_k[i].k);
  for (i = 0; i < MAX_SEQUENCES; i++)
    if (seq[i].ini != NULL)
      free(seq[i].ini);
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

static void setup_anim(int seq_no, int delay)
{
  int o;
  for (o = 1; o <= seq[seq_no].len; o++)
    {
      seq[seq_no].frame[o] = seq[seq_no].base_index + o;
      seq[seq_no].delay[o] = delay;
    }
  seq[seq_no].frame[seq[seq_no].len + 1] = 0;
}


void load_sprite_pak(char seq_path_prefix[100], int seq_no, int speed, int xoffset, int yoffset,
		     rect hardbox, /*bool*/int notanim, /*bool*/int black, /*bool*/int leftalign, /*bool*/int samedir)
{
  char fname[20];
  char crap[200];
  int myslot = next_slot;

  /* If we're reloading a sequence, load_sprite_pak will overwrite the
     previous data. Note that indexes are not checked, so if the new
     sequence is longer than the old one, then it will overwrite
     sprites in other unrelated sequences. In FreeDink, we'll keep
     this behavior, until we're sure this weren't ever misused in a
     released D-Mod. */
  if (seq[seq_no].len != 0)
    myslot = seq[seq_no].base_index + 1;


  seq[seq_no].base_index = myslot - 1;

  if (no_running_main)
    draw_wait();

  char *seq_dirname = pdirname(seq_path_prefix);

  int num = strlen(seq_path_prefix) - strlen(seq_dirname)-1;
  char *fullpath = NULL;
  strcpy(fname, &seq_path_prefix[strlen(seq_path_prefix)-num]);
  sprintf(crap, "%s/dir.ff", seq_dirname);
  if (samedir)
    fullpath = paths_dmodfile(crap);
  else
    fullpath = paths_fallbackfile(crap);

  if (!FastFileInit(fullpath, 5))
    {
      Msg("Could not load dir.ff art file %s", crap);
      free(fullpath);
      free(seq_dirname);
      return;
    }
  free(fullpath);
  free(seq_dirname);


  int oo;
  for (oo = 1; oo <= MAX_FRAMES_PER_SEQUENCE; oo++)
    {
      if (myslot >= MAX_SPRITES)
	{
	  fprintf(stderr, "No sprite slot available! Index %d out of %d.\n",
		  myslot, MAX_SPRITES);
	  break;
	}

      char *leading_zero = NULL;
      //load sprite
      if (oo < 10) leading_zero = "0"; else leading_zero = "";
      sprintf(crap, "%s%s%d.bmp", fname, leading_zero, oo);

      HFASTFILE pfile = FastFileOpen(crap);

      if (pfile == NULL)
	break;
      
      // GFX
      Uint8 *buffer;
      SDL_RWops *rw;
      if (GFX_k[myslot].k != NULL)
	    SDL_FreeSurface(GFX_k[myslot].k);
      
      buffer = (Uint8 *) FastFileLock (pfile, 0, 0);
      rw = SDL_RWFromMem (buffer, FastFileLen (pfile));
      
      GFX_k[myslot].k = load_bmp_from_mem(rw); // auto free()
      // bmp_surf = IMG_Load_RW (rw, 0);
      if (GFX_k[myslot].k == NULL)
	{
	  fprintf(stderr, "Failed to load %s from fastfile\n", crap);
	  FastFileClose(pfile);
	  break;
	}
      
      // Palettes and transparency
      
      /* Note: in the original engine, no palette conversion was done
	 for sprite paks - they need to use the Dink Palette,
	 otherwise weird colors will appear! I think this was done for
	 efficiency. The transparent color was done manually: */

/* 	      if( ddrval == DD_OK ) */
/* 		{ */
/* 		  dib_pitch = (pbi->biWidth+3)&~3; */
/* 		  src = (BYTE *)pic + dib_pitch * (pbi->biHeight-1); */
/* 		  dst = (BYTE *)ddsd.lpSurface; */
/* 		  if (leftalign) */
/* 		    { */
/* 		      //Msg("left aligning.."); */

/* 		      for( y=0; y<(int)pbi->biHeight; y++ ) */
/* 			{ */
/* 			  for( x=0; x<(int)pbi->biWidth; x++ ) */
/* 			    { */
/* 			      dst[x] = src[x]; */
/* 			      if (dst[x] == 0) */
/* 				{ */
/* 				  // Msg("Found a 255..."); */
/* 				  dst[x] = 30; */
/* 				} else */
/* 				if (dst[x] == 255) */
/* 				  { */
/* 				    dst[x] = 249; */
/* 				  } */
/* 			    } */
/* 			  dst += ddsd.lPitch; */
/* 			  src -= dib_pitch; */
/* 			} */
/* 		    } */
/* 		  else if (black) */
/* 		    { */
/* 		      for( y=0; y<(int)pbi->biHeight; y++ ) */
/* 			{ */
/* 			  for( x=0; x<(int)pbi->biWidth; x++ ) */
/* 			    { */
/* 			      dst[x] = src[x]; */

/* 			      if (dst[x] == 0) */
/* 				{ */
/* 				  dst[x] = 30; */
/* 				} */
/* 			    } */
/* 			  dst += ddsd.lPitch; */
/* 			  src -= dib_pitch; */
/* 			} */
/* 		    } */
/* 		  else */
/* 		    { */
/* 		      //doing white */
/* 		      for( y=0; y<(int)pbi->biHeight; y++ ) */
/* 			{ */
/* 			  for( x=0; x<(int)pbi->biWidth; x++ ) */
/* 			    { */
/* 			      dst[x] = src[x]; */

/* 			      if (dst[x] == 255) */
/* 				{ */
/* 				  // Msg("Found a 255..."); */
/* 				  dst[x] = 249; */
/* 				} */
/* 			    } */
/* 			  dst += ddsd.lPitch; */
/* 			  src -= dib_pitch; */
/* 			} */
/* 		    } */

      
      /* TODO: perform the same manual palette conversion like
	 above? */
      if (leftalign)
	; // what are we supposed to do here?
      else if (black)
	/* We might want to directly use the hard-coded '0' index for
	   efficiency */
	SDL_SetColorKey(GFX_k[myslot].k, SDL_SRCCOLORKEY,
			SDL_MapRGB(GFX_k[myslot].k->format, 0, 0, 0));
      else
	/* We might want to directly use the hard-coded '255' index
	   for efficiency */
	SDL_SetColorKey(GFX_k[myslot].k, SDL_SRCCOLORKEY,
			SDL_MapRGB(GFX_k[myslot].k->format, 255, 255, 255));
      
      
      /* TODO: use SDL_RLEACCEL above? "RLE acceleration can
	 substantially speed up blitting of images with large
	 horizontal runs of transparent pixels" (man
	 SDL_SetColorKey) */
      
      
      k[myslot].box.top = 0;
      k[myslot].box.left = 0;
      k[myslot].box.right = GFX_k[myslot].k->w;
      k[myslot].box.bottom = GFX_k[myslot].k->h;
      
      if ( (oo > 1) & (notanim) )
	{
	  k[myslot].yoffset = k[seq[seq_no].base_index + 1].yoffset;
	}
      else
	{
	  if (yoffset > 0)
	    k[myslot].yoffset = yoffset; else
	    {
	      k[myslot].yoffset = (k[myslot].box.bottom -
				       (k[myslot].box.bottom / 4)) - (k[myslot].box.bottom / 30);
	    }
	}
      
      if ( (oo > 1 ) & (notanim))
	{
	  k[myslot].xoffset =  k[seq[seq_no].base_index + 1].xoffset;
	}
      else
	{
	  if (xoffset > 0)
	    k[myslot].xoffset = xoffset;
	  else
	    {
	      k[myslot].xoffset = (k[myslot].box.right -
				       (k[myslot].box.right / 2)) + (k[myslot].box.right / 6);
	    }
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
      
      if (black)
	{
/* 		      ddck.dwColorSpaceLowValue  = DDColorMatch(k[myslot].k, RGB(255,255,255)); */

/* 		      ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue; */
/* 		      k[myslot].k->SetColorKey(DDCKEY_SRCBLT, &ddck); */
	}
      else
	{
/* 		      ddck.dwColorSpaceLowValue  = DDColorMatch(k[myslot].k, RGB(0,0,0)); */
/* 		      ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue; */
/* 		      k[myslot].k->SetColorKey(DDCKEY_SRCBLT, &ddck); */
	}
      myslot++;
      FastFileClose(pfile);
    }
  
  if (oo == 1)
    fprintf(stderr, "Sprite_load_pak error:  Couldn't load %s.\n", crap);
  
  seq[seq_no].len = oo - 1;
  setup_anim(seq_no, speed);

  if (myslot > next_slot)
    // new sequence, not a reload
    next_slot = myslot;

  return;
}


/* Load sprite, either from a dir.ff pack (delegated to
   load_sprite_pak), either from a BMP file */
/* - seq_path_prefix: path to the file, relative to the current game (dink or dmod) */
/* - not_anim: reuse xoffset and yoffset from the first frame of the animation (misnomer) */
void load_sprites(char seq_path_prefix[100], int seq_no, int speed, int xoffset, int yoffset,
		  rect hardbox, /*bool*/int notanim, /*bool*/int black, /*bool*/int leftalign)
{
  char crap[200];
  char *fullpath = NULL;
  int use_fallback = 0;
  int myslot = next_slot;


  if (no_running_main)
    draw_wait();


  /* Order: */
  /* - dmod/.../dir.ff */
  /* - dmod/.../...01.BMP */
  /* - ../dink/.../dir.ff */
  /* - ../dink/.../...01.BMP */
  char *seq_dirname = pdirname(seq_path_prefix);
  sprintf(crap, "%s/dir.ff", seq_dirname);
  fullpath = paths_dmodfile(crap);
  //Msg("Checking for %s..", crap);
  if (exist(fullpath))
    {
      free(fullpath);
      free(seq_dirname);
      load_sprite_pak(seq_path_prefix, seq_no, speed, xoffset, yoffset,
		      hardbox, notanim, black, leftalign, /*true*/1);
      return;
    }
  free(fullpath);
  
  int exists = 0;
  sprintf(crap, "%s01.BMP",seq_path_prefix);
  fullpath = paths_dmodfile(crap);
  exists = exist(fullpath);
  free(fullpath);
  if (!exists)
    {
      sprintf(crap, "%s/dir.ff",  seq_dirname);
      fullpath = paths_fallbackfile(crap);
      //Msg("Checking for %s..", crap);
      exists = exist(fullpath);
      free(fullpath);
      if (exists)
	{
	  load_sprite_pak(seq_path_prefix, seq_no, speed, xoffset, yoffset,
			  hardbox, notanim, black, leftalign, /*false*/0);
	  free(seq_dirname);
	  return;
	}
      else
	{
	  use_fallback = 1;
	}
    }
  free(seq_dirname);


  seq[seq_no].base_index = myslot - 1;

  /* Load the whole sequence (prefix-01.bmp, prefix-02.bmp, ...) */
  int oo;
  for (oo = 1; oo <= MAX_FRAMES_PER_SEQUENCE; oo++)
    {
      if (myslot >= MAX_SPRITES)
	{
	  fprintf(stderr, "No sprite slot available! Index %d out of %d.\n",
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

      // Free previous surface before overwriting it (prevent
      // memory leak)
      if (GFX_k[myslot].k != NULL)
	{
	  SDL_FreeSurface(GFX_k[myslot].k);
	}


      GFX_k[myslot].k = load_bmp_from_fp(in);

      if (GFX_k[myslot].k == NULL)
	{
	  // end of sequence
	  break;
	}
      else
	{
	  /** Configure current frame **/

	  /* Fill in .box; this was previously done in DDSethLoad; in
	     the future we could get rid of the .box field and rely
	     directly on SDL_Surface's .w and .h fields instead: */
	  k[myslot].box.top = 0;
	  k[myslot].box.left = 0;
	  k[myslot].box.right = GFX_k[myslot].k->w;
	  k[myslot].box.bottom = GFX_k[myslot].k->h;
	  
	  /* Define the offsets / center of the image */

	  if (oo > 1 && notanim)
	    {
	      k[myslot].yoffset = k[seq[seq_no].base_index + 1].yoffset;
	    }
	  else
	    {
	      if (yoffset > 0)
		k[myslot].yoffset = yoffset;
	      else
		{
		  k[myslot].yoffset = (k[myslot].box.bottom -
				       (k[myslot].box.bottom / 4)) - (k[myslot].box.bottom / 30);
		}
	    }

	  if (oo > 1 && notanim)
	    {
	      k[myslot].xoffset = k[seq[seq_no].base_index + 1].xoffset;
	    }
	  else
	    {
	      if (xoffset > 0)
		k[myslot].xoffset = xoffset; else
		{
		  k[myslot].xoffset = (k[myslot].box.right -
				       (k[myslot].box.right / 2)) + (k[myslot].box.right / 6);
		}
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
	}

      if (leftalign)
	{
	  //     k[myslot].xoffset = 0;
	  //     k[myslot].yoffset = 0;
	}

      /* Set transparent color: either black or white */
      if (black)
	SDL_SetColorKey(GFX_k[myslot].k, SDL_SRCCOLORKEY,
			SDL_MapRGB(GFX_k[myslot].k->format, 0, 0, 0));
      else
	SDL_SetColorKey(GFX_k[myslot].k, SDL_SRCCOLORKEY,
			SDL_MapRGB(GFX_k[myslot].k->format, 255, 255, 255));
      myslot++;
    }

  /* oo == 1 => not even one sprite was loaded, error */
  /* oo > 1 => the sequence ends */
  
  if (oo == 1)
    {
      /* First frame didn't load! */
      fprintf(stderr, "load_sprites: couldn't open %s: %s\n", crap, SDL_GetError());
      Msg("load_sprites:  Anim %s not found.",seq_path_prefix);
    }
  /* Finalize sequence */
  seq[seq_no].len = oo - 1;
  setup_anim(seq_no, speed);

  // adust next slot index
  next_slot = myslot;

  return;
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
