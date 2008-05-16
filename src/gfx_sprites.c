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


int cur_sprite = 1;
int please_wait = 0;

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
      seq[seq_no].frame[o] = seq[seq_no].start + o;
      seq[seq_no].delay[o] = delay;
    }
  seq[seq_no].frame[seq[seq_no].len + 1] = 0;
}


void load_sprite_pak(char org[100], int seq_no, int speed, int xoffset, int yoffset,
		     rect hardbox, /*bool*/int notanim, /*bool*/int black, /*bool*/int leftalign, /*bool*/int samedir)
{
  int work;

  HFASTFILE                  pfile;
/*   BITMAPFILEHEADER UNALIGNED *pbf; */
/*   BITMAPINFOHEADER UNALIGNED *pbi; */
/*   DDSURFACEDESC       ddsd; */
/*   BITMAP              bm; */

/*   DDCOLORKEY          ddck; */

/*   int x,y,dib_pitch; */
/*   unsigned char *src, *dst; */
  char fname[20];

  //IDirectDrawSurface *pdds;

  int sprite = 71;
/*   /\*BOOL*\/int trans = /\*FALSE*\/0; */
  /*bool*/int reload = /*false*/0;

  char crap[200];

  int save_cur = cur_sprite;

  int oo;

  if (seq[seq_no].len != 0)
    {
      //  Msg("Saving sprite %d", save_cur);
      cur_sprite = seq[seq_no].start + 1;
      //Msg("Temp cur_sprite is %d", cur_sprite);
      reload = /*true*/1;
    }


  seq[seq_no].start = cur_sprite -1;

  if (no_running_main)
    draw_wait();

  char *org_dirname = pdirname(org);

  int num = strlen(org) - strlen(org_dirname)-1;
  char *fullpath = NULL;
  strcpy(fname, &org[strlen(org)-num]);
  sprintf(crap, "%s/dir.ff", org_dirname);
  if (samedir)
    fullpath = paths_dmodfile(crap);
  else
    fullpath = paths_fallbackfile(crap);

  if (!FastFileInit(fullpath, 5))
    {
      Msg("Could not load dir.ff art file %s", crap);
      cur_sprite = save_cur;
      free(fullpath);
      free(org_dirname);
      return;
    }
  free(fullpath);
  free(org_dirname);

  // No color conversion for sprite paks - they need to use the Dink
  // Palette, otherwise weird colors will appear!
  /*           if (!windowed)
	       {
	       lpDDPal->GetEntries(0,0,256,holdpal);
	       lpDDPal->SetEntries(0,0,256,real_pal);
	       }
  */

  for (oo = 1; oo <= 51; oo++)
    {
      char leading_zero[1+1];
      //load sprite
      sprite = cur_sprite;
      //if (reload) Msg("Ok, programming sprite %d", sprite);
      if (oo < 10)
	strcpy(leading_zero, "0");
      else
	strcpy(leading_zero, "");
      sprintf(crap, "%s%s%d.bmp", fname, leading_zero, oo);

      pfile = FastFileOpen(crap);

      if (pfile == NULL)
	{
	  FastFileClose(pfile);

	  //   FastFileFini();
	  if (oo == 1)
	    Msg("Sprite_load_pak error:  Couldn't load %s.",crap);

	  seq[seq_no].len = (oo - 1);
	  //      initFail(hWndMain, crap);
	  setup_anim(seq_no, speed);
	  //                           if (!windowed)  lpDDPal->SetEntries(0,0,256,holdpal);

	  //if (reload) Msg("Ok, tacking %d back on.", save_cur);
	  cur_sprite = save_cur;
	  return;
	}
      else
	{
	  //got file
/* 	  pbf = (BITMAPFILEHEADER *)FastFileLock(pfile, 0, 0); */
/* 	  pbi = (BITMAPINFOHEADER *)(pbf+1); */

/* 	  if (pbf->bfType != 0x4d42 || */
/* 	      pbi->biSize != sizeof(BITMAPINFOHEADER)) */
/* 	    { */
/* 	      Msg("Failed to load"); */
/* 	      Msg(crap); */
/* 	      cur_sprite = save_cur; */
/* 	      FastFileClose( pfile ); */
/* 	      //   FastFileFini(); */

/* 	      return; */
/* 	    } */

/* 	  byte *pic; */

/* 	  pic = (byte *)pbf + 1078; */

	  //Msg("Pic's size is now %d.",sizeof(pic));

/* 	  bm.bmWidth = pbi->biWidth; */
/* 	  bm.bmHeight = pbi->biHeight; */
/*  	  bm.bmWidthBytes = 32; */
/* 	  bm.bmPlanes = pbi->biPlanes; */
/* 	  bm.bmBitsPixel = pbi->biBitCount; */
/* 	  bm.bmBits = pic; */

	  //
	  // create a DirectDrawSurface for this bitmap
	  //
/* 	  ZeroMemory(&ddsd, sizeof(ddsd)); */
/* 	  ddsd.dwSize = sizeof(ddsd); */
/* 	  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH; */
/* 	  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; */
/* 	  ddsd.dwWidth = pbi->biWidth; */
/* 	  ddsd.dwHeight = pbi->biHeight; */

	  if (GFX_k[sprite].k != NULL)
	    {
/* 	      k[sprite].k->Release(); */
	      SDL_FreeSurface(GFX_k[sprite].k);
	    }

/* 	  if (lpDD->CreateSurface(&ddsd, &k[sprite].k, NULL) != DD_OK) */
/* 	    { */
/* 	      Msg("Failed to create pdd surface description"); */
/* 	    } */
/* 	  else */
/* 	    { */
/* 	      ddsd.dwSize = sizeof(ddsd); */
/* 	      ddrval = IDirectDrawSurface_Lock(k[sprite].k, NULL, &ddsd, DDLOCK_WAIT, NULL); */

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

/* 		  IDirectDrawSurface_Unlock(k[sprite].k, NULL); */


	  // GFX
	  /* TODO: perform the same manual palette conversion
	     like above? */
	  {
	    Uint8 *buffer;
	    SDL_RWops *rw;

	    buffer = (Uint8 *) FastFileLock (pfile, 0, 0);
	    rw = SDL_RWFromMem (buffer, FastFileLen (pfile));

	    GFX_k[sprite].k = load_bmp_from_mem(rw); // auto free()
	    // bmp_surf = IMG_Load_RW (rw, 0);
	    if (GFX_k[sprite].k == NULL)
	      {
		fprintf(stderr, "unable to load %s from fastfile", crap);
		return;
	      }

	    if (leftalign)
	      ; // ?
	    else if (black)

	      /* TODO: use SDL_RLEACCEL? "RLE acceleration can
		 substantially speed up blitting of images with large
		 horizontal runs of transparent pixels" (man
		 SDL_SetColorKey) */
	      /* We might want to directly use the hard-coded
		 '0' index for efficiency */
	      SDL_SetColorKey(GFX_k[sprite].k, SDL_SRCCOLORKEY,
			      SDL_MapRGB(GFX_k[sprite].k->format, 0, 0, 0));
	    else
	      /* We might want to directly use the hard-coded
		 '255' index for efficiency */
	      SDL_SetColorKey(GFX_k[sprite].k, SDL_SRCCOLORKEY,
			      SDL_MapRGB(GFX_k[sprite].k->format, 255, 255, 255));
	  }
/* 		} */
/* 	      else */
/* 		{ */
/* 		  Msg("Lock failed err=%d", ddrval); */
/* 		  //return; */
/* 		} */

	  if (sprite > 0)
	    {
	      k[sprite].box.top = 0;
	      k[sprite].box.left = 0;
	      k[sprite].box.right = GFX_k[sprite].k->w;
	      k[sprite].box.bottom = GFX_k[sprite].k->h;

	      if ( (oo > 1) & (notanim) )
		{
		  k[cur_sprite].yoffset = k[seq[seq_no].start + 1].yoffset;
		}
	      else
		{
		  if (yoffset > 0)
		    k[cur_sprite].yoffset = yoffset; else
		    {
		      k[cur_sprite].yoffset = (k[cur_sprite].box.bottom -
					       (k[cur_sprite].box.bottom / 4)) - (k[cur_sprite].box.bottom / 30);
		    }
		}

	      if ( (oo > 1 ) & (notanim))
		{
		  k[cur_sprite].xoffset =  k[seq[seq_no].start + 1].xoffset;
		    }
	      else
		{
		  if (xoffset > 0)
		    k[cur_sprite].xoffset = xoffset;
		  else
		    {
		      k[cur_sprite].xoffset = (k[cur_sprite].box.right -
					       (k[cur_sprite].box.right / 2)) + (k[cur_sprite].box.right / 6);
		    }
		}
	      //ok, setup main offsets, lets build the hard block

	      if (hardbox.right > 0)
		{
		  //forced setting
		  k[cur_sprite].hardbox.left = hardbox.left;
		  k[cur_sprite].hardbox.right = hardbox.right;
		}
	      else
		{
		  //guess setting
		  work = k[cur_sprite].box.right / 4;
		  k[cur_sprite].hardbox.left -= work;
		  k[cur_sprite].hardbox.right += work;
		}

	      if (hardbox.bottom > 0)
		{
		  k[cur_sprite].hardbox.top = hardbox.top;
		  k[cur_sprite].hardbox.bottom = hardbox.bottom;
		}
	      else
		{
		  work = k[cur_sprite].box.bottom / 10;
		  k[cur_sprite].hardbox.top -= work;
		  k[cur_sprite].hardbox.bottom += work;
		}

	      if (black)
		{
/* 		      ddck.dwColorSpaceLowValue  = DDColorMatch(k[cur_sprite].k, RGB(255,255,255)); */

/* 		      ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue; */
/* 		      k[cur_sprite].k->SetColorKey(DDCKEY_SRCBLT, &ddck); */
		}
	      else
		{
/* 		      ddck.dwColorSpaceLowValue  = DDColorMatch(k[cur_sprite].k, RGB(0,0,0)); */
/* 		      ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue; */
/* 		      k[cur_sprite].k->SetColorKey(DDCKEY_SRCBLT, &ddck); */
		    }
	      cur_sprite++;
	      if (!reload)
		save_cur++;
	    }
	  FastFileClose(pfile);
	}
    }
  // FastFileFini();
  return;
}


