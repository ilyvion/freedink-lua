/**
 * Draw background from tiles

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
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

#include <stdlib.h>
#include <string.h>
#include "game_engine.h"
#include "dinkvar.h"
#include "gfx.h"
#include "gfx_tiles.h"
#include "io_util.h"
#include "paths.h"
#include "sfx.h"
#include "log.h"

/* Tiles */
/* Game pieces */
/* +1 to avoid the -1 in array indexes.. */
SDL_Surface *GFX_tiles[NB_TILE_SCREENS+1];

/* Animated tiles current status */
int water_timer;
/*bool*/int fire_forward;
int fire_flip;


/* Local functions */
/* extern "C" IDirectDrawSurface * DDTileLoad(IDirectDraw *pdd, LPCSTR szBitmap, int dx, int dy, int sprite); */

/* TODO: move place_sprites_game() and kill_all_scripts() here, if all
   those sprite-related global variables must be shared with other C
   modules, otherwise let those functions in the sprite-related C
   modules. */


// Load the tiles from the BMPs
void tiles_load_default() {
  char crap[30];
  char crap1[10];
  int h;

  Msg("loading tilescreens...");
  for (h = 1; h <= NB_TILE_SCREENS; h++)
    {
      if (h < 10)
	strcpy(crap1,"0");
      else
	strcpy(crap1, "");      
      sprintf(crap, "tiles/Ts%s%d.BMP", crap1, h);

      tiles_load_slot(crap, h);

      if (GFX_tiles[h] == NULL)
	exit(0);
    }
  
  Msg("Done with tilescreens...");
}

void tiles_load_slot(char* relpath, int slot)
{
  FILE* in = paths_dmodfile_fopen(relpath, "rb");
  if (in == NULL)
    in = paths_fallbackfile_fopen(relpath, "rb");
  
  if (GFX_tiles[slot] != NULL)
    {
      SDL_FreeSurface(GFX_tiles[slot]);
      GFX_tiles[slot] = NULL;
    }

  GFX_tiles[slot] = load_bmp_from_fp(in);
  
  if (GFX_tiles[slot] == NULL) {
    fprintf(stderr, "Couldn't find tilescreen %s: %s\n", relpath, SDL_GetError());
  }
}

/**
 * Free memory used by tiles
 */
void tiles_unload_all(void) {
  int h = 0;
  for (h=1; h <= NB_TILE_SCREENS; h++)
    SDL_FreeSurface(GFX_tiles[h]);
}

/* extern "C" IDirectDrawSurface * DDTileLoad(IDirectDraw *pdd, LPCSTR szBitmap, int dx, int dy, int sprite) */
/* { */
/*     HBITMAP             hbm; */
/*     BITMAP              bm; */
/*     DDSURFACEDESC       ddsd; */
/*     IDirectDrawSurface *pdds; */
        
/*     // */
/*     //  try to load the bitmap as a resource, if that fails, try it as a file */
/*     // */
/*     hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION); */
        
/*     if (hbm == NULL) */
/*         hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION); */
        
/*     if (hbm == NULL) */
/*         return NULL; */
        
/*     // */
/*     // get size of the bitmap */
/*     // */
/*     GetObject(hbm, sizeof(bm), &bm);      // get size of bitmap */
        
/*     // */
/*     // create a DirectDrawSurface for this bitmap */
/*     // */
/*     ZeroMemory(&ddsd, sizeof(ddsd)); */
/*     ddsd.dwSize = sizeof(ddsd); */
/*     ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH; */
/*     ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; */
/*     ddsd.dwWidth = bm.bmWidth; */
/*     ddsd.dwHeight = bm.bmHeight; */
/*     if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK) */
/*         return NULL; */
        
/*     DDCopyBitmap(pdds, hbm, 0, 0, 0, 0); */
/*     tilerect[sprite].bottom = bm.bmHeight; */
/*         tilerect[sprite].right = bm.bmWidth; */
    
/*     DeleteObject(hbm); */
        
/*     return pdds; */
/* } */


/* Draw the background from tiles */
void draw_map_game(void)
{
/*   RECT                rcRect; */
  int pa, cool;
  int x;
              
  *pvision = 0;
                
  while (kill_last_sprite());
  kill_repeat_sounds();
  kill_all_scripts();

  /* 96 = 12 * 8 tile squares; 1 tile square = 50x50 pixels */
  for (x = 0; x < 96; x++)
    {
      cool = pam.t[x].num / 128;
      pa = pam.t[x].num - (cool * 128);

/*       rcRect.left = (pa * 50- (pa / 12) * 600); */
/*       rcRect.top = (pa / 12) * 50; */
/*       rcRect.right = rcRect.left + 50; */
/*       rcRect.bottom = rcRect.top + 50; */
/*       lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+1], */
/* 			 &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */
                        
      // GFX
      {
	SDL_Rect src;
	SDL_Rect dst;
	src.x = (pa * 50 - (pa / 12) * 600);
	src.y = (pa / 12) * 50;
	src.w = 50;
	src.h = 50;
	dst.x = (x * 50 - ((x / 12) * 600))+playl;
	dst.y = (x / 12) * 50;
	SDL_BlitSurface(GFX_tiles[cool+1], &src, GFX_lpDDSTwo, &dst);
      }
    }
                
                
  if (strlen(pam.script) > 1)
    {
      int ms = load_script(pam.script,0, /*true*/1);
                        
      if (ms > 0) 
	{
	  locate(ms, "main");
	  no_running_main = /*true*/1;
	  run_script(ms);
	  no_running_main = /*false*/0;
	}
    }
                
  place_sprites_game();
                
                
  //lets add the sprites hardness to the real hardness, adding it's own uniqueness to our collective.
                
                
  //if script for overall screen, run it
                
  //Msg("Done loading screen.");
  thisTickCount = SDL_GetTicks();
                
  init_scripts();
}
        
        
/* It's used at: freedink.cpp:restoreAll(), DinkC's draw_background(),
   stop_entire_game(). What's the difference with draw_map_game()?? */
void draw_map_game_background(void)
{
/*   RECT rcRect; */
  int pa, cool;
  int x;
              
  for (x = 0; x < 96; x++)
    {
      cool = pam.t[x].num / 128;
      pa = pam.t[x].num - (cool * 128);

/*       rcRect.left = (pa * 50- (pa / 12) * 600); */
/*       rcRect.top = (pa / 12) * 50; */
/*       rcRect.right = rcRect.left + 50; */
/*       rcRect.bottom = rcRect.top + 50; */
/*       lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+1], */
/* 			 &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */

      // GFX
      {
	SDL_Rect src;
	SDL_Rect dst;
	src.x = (pa * 50- (pa / 12) * 600);
	src.y = (pa / 12) * 50;
	src.w = 50;
	src.h = 50;
	dst.x = (x * 50 - ((x / 12) * 600))+playl;
	dst.y = (x / 12) * 50;
	SDL_BlitSurface(GFX_tiles[cool+1], &src, GFX_lpDDSTwo, &dst);
      }
                        
    }
                
  place_sprites_game_background();
}
        
/* Game-specific: animate background (water, fire, ...) */        
void process_animated_tiles(void)
{
/*   RECT rcRect; */
  int cool;
  int flip;
  int pa;
	
  //process water tiles
	
  if (water_timer < thisTickCount)
    {
      int x;

      water_timer = thisTickCount + ((rand() % 2000));
      flip = ((rand() % 2)+1);		
		
      for (x = 0; x < 96; x++)
	{
	  if (pam.t[x].num >= 896 && pam.t[x].num < (896+128))
	    {
				      cool = pam.t[x].num / 128;
				      pa = pam.t[x].num - (cool * 128);
/* 				      rcRect.left = (pa * 50- (pa / 12) * 600); */
/* 				      rcRect.top = (pa / 12) * 50; */
/* 				      rcRect.right = rcRect.left + 50; */
/* 				      rcRect.bottom = rcRect.top + 50; */
				
				
/* 				      lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+flip], */
/* 							 &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */

				      // GFX
				      {
					SDL_Rect src;
					SDL_Rect dst;
					src.x = (pa * 50- (pa / 12) * 600);
					src.y = (pa / 12) * 50;
					src.w = 50;
					src.h = 50;
					dst.x = (x * 50 - ((x / 12) * 600))+playl;
					dst.y = (x / 12) * 50;
					SDL_BlitSurface(GFX_tiles[cool+flip], &src, GFX_lpDDSTwo, &dst);
				      }
				    }	
	}
		
    }
	
  //end of water processing
	
	
  //if (water_timer < thisTickCount)
  {
    int x;
    //	water_timer = thisTickCount + ((rand() % 2000)+1000);
		
    if (fire_forward) fire_flip++;
    if (!fire_forward) fire_flip--;
		
    if (fire_flip < 1)
      {
	fire_flip = 5;
	fire_forward = /*false*/0;
      }
		
    //	if (fire_flip > 4)
    //	{
    //  fire_flip = 4;
    //fire_forward = false;
    //}
		
		
		
    for (x = 0; x < 96; x++)
      {
	if (pam.t[x].num >= 2304 && pam.t[x].num < (2304+128))
				   {
				
				     cool = pam.t[x].num / 128;
				     pa = pam.t[x].num - (cool * 128);
/* 				     rcRect.left = (pa * 50- (pa / 12) * 600); */
/* 				     rcRect.top = (pa / 12) * 50; */
/* 				     rcRect.right = rcRect.left + 50; */
/* 				     rcRect.bottom = rcRect.top + 50; */
				
				
/* 				     lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+fire_flip], */
/* 							&rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */
				
				     // GFX
				     {
				       SDL_Rect src;
				       SDL_Rect dst;
				       src.x = (pa * 50- (pa / 12) * 600);
				       src.y = (pa / 12) * 50;
				       src.w = 50;
				       src.h = 50;
				       dst.x = (x * 50 - ((x / 12) * 600))+playl;
				       dst.y = (x / 12) * 50;
				       SDL_BlitSurface(GFX_tiles[cool+fire_flip], &src, GFX_lpDDSTwo, &dst);
				     }
				   }	
      }
		
  }
	
  //end of water processing
}