/* Load sprite, either from a dir.ff pack (delegated to
   load_sprite_pak), either from a BMP file */
/* - org: path to the file, relative to the current game (dink or dmod) */
/* - seq_no: sequence number */
void load_sprites(char org[100], int seq_no, int speed, int xoffset, int yoffset,
		  rect hardbox, /*bool*/int notanim, /*bool*/int black, /*bool*/int leftalign)
{
  int work;
  char crap[200], hold[5];
  char *fullpath;
  int oo;
  int exists = 0;
  int use_fallback = 0;

  if (no_running_main) draw_wait();

  char *org_dirname = pdirname(org);

  /* Order: */
  /* - dmod/.../dir.ff */
  /* - dmod/.../...01.BMP */
  /* - ../dink/.../dir.ff */
  /* - ../dink/.../...01.BMP */
  sprintf(crap, "%s/dir.ff", org_dirname);
  fullpath = paths_dmodfile(crap);
  //Msg("Checking for %s..", crap);
  if (exist(fullpath))
    {
      free(fullpath);
      free(org_dirname);
      load_sprite_pak(org, seq_no, speed, xoffset, yoffset, hardbox, notanim, black, leftalign, /*true*/1);
      return;
    }
  free(fullpath);

  sprintf(crap, "%s01.BMP",org);
  fullpath = paths_dmodfile(crap);
  exists = exist(fullpath);
  free(fullpath);
  if (!exists)
    {
      sprintf(crap, "%s/dir.ff",  org_dirname);
      fullpath = paths_fallbackfile(crap);
      //Msg("Checking for %s..", crap);
      exists = exist(fullpath);
      free(fullpath);
      if (exists)
	{
	  load_sprite_pak(org, seq_no, speed, xoffset, yoffset, hardbox, notanim, black, leftalign, /*false*/0);
	  free(org_dirname);
	  return;
	}
      use_fallback = 1;
    }
  free(org_dirname);

  seq[seq_no].start = cur_sprite -1;

  /* Load the whole sequence (prefix-01.bmp, prefix-02.bmp, ...) */
  for (oo = 1; oo <= 1000; oo++)
    {
      FILE *in = NULL;
      if (oo < 10) strcpy(hold, "0"); else strcpy(hold,"");
      sprintf(crap, "%s%s%d.BMP",org,hold,oo);

      /* Set the pixel data */
/*       k[cur_sprite].k = DDSethLoad(lpDD, crap, 0, 0, cur_sprite); */
      // GFX
      if (use_fallback)
	in = paths_fallbackfile_fopen(crap, "rb");
      else
	in = paths_dmodfile_fopen(crap, "rb");

      // Free previous surface before overwriting it (prevent
      // memory leak)
      if (GFX_k[cur_sprite].k != NULL)
	{
	  SDL_FreeSurface(GFX_k[cur_sprite].k);
	}

      GFX_k[cur_sprite].k = load_bmp_from_fp(in);
      if (GFX_k[cur_sprite].k == NULL && oo == 1)
	{
	  /* First frame didn't load! */
	  /* It's normal if we're at the end of a sequence */
	  fprintf(stderr, "load_sprites: couldn't open %s: %s\n", crap, SDL_GetError());
	}

      /* Define the offsets / center of the image */
      if (GFX_k[cur_sprite].k != NULL)
	{
	  k[cur_sprite].box.top = 0;
	  k[cur_sprite].box.left = 0;
	  k[cur_sprite].box.right = GFX_k[cur_sprite].k->w;
	  k[cur_sprite].box.bottom = GFX_k[cur_sprite].k->h;
	  if ((oo > 1) & (notanim))
	    {
	      k[cur_sprite].yoffset = k[seq[seq_no].start + 1].yoffset;
	    }
	  else
	    {
	      if (yoffset > 0)
		k[cur_sprite].yoffset = yoffset;
	      else
		{
		  k[cur_sprite].yoffset = (k[cur_sprite].box.bottom -
					   (k[cur_sprite].box.bottom / 4)) - (k[cur_sprite].box.bottom / 30);
		}
	    }

	  if ((oo > 1) & (notanim))
	    {
	      k[cur_sprite].xoffset = k[seq[seq_no].start + 1].xoffset;
	    }
	  else
	    {
	      if (xoffset > 0)
		k[cur_sprite].xoffset = xoffset; else
		{
		  k[cur_sprite].xoffset = (k[cur_sprite].box.right -
					   (k[cur_sprite].box.right / 2)) + (k[cur_sprite].box.right / 6);
		}
	    }
	  //ok, setup main offsets, lets build the hard block

	  if (hardbox.right > 0)
	    {
	      //forced setting
	      k[cur_sprite].hardbox.left = hardbox.left;
	      k[cur_sprite].hardbox.right = hardbox.right;
	    }
	  else
	    {
	      //default setting
	      work = k[cur_sprite].box.right / 4;
	      k[cur_sprite].hardbox.left -= work;
	      k[cur_sprite].hardbox.right += work;
	    }

	  if (hardbox.bottom > 0)
	    {
	      //forced setting
	      k[cur_sprite].hardbox.top = hardbox.top;
	      k[cur_sprite].hardbox.bottom = hardbox.bottom;
	    }
	  else
	    {
	      //default setting
	      /* eg: graphics\dink\push\ds-p2- and
		 graphics\effects\comets\sm-comt2\fbal2- */
	      work = k[cur_sprite].box.bottom / 10;
	      k[cur_sprite].hardbox.top -= work;
	      k[cur_sprite].hardbox.bottom += work;
	    }
	}

      if (leftalign)
	{
	  //     k[cur_sprite].xoffset = 0;
	  //     k[cur_sprite].yoffset = 0;
	}

      //add_text(crap,"LOG.TXT");

      if (GFX_k[cur_sprite].k == NULL)
	{
	  /* oo == 1 => not even one sprite was loaded, error */
	  /* oo > 1 => the sequence ends */

	  if (oo < 2)
	    {
	      Msg("load_sprites:  Anim %s not found.",org);
	    }

	  seq[seq_no].len = (oo - 1);
	  //       initFail(hWndMain, crap);
	  setup_anim(seq_no, speed);

	  return;
	}

      /* Set transparent color: either black or white */
      if (black)
	SDL_SetColorKey(GFX_k[cur_sprite].k, SDL_SRCCOLORKEY,
			SDL_MapRGB(GFX_k[cur_sprite].k->format, 0, 0, 0));
      else
	SDL_SetColorKey(GFX_k[cur_sprite].k, SDL_SRCCOLORKEY,
			SDL_MapRGB(GFX_k[cur_sprite].k->format, 255, 255, 255));
      cur_sprite++;
    }
}






/* Like DDLoadBitmap, except that we don't check the existence of
   szBitmap, and we define the .box sprite attribute with the
   dimentions of the picture */
/* Used in load_sprites() and in freedinkedit.cpp */
/* extern "C" IDirectDrawSurface * DDSethLoad(IDirectDraw *pdd, LPCSTR szBitmap, int dx, int dy, int sprite) */
/* { */
/*         HBITMAP             hbm; */
/*         BITMAP              bm; */
/*         DDSURFACEDESC       ddsd; */
/*         IDirectDrawSurface *pdds; */

/*         // */
/*         //  try to load the bitmap as a resource, if that fails, try it as a file */
/*         // */
/*         hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION); */

/*         if (hbm == NULL) */
/*                 hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION); */

/*         if (hbm == NULL) */
/*                 return NULL; */

/*         // */
/*         // get size of the bitmap */
/*         // */
/*         GetObject(hbm, sizeof(bm), &bm);      // get size of bitmap */

/*         // */
/*         // create a DirectDrawSurface for this bitmap */
/*         // */
/*         ZeroMemory(&ddsd, sizeof(ddsd)); */
/*         ddsd.dwSize = sizeof(ddsd); */
/*         ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH; */
/*         ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; */
/*         ddsd.dwWidth = bm.bmWidth; */
/*         ddsd.dwHeight = bm.bmHeight; */

/*         if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK) */
/*                 return NULL; */

/*         DDCopyBitmap(pdds, hbm, 0, 0, 0, 0); */

/*         DeleteObject(hbm); */
/*         if (sprite > 0) */
/*         { */
/*                 k[sprite].box.top = 0; */
/*                 k[sprite].box.left = 0; */
/*                 k[sprite].box.right = ddsd.dwWidth; */
/*                 k[sprite].box.bottom = ddsd.dwHeight; */

/*         } */



/*         return pdds; */
/* } */
