/**
 * FreeDink editor-specific code

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007  Sylvain Beucler

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
 * I count 10 modes in Dinkedit: map(1), screen tiles(3), screen
 * sprites(6), screen hardness(8), tile hardness(4), sprite
 * chooser(5), tile chooser(2), sprite hardness editor(7), plus input
 * dialog(0?) and the big mystery (9 - probably related to
 * hardness). */
/* TODO: The goal is to split the big keybinding functions into these
 * modes, and in each mode, call a function instead of inlining the
 * code. And we may use 'else if', or even a hashmap to do the
 * bindings.
*/

/* Use constants for readability */
#define MODE_MAP_PICKER 1
#define MODE_TILE_PICKER 2
#define MODE_SPRITE_PICKER 5

#define MODE_SCREEN_TILES 3
#define MODE_SCREEN_SPRITES 6
#define MODE_SCREEN_HARDNESS 8

#define MODE_TILE_HARDNESS 4
#define MODE_SPRITE_HARDNESS 7

#define MODE_DIALOG 0
#define MODE_UNKNOWN 9


#define NAME "DinkEdit"
#define TITLE "DinkEdit"
/* #define WIN32_LEAN_AND_MEAN */
#include <stdio.h>
#include <time.h>
/* for tolower */
#include <ctype.h>
/* #include <io.h> */
/* #include <direct.h> */
#include <unistd.h>
/* #include <windows.h> */
/* For GetStockBrush */
/* #include <windowsx.h> */
/* For VK_* */
/* #include <winuser.h> */
/* #include <mmsystem.h> */
/* #include <ddraw.h> */
/* #include <dsound.h> */

#include "SDL.h"
#include "SDL_rotozoom.h"

#include "init.h"
/* #include "ddutil.h" */
#include "dinkvar.h"
#include "fastfile.h"
#include "gfx.h"
#include "gfx_tiles.h"
#include "gfx_utils.h"
#include "gfx_fonts.h"
#include "sfx.h"
#include "resource.h"
#include "io_util.h"

//Dinkedit-only vars

map_info buffmap;
bool buf_mode = false;
char buf_path[100];
int buf_map = 0;

/* Save x and y coordinates for mode 4, 5 and 6 */
int m4x,m4y,m5x,m5y,m6x,m6y,m5ax,m5ay;
/* LPDIRECTDRAWCLIPPER lpClipper; */
int winoffset = 25;
int winoffsetx = 5;

int sp_base_walk = -1;
int sp_base_idle = -1;
int sp_base_attack = -1;
int sp_base_hit = -1;
int sp_base_die = -1;
int sp_gold, sp_defense, sp_strength, sp_exp, sp_hitpoints;
int sp_timer = 33;
int sp_que;
int sp_hard = 1;
int sp_sound = 0;
int sp_type = 1;
int sp_prop = 0;
int sp_warp_map = 0;
bool show_display = true;
int sp_picker = 0;
int sp_nohit = 0;
int sp_touch_damage = 0;
int sp_warp_x = 0;
int sp_warp_y = 0;
int sp_parm_seq = 0;
char sp_script[15];

int old_command;
int sp_cycle = 0;
  int cur_screen;
int selx = 1;
int sely = 1;
int last_mode = 0;
int last_modereal = 0;
int hold_warp_map, hold_warp_x,hold_warp_y;
int sp_seq,sp_frame = 0;


/* const int NUM_SOUND_EFFECTS = 6; */
const int NUM_SOUND_EFFECTS = 2;

/* TODO: there should be more resources in Dinkedit.exe - ask Seth */
typedef enum enum_EFFECT
{
    SOUND_STOP = 0,
/*     SOUND_THROW, */
    SOUND_JUMP,
/*     SOUND_STUNNED, */
/*     SOUND_BEARSTRIKE, */
/*     SOUND_BEARMISS, */
} EFFECT;

char szSoundEffects[NUM_SOUND_EFFECTS][PATH_MAX] =
{
    "stop.wav",
/*     "THROW.WAV", */
    "jump.wav"
/*     "STUNNED.WAV", */
/*     "STRIKE02.WAV", */
/*     "MISS02.WAV" */
};


int x = 640;
int y = 480;
RECT rc;
int cx;
int cy;
int speed;





// PROC NAMES

/*BOOL*/int initFail(char mess[200] );

bool getkey(int key);
char key_convert(int key);

void draw_map( void);
void draw_used( void);
/* void dderror(HRESULT hErr); */

int SInitSound();




/* Beuc: The only difference with flip_it() I can see is the call to
   restoreAll(). It's only used in DinkEdit and copy_bmp(). */
void flip_it_second(void)
{
/*         DDBLTFX     ddbltfx; */
        
        RECT rcRectSrc;    RECT rcRectDest;
/*         POINT p; */
        
/*         if (!windowed) */
/*         { */
                
/*                 while( 1 ) */
/*                 { */
/*                         ddrval = lpDDSPrimary->Flip(NULL,DDFLIP_WAIT ); */
/*                         if( ddrval == DD_OK ) */
/*                         { */
/*                                 break; */
/*                         } */
/*                         if( ddrval == DDERR_SURFACELOST ) */
/*                         { */
/*                                 if( ddrval != DD_OK ) */
/*                                 { */
/*                                         break; */
/*                                 } */
/*                         } */
/*                         if( ddrval != DDERR_WASSTILLDRAWING ) */
/*                         { */
                                
                                
/*                         } */
/*                 }  */
                
/*         } else */
/*         { */
/*                 //windowed mode, no flipping              */
/*                 p.x = 0; p.y = 0;     */
/*                 ClientToScreen(hWndMain, &p); */
/*                 GetClientRect(hWndMain, &rcRectDest); */
                
/*                 //rcRectDest.top += winoffset; */
/*                 rcRectDest.bottom = 480; */
/*                 rcRectDest.right = 640; */
                
/*                 OffsetRect(&rcRectDest, p.x, p.y); */
/*                 SetRect(&rcRectSrc, 0, 0, 640, 480); */
                
/*                 ddbltfx.dwSize = sizeof(ddbltfx); */
                
/*                 ddbltfx.dwDDFX = DDBLTFX_NOTEARING; */
/*                 ddrval = lpDDSPrimary->Blt( &rcRectDest, lpDDSBack, &rcRectSrc, DDBLT_DDFX | DDBLT_WAIT, &ddbltfx); */
		// GFX
		{
		  // TODO: work directly on either lpDDSBack or
		  // lpDDSPrimary: the double buffer (Back) is managed
		  // by SDL, and SDL_Flip is used to refresh the
		  // physical screen (Primary), so only one of them is
		  // necessary.
		  SDL_BlitSurface(GFX_lpDDSBack, NULL, GFX_lpDDSPrimary, NULL);
		  
		  if (trigger_palette_change)
		    {
		      // Apply the logical palette to the physical
		      // screen. This may trigger a Flip (so don't do
		      // that until Back is read), but not necessarily
		      // (so do a Flip anyway).
		      SDL_SetPalette(GFX_lpDDSPrimary, SDL_PHYSPAL,
				     cur_screen_palette, 0, 256);
		      trigger_palette_change = 0;
		    }
		  SDL_Flip(GFX_lpDDSPrimary);
		}
/* 	} */
}


/*
 * restoreAll
 *
 * restore all lost objects
 */

/* HRESULT restoreAll( void ) */
/* { */
/*     HRESULT     ddrval; */
/* 	RECT rcRect; */
/*     ddrval = lpDDSPrimary->Restore(); */
/*     if( ddrval == DD_OK ) */
/*     { */
            
		
		
/* 		ddrval = lpDDSTwo->Restore(); */
/* //reload_batch(); */


/* Msg("Restoring some stuff."); */

/* /\*  for (int oo = 1; oo < 9; oo++) */
/*   { */

/*         ddrval = k[oo].k->Restore(); */
/*         if( ddrval == DD_OK ) */
/*         { */
/*   sprintf(crap, "TILES\\S%d.BMP",oo); */
/*             DDReLoadBitmap(k[oo].k, crap); */
/*         } */

/*   } */

  
/*       for (int h=1; h < tile_screens; h++) */
/* 	{ */
/*         ddrval = tiles[h]->Restore(); */
  	 
/*         if( ddrval == DD_OK ) */
/*         { */
/*       	 if (h < 10) strcpy(crap1,"0"); else strcpy(crap1, ""); */
/* 		sprintf(crap, "TILES\\TS%s%d.BMP",crap1,h); */
/*   Msg("Loaded tilescreen %d",h);  */
/* //		sprintf(crap, "TS%d.BMP",h); */
/* 		DDReLoadBitmap(tiles[h], crap);  */
	 
/* 		} */
/* 	  } */


/* *\/ */

/*   rcRect.left = 0; */
/*   rcRect.top = 0; */

/* // if (mode == 0) */
/* if (mode == MODE_DIALOG) */
/* { */
/*     rcRect.right = x; */
/*     rcRect.bottom = y;	 */
/* //lpDDSTwo->BltFast( 0, 0, game[15],  &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */

/* } */

/* // if (mode == 1) draw_used(); */
/* // if (mode == 3) draw_map(); */
/* // if (mode == 6) draw_map(); */
/* // if (mode == 7) draw_map(); */
/* if (mode == MODE_MAP_PICKER) draw_used(); */
/* if (mode == MODE_SCREEN_TILES) draw_map(); */
/* if (mode == MODE_SCREEN_SPRITES) draw_map(); */
/* if (mode == MODE_SPRITE_HARDNESS) draw_map(); */

/*     rcRect.right = 600; */
/*     rcRect.bottom = 450; */
/*     //    if (mode == 2) */
/*     if (mode == MODE_TILE_PICKER) */
/*       { */
/* 	lpDDSTwo->BltFast( 0, 0, tiles[cur_screen],  &tilerect[cur_screen], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
/* 	// GFX */
/* 	SDL_BlitSurface(GFX_tiles[cur_screen], NULL, GFX_lpDDSTwo, NULL); */
/*       } */
/* 	} */
/*     return ddrval; */

/* } /\* restoreAll *\/ */

void draw_sprite(SDL_Surface *GFX_lpdest, int h)
{
  RECT box_crap,box_real;
/*   HRESULT             ddrval; */
/*   DDBLTFX     ddbltfx; */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
/*   ddbltfx.dwFillColor = 0; */
  
  if (get_box(h, &box_crap, &box_real))
    {
/*       while(1) */
/* 	{ */
/* 	again: */
/* 	  ddrval = lpdest->Blt(&box_crap, k[getpic(h)].k, */
/* 			       &box_real, DDBLT_KEYSRC, &ddbltfx); */

	  // GFX
	  /* Classical scaling - copy/paste from dinkvar.cpp */
	  {
	    SDL_Rect src, dst;
	    SDL_Surface *scaled;
	    double sx, sy;
	    src.x = box_real.left;
	    src.y = box_real.top;
	    src.w = box_real.right - box_real.left;
	    src.h = box_real.bottom - box_real.top;
	    dst.x = box_crap.left;
	    dst.y = box_crap.top;
	    dst.w = box_crap.right - box_crap.left;
	    dst.h = box_crap.bottom - box_crap.top;
	    sx = 1.0 * dst.w / src.w;
	    sy = 1.0 * dst.h / src.h;
	    /* In principle, double's are precised up to 15 decimal
	       digits */
	    if (fabs(sx-1) > 1e-10 || fabs(sy-1) > 1e-10)
	      {
		scaled = zoomSurface(GFX_k[getpic(h)].k, sx, sy, SMOOTHING_OFF);
		/* Disable transparency if it wasn't active in the
		   source surface (SDL_gfx bug, report submitted to
		   the author) */
		if ((GFX_k[getpic(h)].k->flags & SDL_SRCCOLORKEY) == 0)
		  SDL_SetColorKey(scaled, 0, 0);
		src.x = (int) round(src.x * sx);
		src.y = (int) round(src.y * sy);
		src.w = (int) round(src.w * sx);
		src.h = (int) round(src.h * sy);
		SDL_BlitSurface(scaled, &src, GFX_lpdest, &dst);
		SDL_FreeSurface(scaled);
	      }
	    else
	      {
		/* No scaling */
		SDL_BlitSurface(GFX_k[getpic(h)].k, &src, GFX_lpdest, &dst);
	      }
	  }

/* 	  if (ddrval != DD_OK) */
/* 	    { */
/* 	      if (ddrval == DDERR_WASSTILLDRAWING) */
/* 		goto again; */
	      
/* 	      //dderror(ddrval); */
/* 	      dderror(ddrval); */
/* 	      if (draw_map_tiny > 0)  */
/* 		{ */
/* 		  Msg("MainSpriteDraw(): Could not draw sprite %d, pic %d. (Seq %d, Fram %d) (map %d)",h,getpic(h),spr[h].pseq, spr[h].pframe, draw_map_tiny); */
/* 		  Msg("Box_crap: %d %d %d %d, Box_real: %d %d %d %d",box_crap.left,box_crap.top, */
/* 		      box_crap.right, box_crap.bottom,box_real.left,box_real.top, */
/* 		      box_real.right, box_real.bottom); */
/* 		} */
/* 	      else */
/* 		{ */
/* 		  Msg("MainSpriteDraw(): Could not draw sprite %d, pic %d. (map %d)",h,getpic(h), cur_map); */
/* 		} */
/* 	      check_sprite_status(h); */
/* 	      break; */
/* 	    } */
/* 	  break; */
/* 	} */
    }
}

	
	void place_sprites(void )
	{
		int sprite;
		int que;
	int highest_sprite;
	/*BOOL*/int bs[max_sprites_at_once];
	int rank[max_sprites_at_once];

	
	memset(&bs,0,sizeof(bs));
		
		for (int r1 = 1; r1 < 100; r1++)
		{
			
			highest_sprite = 2000; //more than it could ever be
			
			rank[r1] = 0;
			
			for (int h1 = 1; h1 < 100;  h1++)
			{
					if (bs[h1] == /*FALSE*/0)
					{
						if (pam.sprite[h1].que == 0) que = pam.sprite[h1].y; else 
							
						{
							que = pam.sprite[h1].que;
				//		Msg("hahha.. que is not 0.");
						
						}
							
							if ( que < highest_sprite )
						{
							highest_sprite = que;
							rank[r1] = h1;
						}
						
					}
					
				
			}
			if (rank[r1] != 0)	
				bs[rank[r1]] = /*TRUE*/1;
		}
		
     

		
		int j;
		
		for (int oo =1; rank[oo] > 0; oo++)
		{
//Msg("O				k, rank[%d] is %d.",oo,rank[oo]);		
			j = rank[oo];
				
			if (j >= max_sprites_at_once)
			{
          j = 1;
		  Msg("Trying to process sprite %d, why?",j);
			}
			
			if (pam.sprite[j].active == true) if ( ( pam.sprite[j].vision == 0) || (pam.sprite[j].vision == map_vision))
			{
				//we have instructions to make a sprite

				if (  (pam.sprite[j].type == 0)  | (pam.sprite[j].type == 2) )
				{
				//make it part of the background (much faster)
					
				sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y,0,
				pam.sprite[j].seq,pam.sprite[j].frame,
				pam.sprite[j].size);
				spr[sprite].hard = pam.sprite[j].hard;

			    check_sprite_status(sprite);
				spr[sprite].sp_index = j;
					
				CopyRect(&spr[sprite].alt , &pam.sprite[j].alt);

				if (pam.sprite[j].type == 0)
				  draw_sprite(GFX_lpDDSTwo, sprite);
				
				
				if (spr[sprite].hard == 0)
				{
				if (pam.sprite[j].prop == 0)
					add_hardness(sprite, 1); else add_hardness(sprite,100+j);
				
				
				}
				
				
				spr[sprite].active = false;
				}
				
				if (pam.sprite[j].type == 1)
				{
				//make it a living sprite
					
				sprite =	add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y,0,
					pam.sprite[j].seq,pam.sprite[j].frame,
					pam.sprite[	j].size);
	
				spr[sprite].que = pam.sprite[j].que;
				check_sprite_status(sprite);
				spr[sprite].hard = pam.sprite[j].hard;
			   
				CopyRect(&spr[sprite].alt , &pam.sprite[j].alt);
				
				if (spr[sprite].hard == 0)
				{
				if (pam.sprite[j].prop == 0)
					add_hardness(sprite, 1); else add_hardness(sprite,100+j);
				
				
				}
				
				
				
				
				}
			}
			
		}
		
		
	}
	


/* Draw background from tiles */
void draw_map(void)
{
  RECT rcRect;
  int pa, cool,crap;   
  
  /* Replaced by a call to fill_screen(0) */
  fill_screen(0);
  /*
  DDBLTFX     ddbltfx;
  ZeroMemory(&ddbltfx, sizeof(ddbltfx));
  ddbltfx.dwSize = sizeof( ddbltfx);
  ddbltfx.dwFillColor = 0;
  crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
  */
  fill_whole_hard();
  
  while (kill_last_sprite()); 
  
  for (int x=0; x<96; x++)
    {
      cool = pam.t[x].num / 128;
      pa = pam.t[x].num - (cool * 128);
      rcRect.left = (pa * 50- (pa / 12) * 600);
      rcRect.top = (pa / 12) * 50;
      rcRect.right = rcRect.left + 50;
      rcRect.bottom = rcRect.top + 50;
      
      
/*       lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+1], */
/* 			 &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */
      // GFX
      {
	SDL_Rect src, dst;
	src.x = (pa * 50- (pa / 12) * 600);
	src.y = (pa / 12) * 50;
	src.w = src.h = 50;
	dst.x = (x * 50 - ((x / 12) * 600))+playl;
	dst.y = (x / 12) * 50;
 	SDL_BlitSurface(GFX_tiles[cool+1], &src, GFX_lpDDSTwo, &dst);
      }
    }
  place_sprites();
}




/* Draw the currently selected tile square (in the bottom-right corner
   of the screen) */
void draw_current( void)
{
  RECT rcRect;
  int x,cool;
  cool = cur_tile / 128;
  x = cur_tile - (cool * 128);
  
  //x = cur_tile; 
  rcRect.left = (x * 50- (x / 12) * 600);
  rcRect.top = (x / 12) * 50;
  rcRect.right = rcRect.left + 50;
  rcRect.bottom = rcRect.top + 50;
  
  //(((spr[1].y+1)*12) / 50)+(spr[1].x / 50) );	
/*   lpDDSTwo->BltFast( 590,430 , tiles[cool+1], */
/* 		     &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */
  //GFX
  {
    SDL_Rect src, dst = {590, 430};
    src.x = (x * 50- (x / 12) * 600);
    src.y = (x / 12) * 50;
    src.h = src.w = 50;
    SDL_BlitSurface(GFX_tiles[cool+1], &src, GFX_lpDDSTwo, &dst);
  }
}

/* Edit a tile hardness - show semi-transparent red/blue/orange
   squares on pixels with different kinds of hardness */
void draw_hard( void)
{
 //RECT                rcRect;
 
  for (int x=0; x<50; x++)
    {
      for (int y=0; y<50; y++)
	{
	  /* red */
	  if (hmap.tile[hard_tile].x[x].y[y] == 1)
	    {
/* 	      lpDDSBack->BltFast(95+(x*9), y*9, k[seq[10].frame[2]].k, */
/* 				 &k[seq[10].frame[2]].box, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT); */
	      // GFX
	      {
		SDL_Rect dst;
		dst.x = 95 + x*9;
		dst.y = y*9;
		SDL_BlitSurface(GFX_k[seq[10].frame[2]].k, NULL, GFX_lpDDSBack, &dst);
	      }
	    }

	  /* blue */
	  if (hmap.tile[hard_tile].x[x].y[y] == 2)
	    {
/* 	      lpDDSBack->BltFast(95+(x*9),y*9, k[seq[10].frame[9]].k, */
/* 				 &k[seq[10].frame[9]].box, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT); */
	      // GFX
	      {
		SDL_Rect dst;
		dst.x = 95 + x*9;
		dst.y = y*9;
		SDL_BlitSurface(GFX_k[seq[10].frame[9]].k, NULL, GFX_lpDDSBack, &dst);
	      }
	    }

	  /* orange */
	  if (hmap.tile[hard_tile].x[x].y[y] == 3)
	    {
/* 	      lpDDSBack->BltFast(95+(x*9),y*9, k[seq[10].frame[10]].k, */
/* 				 &k[seq[10].frame[10]].box, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT); */
	      // GFX
	      {
		SDL_Rect dst;
		dst.x = 95 + x*9;
		dst.y = y*9;
		SDL_BlitSurface(GFX_k[seq[10].frame[10]].k, NULL, GFX_lpDDSBack, &dst);
	      }
	    }
	}
    }
}


void
draw_this_map(map_info* pmap)
{
  for (int x=0; x<768; x++)
    {
      /* Blue square - unused screen */
      if (pmap->loc[x+1] == 0)
	{
/* 	  lpDDSTwo->BltFast((x) * 20 - ((x / 32) * 640), (x / 32) * 20, k[seq[10].frame[6]].k, */
/* 			    &k[seq[10].frame[6]].box, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */
	  // GFX
	  {
	    SDL_Rect dst;
	    SDL_Surface *sprite;
	    sprite = GFX_k[seq[10].frame[6]].k;
	    dst.x = x * 20 - x/32*640;
	    dst.y = x/32 * 20;
	    /* We need to avoid transparency though */
	    Uint32 colorkey = sprite->format->colorkey;
	    SDL_SetColorKey(sprite, 0, 0); /* Temporarily disable transparent color */
	    SDL_BlitSurface(sprite, NULL, GFX_lpDDSTwo, &dst);
	    SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, colorkey);
	  }
	}
      /* Red square - used screen */
      if (pmap->loc[x+1] > 0)
	{
/* 	  lpDDSTwo->BltFast((x) * 20 - ((x / 32) * 640), (x / 32) * 20, k[seq[10].frame[7]].k, */
/* 			    &k[seq[10].frame[7]].box, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
	  // GFX
	  {
	    SDL_Rect dst;
	    SDL_Surface *sprite = GFX_k[seq[10].frame[7]].k;
	    dst.x = x * 20 - x/32*640;
	    dst.y = x/32 * 20;
	    /* We need to avoid transparency though */
	    Uint32 colorkey = sprite->format->colorkey;
	    SDL_SetColorKey(sprite, 0, 0); /* Temporarily disable transparent color */
	    SDL_BlitSurface(sprite, NULL, GFX_lpDDSTwo, &dst);
	    SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, colorkey);
	  }
	}
      
      /* M mark - screen has MIDI */
      if (pmap->music[x+1] != 0)
	{
/* 	  lpDDSTwo->BltFast((x) * 20 - ((x / 32) * 640), (x / 32) * 20, k[seq[10].frame[12]].k, */
/* 			    &k[seq[10].frame[12]].box, DDBLTFAST_SRCCOLORKEY| DDBLTFAST_WAIT ); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = x * 20 - x/32*640;
	    dst.y = x/32 * 20;
	    SDL_BlitSurface(GFX_k[seq[10].frame[12]].k, NULL, GFX_lpDDSTwo, &dst);
	  }
	}
      /* S mark - screen has screentype / is indoor */
      if (pmap->indoor[x+1] != 0)
	{
/* 	  lpDDSTwo->BltFast( (x) * 20 - ((x / 32) * 640), (x / 32) * 20, k[seq[10].frame[13]].k, */
/*            &k[seq[10].frame[13]].box, DDBLTFAST_SRCCOLORKEY| DDBLTFAST_WAIT ); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = x * 20 - x/32*640;
	    dst.y = x/32 * 20;
	    SDL_BlitSurface(GFX_k[seq[10].frame[13]].k, NULL, GFX_lpDDSTwo, &dst);
	  }
	}
    }
}

/* Draw all squares in the map picker mode, including the 'M' (midi)
   and 'S' (screentype) marks */
void draw_used(void)
{
  /*box_crap.top = spr[h].y + k[spr[h].pic].hardbox.top;
    box_crap.bottom = spr[h].y + k[spr[h].pic].hardbox.bottom;
    box_crap.left = spr[h].x + k[spr[h].pic].hardbox.left;
    box_crap.right = spr[h].x + k[spr[h].pic].hardbox.right;
  */
  //lpDDSBack->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL, &ddbltfx);
  draw_this_map(&map);
}




bool load_map_buf(const int num)
{
FILE *          fp;
long holdme,lsize;
char crap[120];
//RECT box;
// play.map = num;
//Msg("Loading map %d...",num);
sprintf(crap, "%sMAP.DAT", buf_path);
 fp = fopen(ciconvert(crap), "rb");
			 if (!fp)
			 {
              Msg("Cannot find MAP.DAT file!!!");
			 return(false);
			 }
			 lsize = sizeof(struct small_map);
			 holdme = (lsize * (num-1));
			 fseek( fp, holdme, SEEK_SET);
			 //Msg("Trying to read %d bytes with offset of %d",lsize,holdme);
			 int shit = fread( &pam, lsize, 1, fp);       /* current player */
			 //Msg("Read %d bytes.",shit);
			 fclose(fp);

return(true);
	}

void load_info_buff(void)
{
FILE *          fp;
char crap[120];
 sprintf(crap, "%sDINK.DAT", buf_path);


 fp = fopen(ciconvert(crap), "rb");
            if (fp)
				{
	       Msg("World data loaded."); 
      	fread(&buffmap,sizeof(struct map_info),1,fp);
   	 fclose(fp);
				buf_mode = true;
				} else
				{
        Msg("World not found in %s.", buf_path);
		buf_mode = false;
				}

}

/* draw_used() but on a different map ('L' in map picker mode) */
void draw_used_buff(void)
{
  load_info_buff();
  
  if (!buf_mode) 
    {
      //failed
      /* TODO: display error message to the user */
      draw_used();
      return;
    }
  
  draw_this_map(&buffmap);
}


int add_new_map(void)
{

FILE *          fp;
long now;
char crap[80];
 sprintf(crap, "MAP.DAT");


 fp = fopen(ciconvert(crap), "a+");
				fwrite(&pam,sizeof(struct small_map),1,fp);
				now = (ftell(fp) / (sizeof(struct small_map)));
				fclose(fp);
return(now);
}

int add_new_map_buff(void)
{

FILE *          fp;
long now;
 
char crap[120];
 sprintf(crap, "%sMAP.DAT", buf_path);

 fp = fopen(ciconvert(crap), "a+");
				fwrite(&pam,sizeof(struct small_map),1,fp);
				now = (ftell(fp) / (sizeof(struct small_map)));
				fclose(fp);
return(now);
}





/* Set the keyboard state in sjoy */
void check_keyboard(void)
{
  /* GetKeyboard (which calls W32API's GetAsyncKeyState) returns a
     combined value in a short int; the most-significant bit indicates
     if the key is currently pressed; the least significant bit
     indicates if the key was pressed since the last call. */
  
  /* Check if the key was just pressed, or only maintained pressed */
  for (int x=1; x <=255; x++)
    {
      /* Put the current keyboard state in cache */
      /* getkey() then can check sjoy.realkey - that is, from the cache */
      sjoy.realkey[x] = GetKeyboard(x);

      sjoy.key[x] = /*FALSE*/0;
      if (getkey(x))
	{
	  if (sjoy.kletgo[x] == /*TRUE*/1) 
	    /* We just changed from "released" to "pressed" */
	    {
	      sjoy.key[x] = /*TRUE*/1;
	    }
	  sjoy.kletgo[x] = /*FALSE*/0;
	}
      else
	sjoy.kletgo[x] = /*TRUE*/1;	
    }
  sjoy.kletgo[SDLK_LSHIFT] = /*TRUE*/1;
  sjoy.kletgo[SDLK_RSHIFT] = /*TRUE*/1;
}


void check_joystick(void)
{

int ddrval;
int total;
//memset(&sjoy,0,sizeof(sjoy));

      for (int e2=1; e2 <=10; e2++) 
	  {
		  sjoy.joybit[e2] = /*FALSE*/0;
	  	  
	  }
sjoy.right = /*FALSE*/0;
sjoy.left = /*FALSE*/0;
sjoy.up = /*FALSE*/0;
sjoy.down = /*FALSE*/0;


if (joystick)
{
// memset(&jinfo,0,sizeof(JOYINFOEX));
// jinfo.dwSize=sizeof(JOYINFOEX);
// jinfo.dwFlags=JOY_RETURNALL;
// ddrval = joyGetPosEx(JOYSTICKID1,&jinfo);

// total = jinfo.dwButtons;

//  if ((total - 512) >= 0)
//  {
//  	 sjoy.joybit[10] = TRUE;
// 	 total = total - 512;
//  }

//  if ((total - 256) >= 0)
//  {
//  	 sjoy.joybit[9] = TRUE;
// 	 total = total - 256;
//  }

//  if ((total - 128) >= 0)
//  {
//  	 sjoy.joybit[8] = TRUE;
// 	 total = total - 128;
//  }

//  if ((total - 64) >= 0)
//  {
//  	 sjoy.joybit[7] = TRUE;
// 	 total = total - 64;
//  }

//  if ((total - 32) >= 0)
//  {
//  	 sjoy.joybit[6] = TRUE;
// 	 total = total - 32;
//  }

//  if ((total - 16) >= 0)
//  {
//  	 sjoy.joybit[5] = TRUE;
// 	 total = total - 16;
//  }

//  if ((total - 8) >= 0)
//  {
//  	 sjoy.joybit[4] = TRUE;
// 	 total = total - 8;
//  }

//  if ((total - 4) >= 0)
//  {
//  	 sjoy.joybit[3] = TRUE;
// 	 total = total - 4;
//  }

//  if ((total - 2) >= 0)
//  {
//  	 sjoy.joybit[2] = TRUE;
// 	 total = total - 2;
//  }

//  if ((total - 1) >= 0)
//  {
//  	 sjoy.joybit[1] = TRUE;
// 	 total = total - 1;
//  }

      
      
//  if (jinfo.dwXpos > 40000) sjoy.right = TRUE;
//  if (jinfo.dwXpos < 25000) sjoy.left = TRUE;
//  if (jinfo.dwYpos > 40000) sjoy.down = TRUE;
//  if (jinfo.dwYpos < 25000) sjoy.up = TRUE;

}

if (GetKeyboard(SDLK_ESCAPE /* 27 */)) sjoy.joybit[1] = /*TRUE*/1; //esc
if (GetKeyboard(SDLK_RETURN /* 13 */)) sjoy.joybit[2] = /*TRUE*/1;
if (GetKeyboard('x' /* 88 */)) sjoy.joybit[3] = /*TRUE*/1;
if (GetKeyboard('z' /* 90 */)) sjoy.joybit[4] = /*TRUE*/1;

if (GetKeyboard(SDLK_TAB /* 9 */)) sjoy.joybit[5] = /*TRUE*/1; //tab



for (int x5=1; x5 <=10; x5++) sjoy.button[x5] = /*FALSE*/0; 
	
	  
	  for (int x=1; x <=10; x++)
			 
		 {
		 if (sjoy.joybit[x])
		 {
			 if (sjoy.letgo[x] == /*TRUE*/1) 
			 {
			 sjoy.button[x] = /*TRUE*/1;
			 sjoy.letgo[x] = /*FALSE*/0;
			 }
			
		 }
		 }


for (int x2=1; x2 <=10; x2++) 
	  {
		if (sjoy.joybit[x2])  sjoy.letgo[x2] = /*FALSE*/0; else sjoy.letgo[x2] = /*TRUE*/1;
	  	  
	  }



if (GetKeyboard(SDLK_RIGHT /* 39 */)) sjoy.right = /*TRUE*/1;
if (GetKeyboard(SDLK_LEFT /* 37 */)) sjoy.left = /*TRUE*/1;
if (GetKeyboard(SDLK_DOWN /* 40 */)) sjoy.down = /*TRUE*/1;
if (GetKeyboard(SDLK_UP /* 38 */)) sjoy.up = /*TRUE*/1;

check_keyboard();

}


/* Displays a tile fullscreen, so we can select some squares */
void loadtile(int tileset)
{
/*   DDBLTFX     ddbltfx; */
/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
  
  //feel tile background with a color
  
/*   ddbltfx.dwFillColor = 0; */
/*   ddbltfx.dwSize = sizeof(ddbltfx); */
/*   lpDDSTwo->Blt(NULL,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
  // GFX
  SDL_FillRect(GFX_lpDDSTwo, NULL, 0);
  
  spr[1].seq = 3; spr[1].seq_orig = 3; 
  //  if (mode == 3)
  if (mode == MODE_SCREEN_TILES)
    {
      m3x = spr[1].x; m3y = spr[1].y;
      spr[1].x = m2x; spr[1].y = m2y; mode = 2;
      spr[1].speed = 50;  
    }

  //  if (mode == 4)
  if (mode == MODE_TILE_HARDNESS)
    {
      spr[1].x = m2x; spr[1].y = m2y; mode = 2;
      spr[1].speed = 50;  
    }

/*   lpDDSTwo->BltFast(0, 0, tiles[tileset], &tilerect[tileset], DDBLTFAST_NOCOLORKEY |DDBLTFAST_WAIT); */
  // GFX
  SDL_BlitSurface(GFX_tiles[tileset], NULL, GFX_lpDDSTwo, NULL);
  cur_screen = tileset;
  
  last_mode = tileset;
  
  while(kill_last_sprite());
}


int sp_get( int num)
{
	
int t = 1;
	for (int j=1; j < max_sequences; j++)
	{
     check_frame_status(j, 1);
		
		if (seq[j].frame[1] != 0)
	 {
       if (t == num) 
	   {
		
	
		   return(j);
		
	   }
		   t++;

	 }


	
	}

return(0);

}

/* In sprite picking mode, draw a grid to separate sprites or
   sequences' 50x50 previews */
static void draw_sprite_picker_grid(void)
{
/*   DDBLTFX ddbltfx; */
/*   RECT box_crap; */

/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof(ddbltfx); */

  /* Draw vertical lines */
  for (int x2=1; x2 <= 12; x2++)
    {
/*       ddbltfx.dwFillColor = 120; */
      
/*       box_crap.top = 0; */
/*       box_crap.bottom = 400; */
/*       box_crap.left = (x2*50) -1; */
/*       box_crap.right = box_crap.left+1; */

/*       ddrval = lpDDSTwo->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx); */
      // GFX
      {
	SDL_Rect dst = {12345, 0, 12345, 400};
	dst.x = x2*50 - 1;
	dst.w = 1;
	SDL_FillRect(GFX_lpDDSTwo, &dst, 120);
      }
    }
  
  /* Draw horizontal lines */
  for (int x3=1; x3 <= 8; x3++)
    {
/*       ddbltfx.dwFillColor = 120; */

/*       box_crap.top = (50 * x3)-1; */
/*       box_crap.bottom = box_crap.top +1; */
/*       box_crap.left = 0; */
/*       box_crap.right = 600; */

/*       ddrval = lpDDSTwo->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx); */
      // GFX
      {
	SDL_Rect dst = {0, 12345, 600, 12345};
	dst.y = x3*50 - 1;
	dst.h = 1;
	SDL_FillRect(GFX_lpDDSTwo, &dst, 120);
      }
    }
}

/* Main sprite selection screen - display a page of the sequence
   selector */
void draw15(int num)
{
  int crap;   
/*   DDBLTFX ddbltfx; */
  RECT  crapRec, Rect, box_crap;
  int frame,ddrval;
  int se;
  int dd;	
  
  //get_sp_seq(2);
  
  while(kill_last_sprite());
  
  
/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
/*   ddbltfx.dwFillColor = 0; */
/*   crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
  // GFX
  SDL_FillRect(GFX_lpDDSTwo, NULL, 0);
  
/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
  
  Say("Please wait, loading sprite data into SmartCache system...", 147,160);
  
  flip_it_second();
  
  /* Draw sprites */
  for (int x1=0; x1 <= 11; x1++)
    {
      for (int y1=0; y1 <= 7; y1++)
	{
	  num++;
	  
	  se = sp_get(num);
	  check_seq_status(se);
	  
	  if (se > 0)
	    {
	      frame = 1;
	      
/* 	      Rect.left = x1 * 50; */
/* 	      Rect.top = y1 * 50; */
/* 	      Rect.right = Rect.left + 50; */
/* 	      Rect.bottom = Rect.top + 50; */
	      
/* 	      crapRec = k[seq[se].frame[frame]].box; */
	      
/* 	      dd = lpDDSTwo->Blt(&Rect, k[seq[se].frame[frame]].k, */
/* 				 &crapRec, DDBLT_KEYSRC | DDBLT_DDFX | DDBLT_WAIT, &ddbltfx); */
	      // GFX
	      /* Optimized scaling: no clipping */
	      {
		SDL_Rect src, dst;
		SDL_Surface *scaled;
		double sx, sy;
		dst.x = x1 * 50;
		dst.y = y1 * 50;
		dst.w = 50;
		dst.h = 50;
		sx = 1.0 * dst.w / GFX_k[seq[se].frame[frame]].k->w;
		sy = 1.0 * dst.h / GFX_k[seq[se].frame[frame]].k->h;
		if (sx != 1 || sy != 1)
		  {
		    scaled = zoomSurface(GFX_k[seq[se].frame[frame]].k, sx, sy, SMOOTHING_OFF);
		    SDL_BlitSurface(scaled, NULL, GFX_lpDDSTwo, &dst);
		    SDL_FreeSurface(scaled);
		  }
		else
		  {
		    SDL_BlitSurface(GFX_k[seq[se].frame[frame]].k, &src, GFX_lpDDSTwo, &dst);
		  }
	      }


/* 	      if (dd != DD_OK) Msg("Error with drawing sprite! Seq %d, Spr %d.", se, frame); */
	    }		
	}
    }
  draw_sprite_picker_grid();
}

/* Inner sprite selection screen - display all frames/sprites in a
   sequence */
void draw96(int def)
{
  int crap;   
/*   DDBLTFX ddbltfx; */
  RECT crapRec, Rect, box_crap;
  int frame,ddrval;
  int se;
  int dd;	
  
  //get_sp_seq(2);
  se = sp_seq;
  check_seq_status(se);
  int num = 0;
  while(kill_last_sprite());
  
/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
/*   ddbltfx.dwFillColor = 0; */
/*   crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
  // GFX
  SDL_FillRect(GFX_lpDDSTwo, NULL, 0);  
  
/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
  
  for (int x1=0; x1 <= 11; x1++)
    {
      for (int y1=0; y1 <= 7; y1++)
	{
	  num++;
	  if (seq[se].frame[num] == 0)
	    {
	      //all done displaying
	      goto pass;
	    }
	  //se = sp_get(num);
	  frame = 1;
	  
/* 	  Rect.left = x1 * 50; */
/* 	  Rect.top = y1 * 50; */
/* 	  Rect.right = Rect.left + 50; */
/* 	  Rect.bottom = Rect.top + 50; */
	  
	  if (def > 0 && num == def)
	    {
	      //set default location to sprite they are holding
	      spr[1].x = x1*50;
	      spr[1].y = y1 *50;
	    }
	  
/* 	  crapRec = k[seq[se].frame[num]].box; */
	  
/* 	  dd = lpDDSTwo->Blt(&Rect, k[seq[se].frame[num]].k, */
/* 			     &crapRec, DDBLT_KEYSRC | DDBLT_DDFX | DDBLT_WAIT, &ddbltfx ); */
	  // GFX
	  /* Optimized scaling: no clipping */
	  {
	    SDL_Rect src, dst;
	    SDL_Surface *scaled;
	    double sx, sy;
	    dst.x = x1 * 50;
	    dst.y = y1 * 50;
	    dst.w = 50;
	    dst.h = 50;
	    sx = 1.0 * dst.w / GFX_k[seq[se].frame[num]].k->w;
	    sy = 1.0 * dst.h / GFX_k[seq[se].frame[num]].k->h;
	    if (sx != 1 || sy != 1)
	      {
		scaled = zoomSurface(GFX_k[seq[se].frame[num]].k, sx, sy, SMOOTHING_OFF);
		SDL_BlitSurface(scaled, NULL, GFX_lpDDSTwo, &dst);
		SDL_FreeSurface(scaled);
	      }
	    else
	      {
		SDL_BlitSurface(GFX_k[seq[se].frame[num]].k, &src, GFX_lpDDSTwo, &dst);
	      }
	  }
	  
/* 	  if (dd != DD_OK) Msg("Error with drawing sprite! Seq %d, Spr %d.",se,frame); */
	}
    }
  
 pass:
  draw_sprite_picker_grid();  
}


void sp_add( void )
{
	for (int j =1; j < 100; j++)
	{
		if (pam.sprite[j].active == false)
		{
		  
			last_sprite_added = j;
			//Msg("Adding sprite %d, seq %d, frame %d.",j,sp_seq,sp_frame);
			pam.sprite[j].active = true;
            pam.sprite[j].frame = sp_frame;
            pam.sprite[j].seq = sp_seq;
			pam.sprite[j].x = spr[1].x;
			pam.sprite[j].y = spr[1].y;
			pam.sprite[j].size = spr[1].size;
			pam.sprite[j].type = sp_type;
			pam.sprite[j].brain = sp_brain;
			pam.sprite[j].speed = sp_speed;
			pam.sprite[j].base_walk = sp_base_walk;
			pam.sprite[j].base_idle = sp_base_idle;
			pam.sprite[j].base_attack = sp_base_attack;
			pam.sprite[j].base_hit = sp_base_hit;
			pam.sprite[j].timer = sp_timer;
			pam.sprite[j].que = sp_que;
			pam.sprite[j].hard = sp_hard;
			pam.sprite[j].prop = sp_prop;
			pam.sprite[j].warp_map = sp_warp_map;
			pam.sprite[j].warp_x = sp_warp_x;
			pam.sprite[j].warp_y = sp_warp_y;
			pam.sprite[j].parm_seq = sp_parm_seq;
			strcpy(pam.sprite[j].script, sp_script);
			pam.sprite[j].base_die = sp_base_die;
			pam.sprite[j].gold = sp_gold;
			pam.sprite[j].exp = sp_exp;
			pam.sprite[j].strength = sp_strength;
			pam.sprite[j].defense = sp_defense;
			pam.sprite[j].hitpoints = sp_hitpoints;
			pam.sprite[j].sound = sp_sound;
		    pam.sprite[j].vision = map_vision;	
		    pam.sprite[j].nohit = sp_nohit;		
			pam.sprite[j].touch_damage = sp_touch_damage;		
			CopyRect(&pam.sprite[j].alt , &spr[1].alt);
		    return;
		}



	}


}


void smart_add(void)
{


	
	
	sp_add();

if (sp_screenmatch)
{

if (spr[1].size == 100)
{
//lets see if the sprite crosses into another screen
//use screenmatch technology

	if ((spr[1].x - k[getpic(1)].xoffset) + k[getpic(1)].box.right > 620)
	{
	Msg("need to add it to the right");
		
		//need to add it to the right
		if (map.loc[cur_map+1] > 0)
		{
        	save_map(map.loc[cur_map]);
          Msg("loading right map");
			load_map(map.loc[cur_map+1]);
               int holdx = spr[1].x;
               int holdy = spr[1].y;
			   spr[1].x -= 600;
			sp_add();
			save_map(map.loc[cur_map+1]);
			load_map(map.loc[cur_map]);
           spr[1].x = holdx;
		   spr[1].y = holdy;
		}
	}


	if ((spr[1].x - k[getpic(1)].xoffset) - k[getpic(1)].box.right < 20)
	{
	Msg("need to add it to the right");
		
		//need to add it to the left
		if (map.loc[cur_map-1] > 0)
		{
        	save_map(map.loc[cur_map]);
          Msg("loading right map");
			load_map(map.loc[cur_map-1]);
               int holdx = spr[1].x;
               int holdy = spr[1].y;
			   spr[1].x += 600;
			sp_add();
			save_map(map.loc[cur_map-1]);
			load_map(map.loc[cur_map]);
           spr[1].x = holdx;
		   spr[1].y = holdy;
		}
	}


	if ((spr[1].y - k[getpic(1)].yoffset) + k[getpic(1)].box.bottom > 400)
	{
	Msg("need to add it to the bottom");
		
		//need to add it to the bottom
		if (map.loc[cur_map+32] > 0)
		{
        	save_map(map.loc[cur_map]);
          Msg("loading bottom ");
			load_map(map.loc[cur_map+32]);
               int holdx = spr[1].x;
               int holdy = spr[1].y;
			   spr[1].y -= 400;
			sp_add();
			save_map(map.loc[cur_map+32]);
			load_map(map.loc[cur_map]);
           spr[1].x = holdx;
		   spr[1].y = holdy;
		}
	}


	if ((spr[1].y - k[getpic(1)].yoffset) - k[getpic(1)].box.bottom < 0)
	{
	Msg("need to add it to the top");
		
		//need to add it to the left
		if (map.loc[cur_map-32] > 0)
		{
        	save_map(map.loc[cur_map]);
          Msg("loading top map");
			load_map(map.loc[cur_map-32]);
               int holdx = spr[1].x;
               int holdy = spr[1].y;
			   spr[1].y += 400;
			sp_add();
			save_map(map.loc[cur_map-32]);
			load_map(map.loc[cur_map]);
           spr[1].x = holdx;
		   spr[1].y = holdy;
		}
	}

}

}





}





void blit(int seq1, int frame, SDL_Surface *GFX_lpdest, int tx, int ty)
{
/* RECT math; */

/* 	math = k[seq[seq1].frame[frame]].box; */
/* 	OffsetRect(&math, tx, ty); */
/* 	ddrval = lpdest->Blt(&math, k[seq[seq1].frame[frame]].k, &k[seq[seq1].frame[frame]].box, DDBLT_WAIT, NULL); */
	// GFX
	// No scaling needed here
	{
	  SDL_Rect dst;
	  dst.x = tx;
	  dst.y = ty;
	  SDL_BlitSurface(GFX_k[seq[seq1].frame[frame]].k, NULL, GFX_lpdest, &dst);
	}
}


void check_in(void)

{


	in_huh = in_master;

	
	


	if (in_master == 1)
	  {	
                            in_command = 1; //number
							in_int = &spr[1].size;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",spr[1].size); //set default
				blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Size?",260,175);
	  }





if (in_master == 2)
{

                            in_command = 1; //number
							in_int = &sp_type;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_type); //set default
				blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Type?",260,175);
					   	Say("Type controls the sprites basic type - 0 means it is ornamental only"
							"(cannot walk behind or think) 1 - means normal sprite.  (for a tree or person)"
							,10,10);
						
}


if (in_master == 3)
{

                            in_command = 1; //number
							in_int = &sp_brain;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_brain); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                         Say("New Brain?",260,175);
					     	Say("Brains are a predefined way for this sprite to behave. 0 = No movement, 1 = Dink,"
							" 2 = Dumb Sprite Bouncer, 3 = Duck, 4 = Pig, 6 = repeat, 7 = one loop then kill,"
						    " 9 = moster (all diag), 10 = monster(no diag)"
							,10,10);
                      						

}


if (in_master == 4)
{

                            in_command = 1; //number
							in_int = &sp_speed;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_speed); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Speed?",260,175);
					   	Say("Speed rating allows you to adjust how fast a certain sprite moves.  Works with"
							" most brains."
							,10,10);
						
}


if (in_master == 5)
{

                            in_command = 1; //number
							in_int = &sp_timer;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_timer); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Timing?",260,175);
					   	Say("This is the delay the CPU waits before processing the sprite after each cycle.  "
							"(in thousands of a second - so 33 would mean 30 times a second)"
							,10,10);
						
}


if (in_master == 6)
{

                            in_command = 1; //number
							in_int = &sp_base_walk;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_base_walk); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Base Walk?",260,175);
					   	Say("The base for which the CPU adds 1 through 9 to make the sprite move, depending on"
							" direction.  Must be a multiple of ten. (ie, 20 to look like a duck, 40 to look like a pig)"
							,10,10);
						
}

if (in_master == 7)
{

                            in_command = 1; //number
							in_int = &sp_base_idle;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_base_idle); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Base Idle?",260,175);
					   	Say("Some brains can optionally use extra sprites for their \'idle\' pose."
							
							,10,10);
						
}


if (in_master == 8)
{

                            in_command = 1; //number
							in_int = &sp_que;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_que); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Depth Que?",250,175);
					   	Say("From 1 to 20000, 0 for default.  (defaults to y cord)"
							
							,10,10);
						
}


if (in_master == 9)
{

                            in_command = 1; //number
							in_int = &sp_hard;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_hard); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Hardness?",260,175);
					   	Say("Sets how hardness works.  1 means normal, (monsters) 0 means added to background. (walls, trees)"
							
							,10,10);
						
}

if (in_master == 10)
{

                            in_command = 1; //number
							in_int = &sp_prop;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_prop); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("New Properties?",260,175);
					        Say("Sets special properties for the hardblock.  0 = normal (just hard) 1 = warp."
							,10,10);
						
}

if (in_master == 11)
{

                            in_command = 1; //number
							in_int = &sp_warp_map;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_warp_map); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Warp Map #",260,175);
					        Say("These parms are valid if the hard block property setting is 1.  (warp)"
							
								,10,10);
						
}

if (in_master == 12)
{

                            in_command = 1; //number
							in_int = &sp_warp_x;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_warp_x); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Warp X:",260,175);
					        Say("The X location to warp to.  (20 to 619)"
								,10,10);
						
}

if (in_master == 13)
{

                            in_command = 1; //number
							in_int = &sp_warp_y;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_warp_y); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Warp Y:",260,175);
					        Say("The Y location to warp to.  (0 to 499)"
								,10,10);
						
}

if (in_master == 14)
{

                            in_command = 1; //number
							in_int = &sp_parm_seq;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_parm_seq); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Sequence:",260,175);
					        Say("This parm is used by some brains/settings if set.  A sequence is an animation #."
								,10,10);
						
}
if (in_master == 15)
{

                            in_command = 2; //string
							sprintf(in_default, "%s", sp_script);
                             in_max = 13; 
                              in_string = sp_script;

							blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Script:",260,175);
					        Say("Filename of script this sprite uses."
								,10,10);
						
}


if (in_master == 16)
{

                            in_command = 1; //number
							in_int = &sp_base_die;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_base_die); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Base Death:",260,175);
					        Say("If this sprite dies, this will be used."
								,10,10);
						
}

if (in_master == 17)
{

                            in_command = 1; //number
							in_int = &sp_sound;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_sound); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Sound:",260,175);
					        Say("This sprite will play this sound looped until it dies."
								,10,10);
						
}

if (in_master == 18)
{

                            in_command = 1; //number
							in_int = &sp_hitpoints;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_hitpoints); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Hitpoints:",260,175);
					        Say("How strong is this creature?  (0 = not alive/invincable)"
								,10,10);
						
}

if (in_master == 19)
{

                            in_command = 1; //number
							in_int = &sp_nohit;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_nohit); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Nohit:",260,175);
					        Say("Can this this be punched? 0 if yes.  Either way it will"
							"still check for hit() if a script is attached."
								,10,10);
						
}


if (in_master == 20)
{

                            in_command = 1; //number
							in_int = &sp_touch_damage;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_touch_damage); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Touch Damage:",260,175);
					        Say("If not 0, the hardbox of this sprite will cause this"
							"much damage if touched."
								,10,10);
						
}


if (in_master == 21)
{

                            in_command = 1; //number
							in_int = &sp_base_attack;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_base_attack); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Base Attack:",260,175);
					        Say("If not -1, this monster can attack with this sprite base. (base + dir)"
							
								,10,10);
						
}

if (in_master == 22)
{

                            in_command = 1; //number
							in_int = &sp_defense;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",sp_defense); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Defense:",260,175);
					        Say("This will deducted from any attack."
							
								,10,10);
						
}


if (in_master == 30)
{

                            in_command = 2; //string
							sprintf(in_default, "%s",  buf_path);
                             in_max = 80; 
                              in_string = buf_path;

							blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Path:",260,175);
					        Say("Enter the path with trailing backslash to a dir containing another dink.dat and map.dat file to choose a replacement"
								"for this block. (or enter to choose a replacement from the current map)"
								,10,10);
						
}


if (in_master == 31)
{

                            in_command = 2; //string
							sprintf(in_default, "%s",  pam.script);
                             in_max = 20; 
                              in_string = pam.script;

							blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Script:",260,175);
					        Say("This script will be run before the screen is drawn.  A good place"
								"to change the vision, ect."
								,10,10);
						
}

if (in_master == 32)
{

                            in_command = 1; //number
							in_int = &map_vision;
                            in_max = 10; //max _length
							sprintf(in_default,"%d",map_vision); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Vision:",260,175);
					        Say("Current vision.  If not 0, any sprites you add will ONLY show up"
							" in the game if the vision level matches this one."
								,10,10);
						
}


if (in_master == 33)
{

                            in_command = 1; //number
							
                            in_max = 10; //max _length
							sprintf(in_default,"%d",*in_int); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Music # for screen?:",260,175);
					        Say("Will play #.MID for this screen if nothing else is playing."							
								,10,10);
						
}

if (in_master == 34)
{

                            in_command = 1; //number
							
                            in_max = 10; //max _length
							sprintf(in_default,"%d",*in_int); //set default
						    blit(30,1,GFX_lpDDSBack,250,170);
	                        Say("Screentype?:",260,175);
					        Say("Enter 1 for 'indoors'.  (so it won't show up on the player map)."							
								,10,10);
						
}

old_command = in_master;

in_master = 0;	
in_onflag = true;

	
}

//this changes all none 0 blocks in this tile to num
void change_tile(int tile, int num)
{
	for (int x = 0; x < 50; x++)
	{
		for (int y = 0; y < 50; y++)
		{
			
			
			if (hmap.tile[tile].x[x].y[y] != 0)
			{
			hmap.tile[tile].x[x].y[y] = num;
			}
		}
	}
			

}

 
 
void copy_front_to_two( void)
{
/*   RECT rcRect; */
/*   rcRect.left = 0; */
/*   rcRect.top = 0; */
/*   rcRect.right = x; */
/*   rcRect.bottom = y; */
  
/*   lpDDSTwo->BltFast( 0, 0, lpDDSBack, */
/* 		     &rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
  // GFX
  SDL_BlitSurface(GFX_lpDDSBack, NULL, GFX_lpDDSTwo, NULL);
}


/* Create a 20x20 view of lpDDSTwo for use in the minimap */
void shrink_screen_to_these_cords(int x1, int y1)
{
/*   RECT crapRec, Rect; */
/*   DDBLTFX ddbltfx; */
/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof(ddbltfx); */
  
/*   SetRect(&crapRec, playl, 0, playx, 400); */
/*   SetRect(&Rect, x1, y1, x1+20, y1+20); */
  
/*   lpDDSBack->Blt(&Rect, lpDDSTwo, */
/* 		 &crapRec, DDBLT_DDFX | DDBLT_WAIT, &ddbltfx); */
  
  // GFX
  /* Generic scaling - except no transparency (plus I'm sure scaling
     is necessary) */
  {
    SDL_Rect src, dst;
    SDL_Surface *scaled;
    double sx, sy;
    src.x = playl;
    src.y = 0;
    src.w = playx - playl;
    src.h = 400;
    dst.x = x1;
    dst.y = y1;
    dst.w = 20;
    dst.h = 20;
    sx = 1.0 * dst.w / src.w;
    sy = 1.0 * dst.h / src.h;

    /* TODO: switch to shrinkSurface() */
    scaled = zoomSurface(GFX_lpDDSTwo, sx, sy, SMOOTHING_OFF);
    /* Disable transparency if it wasn't active in the source surface
       (SDL_gfx bug, report submitted to the author) */
    if ((GFX_lpDDSTwo->flags & SDL_SRCCOLORKEY) == 0)
      SDL_SetColorKey(scaled, 0, 0);
    src.x = (int) round(src.x * sx);
    src.y = 0; /* (int) round(src.y * sy); */
    src.w = (int) round(src.w * sx);
    src.h = (int) round(src.h * sy);
    SDL_BlitSurface(scaled, &src, GFX_lpDDSBack, &dst);
    SDL_FreeSurface(scaled);
  }
}


/****************************************************************************
 *
 *      UpdateCursorPosition
 *
 *      Move our private cursor in the requested direction, subject
 *      to clipping, scaling, and all that other stuff.
 *
 *      This does not redraw the cursor.  You need to do that yourself.
 *
 ****************************************************************************/
void UpdateCursorPosition(int x, int y)
{

    /*
     *  Pick up any leftover fuzz from last time.  This is important
     *  when scaling down mouse motions.  Otherwise, the user can
     *  drag to the right extremely slow for the length of the table
     *  and not get anywhere.
     */
    sp_cycle = 0;
    spr[1].x = x;
    spr[1].y = y;
    /* Clip the cursor to our client area */

}


void  Scrawl_OnMouseInput(void)
{
  Uint8 button;
  int x, y;

  mouse1 = false;
  if (mode != 6) return;

  SDL_PumpEvents();
  button = SDL_GetMouseState(&x, &y);
  UpdateCursorPosition(x, y);
  if (button & SDL_BUTTON(1))
	mouse1 = true;
  return;

	
/* 	BOOL fDone = 0; */

/*     while (!fDone) { */
   
/* 		DIDEVICEOBJECTDATA od; */

/*         DWORD dwElements = 1; */

/*         HRESULT hr = g_pMouse->GetDeviceData( */
/*                              sizeof(DIDEVICEOBJECTDATA), &od, */
/*                              &dwElements, 0); */

/*         if (hr == DIERR_INPUTLOST) { */
/*             /\* */
/*              *  We had acquisition, but lost it.  Try to reacquire it. */
/*              * */
/*              *  WARNING!  DO NOT ATTEMPT TO REACQUIRE IF YOU GET */
/*              *  DIERR_NOTACQUIRED!  Otherwise, you're extremely likely */
/*              *  to get caught in an infinite loop:  The acquire will fail, */
/*              *  and you'll get another DIERR_NOTACQUIRED so you'll */
/*              *  try to aquire again, and that'll fail, etc. */
/*              *\/ */
/*         //    PostMessage(hwnd, WM_SYNCACQUIRE, 0, 0L); */
/*          //   break; */
 
/* 		Msg("Have no aquisition!!"); */
/* 		g_pMouse->Acquire(); */

/* 		} */

/*         /\* Unable to read data or no data available *\/ */
/*         if (FAILED(hr) || dwElements == 0) { */
/*            // Msg("No mouse data there."); */
/* 			break; */
/*         } */

        /* Look at the element to see what happened */

/*
        switch (od.dwOfs) {

        // DIMOFS_X: Mouse horizontal motion
        case DIMOFS_X: UpdateCursorPosition(od.dwData, 0); break;


        //DIMOFS_Y: Mouse vertical motion
        case DIMOFS_Y: UpdateCursorPosition(0, od.dwData); break;

case DIDFT_BUTTON: if (od.dwData > 0) mouse1 = true; break;


 


        }
*/

/*         if (od.dwOfs == DIMOFS_X) */
/*         { */
/*          // DIMOFS_X: Mouse horizontal motion */
/*          UpdateCursorPosition(od.dwData, 0); */
/*         } */
/*         else if (od.dwOfs == DIMOFS_Y) */
/*         { */
/*          //DIMOFS_Y: Mouse vertical motion */
/*          UpdateCursorPosition(0, od.dwData); */
/*         } */
/*         else if (od.dwOfs == DIDFT_BUTTON) */
/*         { */
/*          if (od.dwData > 0) mouse1 = true; */
/*         } */

/*     } */

}



void write_moves(void)
{
char crap[100];
char move[100];
char fname[100];

strcpy(fname, sp_script);

if (strlen(sp_script) > 2)
{

} else
{
strcpy(fname, "CRAP");
}
	
	if (sjoy.key[SDLK_KP8 /* 104 */])
 {
EditorSoundPlayEffect( SOUND_JUMP );	
	sprintf(crap, "story\\%s.c",fname);

sprintf(move, "move_stop(&current_sprite, 8, %d, 1)\n",spr[1].y);
	add_text( move, crap);
 }
if (sjoy.key[SDLK_KP4 /* 100 */])
 {
	EditorSoundPlayEffect( SOUND_JUMP );
	sprintf(crap, "story\\%s.c",fname);

	sprintf(move, "move_stop(&current_sprite, 4, %d, 1)\n",spr[1].x);
	add_text( move, crap);
 }

if (sjoy.key[SDLK_KP5 /* 101 */])
 {
	EditorSoundPlayEffect( SOUND_JUMP );
	sprintf(crap, "story\\%s.c",fname);
	
  add_text( "//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n", crap);	
 }


if (sjoy.key[SDLK_KP2 /* 98 */])
 {
	EditorSoundPlayEffect( SOUND_JUMP );
	sprintf(crap, "story\\%s.c",fname);
	sprintf(move, "move_stop(&current_sprite, 2, %d, 1)\n",spr[1].y);
	add_text( move, crap);
 }


if (sjoy.key[SDLK_KP6 /* 102 */])
 {
	EditorSoundPlayEffect( SOUND_JUMP );
	sprintf(crap, "story\\%s.c",fname);
	sprintf(move, "move_stop(&current_sprite, 6, %d, 1)\n",spr[1].x);
	add_text( move, crap);
 }

if (sjoy.key[SDLK_KP7 /* 103 */])
 {
	EditorSoundPlayEffect( SOUND_JUMP );
	sprintf(crap, "story\\%s.c",fname);
	sprintf(move, "move_stop(&current_sprite, 7, %d, 1)\n",spr[1].x);
	add_text( move, crap);
 }

if (sjoy.key[SDLK_KP1 /* 97 */])
 {
	EditorSoundPlayEffect( SOUND_JUMP );
	sprintf(crap, "story\\%s.c",fname);
	sprintf(move, "move_stop(&current_sprite, 1, %d, 1)\n",spr[1].x);
	add_text( move, crap);
 }

if (sjoy.key[SDLK_KP9 /* 105 */])
 {
	EditorSoundPlayEffect( SOUND_JUMP );
	sprintf(crap, "story\\%s.c",fname);
	sprintf(move, "move_stop(&current_sprite, 9, %d, 1)\n",spr[1].x);
	add_text( move, crap);
 }

if (sjoy.key[SDLK_KP3 /* 99 */])
 {
	EditorSoundPlayEffect( SOUND_JUMP );
	sprintf(crap, "story\\%s.c",fname);
	sprintf(move, "move_stop(&current_sprite, 3, %d, 1)\n",spr[1].x);
	add_text( move, crap);
 }


}


void draw_hard_tile(int x1, int y1, int tile)
{
/* HRESULT             ddrval; */
RECT box;	

/*                DDBLTFX     ddbltfx; */
/* ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/* ddbltfx.dwSize = sizeof( ddbltfx); */

	for (int x = 0; x < 50; x++)
		{
			for (int y = 0; y < 50; y++)
			{
				
				
			if (hmap.tile[tile].x[x].y[y] == 1)
			{
			//draw it
			
/*  ddbltfx.dwFillColor = RGB(255,255,255); */
 
/*  SetRect(&box, x1+x+20,y1+y,x1+x+1+20,y1+y+1); */
/*  ddrval = lpDDSBack->Blt(&box ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
 // GFX
 {
   SDL_Rect dst;
   dst.x = x1+x+20;
   dst.y = y1+y;
   dst.w = 1;
   dst.y = 1;
   SDL_FillRect(GFX_lpDDSBack, &dst, SDL_MapRGB(GFX_lpDDSBack->format, 255, 255, 255));
 }
 //if (ddrval != DD_OK) dderror(ddrval);
				
			}

			}
		}

}


/*
 * updateFrame
 * 
 * Decide what needs to be blitted next, wait for flip to complete,
 * then flip the buffers.
 */
void updateFrame(void)
{
  //    static DWORD        lastTickCount[4] = {0,0,0,0};
  //    static int          currentFrame[3] = {0,0,0};
  unsigned long thisTickCount; 
  //  char buffer[20];
  unsigned char state[256]; 
  RECT                rcRect;
  RECT  crapRec, Rect;
  RECT rcRectSrc;  
  RECT rcRectDest, box_crap,box_real;
/*   POINT p; */
  char msg[500];
  char buff[200];
  //	DWORD               delay[4] = {0, 0, 0, 20};
/*   HDC         hdc; */
  int in_crap2;
  int                 holdx;
  //PALETTEENTRY        pe[256];
/*   HRESULT             ddrval; */
  int xx;
/*   DDBLTFX     ddbltfx; */
  /*BOOL*/int kickass,cool;
  /*BOOL*/int bs[max_sprites_at_once];
	
  int rank[max_sprites_at_once];
  int highest_sprite;
  int crap;
  // Decide which frame will be blitted next
  thisTickCount = SDL_GetTicks();
  strcpy(buff,"Nothing");
  state[1] = 0;  
  check_joystick();	
  Scrawl_OnMouseInput();
  kickass = false;
  rcRect.left = 0;
  rcRect.top = 0;
  rcRect.right = x;
  rcRect.bottom = y;

  if (draw_map_tiny != -1)
    {
		
    tiny_again:
      if (draw_map_tiny  == 769)
	{
	  draw_map_tiny = -1;
	  while(kill_last_sprite());
	  //all done
	} else
	{
		
		
	  draw_map_tiny++;
		
	  copy_front_to_two();
		
		
	  if (map.loc[draw_map_tiny] != 0)
	    {
	      //a map exists here
	      load_map(map.loc[draw_map_tiny]);
	      //map loaded, lets display it
	      draw_map();
			
	      goto pass_flip;
	    } else goto tiny_again;
		
		
		
	}
	
    }
	
	
/*   while( 1 ) */
/*     { */
/*       ddrval = lpDDSBack->BltFast( 0, 0, lpDDSTwo, */
/* 				   &rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
      // GFX
      SDL_BlitSurface(GFX_lpDDSTwo, NULL, GFX_lpDDSBack, NULL);

		
/*       if( ddrval == DD_OK ) */
/*         { */
/* 	  break; */
/*         } */
/*       if( ddrval == DDERR_SURFACELOST ) */
/*         { */
/* 	  ddrval = restoreAll(); */
/* 	  if( ddrval != DD_OK ) */
/*             { */
/* 	      return; */
/*             } */
/*         } */
/*       if( ddrval != DDERR_WASSTILLDRAWING ) */
/*         { */
/* 	  return; */
/*         } */
    
/*     } */

 pass_flip:

  memset(&bs,0,sizeof(bs));
		
  int	max_s = 105;
		
  //max_sprites_at_once;
		
  /*for (int r2 = 1; r2 < max_sprites_at_once; r2++)
    {
    if (spr[r2].active) max_s = r2+1;
    }
  */
  int height;

	
									
  spr[1].que = 20000;
  if (mode == MODE_SCREEN_SPRITES) if (   ! ((spr[1].pseq == 10) && (spr[1].pframe == 8)) ) spr[1].que = sp_que;
	
  if (!in_enabled)

    for (int r1 = 1; r1 < max_s+1; r1++)
      {
			
	highest_sprite = 22024; //more than it could ever be
			
	rank[r1] = 0;
			
	for (int h1 = 1; h1 < max_s+1; h1++)
	  {
	    if (spr[h1].active)
	      { 
		if (bs[h1] == /*FALSE*/0)
		  {
		    //Msg( "Ok,  %d is %d", h1,(spr[h1].y + k[spr[h1].pic].yoffset) );
		    if (spr[h1].que != 0) height = spr[h1].que; else height = spr[h1].y;
		    if ( height < highest_sprite )
		      {
			highest_sprite = height;
			rank[r1] = h1;
		      }
						
		  }
					
	      }
				
	  }
	if (rank[r1] != 0)	
	  bs[rank[r1]] = /*TRUE*/1;
      }
		


						

  if (!in_enabled)
	
    for (int jj = 1; jj < max_s; jj++)
      {
	
	int h = rank[jj];
	//Msg("Studying %d.,",h);

	if (spr[h].active)
	  {
			
	    //        Msg("Sprite %d is active.",h);
			
				
	    if (spr[h].brain == 1)
	      {
		if ( (spr[h].seq == 0) | (mode == MODE_TILE_HARDNESS)  )
		  {		
					

							
		    //if (mode == 7)
		    if (mode == MODE_SPRITE_HARDNESS)
		      {

			//editting a sprite, setting hard box and depth dot.
			spr[1].pseq = 1;
			spr[1].pframe = 1;

			if (sjoy.button[1])
			  {
			    //they want out
			    //mode = 5;
			    mode = MODE_SPRITE_PICKER;
			    draw96(0);
			    spr[1].x = m5x;
			    spr[1].y = m5y;
			    spr[1].pseq = 10;
			    spr[1].pframe = 5;
			    spr[1].speed = 50;
			    goto sp_edit_end;

			  }


			if (sjoy.key[SDLK_TAB /* 9 */])
			  {

			    //they hit tab, lets toggle what mode they are in
			    if (sp_mode == 0) sp_mode = 1; else if (sp_mode == 1) sp_mode = 2; else if (sp_mode == 2) sp_mode = 0;


			  }
			if (sjoy.key['s' /* 83 */])
			  {

			    //they hit tab, lets toggle what mode they are in
			    char death[150];
			    char filename[10];
	
			    sprintf(death, "SET_SPRITE_INFO %d %d %d %d %d %d %d %d\n",
				    sp_seq,sp_frame, k[seq[sp_seq].frame[sp_frame]].xoffset,  k[seq[sp_seq].frame[sp_frame]].yoffset,

				    k[seq[sp_seq].frame[sp_frame]].hardbox.left, k[seq[sp_seq].frame[sp_frame]].hardbox.top,
				    k[seq[sp_seq].frame[sp_frame]].hardbox.right,k[seq[sp_seq].frame[sp_frame]].hardbox.bottom);

			    strcpy(filename, "dink.ini");
			    add_text(death,filename);
			    EditorSoundPlayEffect( SOUND_JUMP );
			  }


			int modif = 1;
			if (SDL_GetModState()&KMOD_SHIFT)
			  modif += 9;



			if (sp_mode == 0)
			  {

			    //ok, we are editting depth dot

			    if (SDL_GetModState()&KMOD_CTRL)
			      {
				if (sjoy.key[SDLK_RIGHT /* 39 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].xoffset += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.key[SDLK_LEFT /* 37 */])
				  {
							
				    k[seq[sp_seq].frame[sp_frame]].xoffset -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.key[SDLK_UP /* 38 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].yoffset -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.key[SDLK_DOWN /* 40 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].yoffset += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
			      } else
					
			      {
				if (sjoy.right)
				  {
				    k[seq[sp_seq].frame[sp_frame]].xoffset +=  modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.left)
				  {
				    k[seq[sp_seq].frame[sp_frame]].xoffset -=  modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.up)
				  {
				    k[seq[sp_seq].frame[sp_frame]].yoffset -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.down)
				  {
				    k[seq[sp_seq].frame[sp_frame]].yoffset += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						

			      }

			  }



			if (sp_mode == 2)
			  {

			    //ok, we are top left hardness

			    if (SDL_GetModState()&KMOD_CTRL)
			      {
				if (sjoy.key[SDLK_RIGHT /* 39 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.right += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.key[SDLK_LEFT /* 37 */])
				  {
							
				    k[seq[sp_seq].frame[sp_frame]].hardbox.right -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.key[SDLK_UP /* 38 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.bottom -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.key[SDLK_DOWN /* 40 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.bottom += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
			      } else
					
			      {
				if (sjoy.right)
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.right +=  modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.left)
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.right -=  modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.up)
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.bottom -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.down)
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.bottom += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						

			      }


			    if (k[seq[sp_seq].frame[sp_frame]].hardbox.right <= k[seq[sp_seq].frame[sp_frame]].hardbox.left)
			      k[seq[sp_seq].frame[sp_frame]].hardbox.left = k[seq[sp_seq].frame[sp_frame]].hardbox.right -1;

						
			    if (k[seq[sp_seq].frame[sp_frame]].hardbox.bottom <= k[seq[sp_seq].frame[sp_frame]].hardbox.top)
			      k[seq[sp_seq].frame[sp_frame]].hardbox.top = k[seq[sp_seq].frame[sp_frame]].hardbox.bottom -1;


			  }


			if (sp_mode == 1)
			  {

			    //ok, we are top left hardness

			    if (SDL_GetModState()&KMOD_CTRL)
			      {
				if (sjoy.key[SDLK_RIGHT /* 39 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.left += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.key[SDLK_LEFT /* 37 */])
				  {
							
				    k[seq[sp_seq].frame[sp_frame]].hardbox.left -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.key[SDLK_UP /* 38 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.top -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.key[SDLK_DOWN /* 40 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.top += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
			      } else
					
			      {
				if (sjoy.right)
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.left +=  modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.left)
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.left -=  modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.up)
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.top -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						
				if (sjoy.down)
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.top += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
						

			      }


			    if (k[seq[sp_seq].frame[sp_frame]].hardbox.left >= k[seq[sp_seq].frame[sp_frame]].hardbox.right)
			      k[seq[sp_seq].frame[sp_frame]].hardbox.right = k[seq[sp_seq].frame[sp_frame]].hardbox.left +1;

						
			    if (k[seq[sp_seq].frame[sp_frame]].hardbox.top >= k[seq[sp_seq].frame[sp_frame]].hardbox.bottom)
			      k[seq[sp_seq].frame[sp_frame]].hardbox.bottom = k[seq[sp_seq].frame[sp_frame]].hardbox.bottom +1;




			  }




			if (k[seq[sp_seq].frame[sp_frame]].hardbox.top > 200 ) k[seq[sp_seq].frame[sp_frame]].hardbox.top = 200;
			if (k[seq[sp_seq].frame[sp_frame]].hardbox.top <-200 ) k[seq[sp_seq].frame[sp_frame]].hardbox.top = -200;

			if (k[seq[sp_seq].frame[sp_frame]].hardbox.left > 316) k[seq[sp_seq].frame[sp_frame]].hardbox.left = 316;
			if (k[seq[sp_seq].frame[sp_frame]].hardbox.left < -320) k[seq[sp_seq].frame[sp_frame]].hardbox.left = -320;


			if (k[seq[sp_seq].frame[sp_frame]].hardbox.bottom > 200) k[seq[sp_seq].frame[sp_frame]].hardbox.bottom = 200;
			if (k[seq[sp_seq].frame[sp_frame]].hardbox.bottom <-200 ) k[seq[sp_seq].frame[sp_frame]].hardbox.bottom = -200;

			if (k[seq[sp_seq].frame[sp_frame]].hardbox.right > 316) k[seq[sp_seq].frame[sp_frame]].hardbox.right = 316;
			if (k[seq[sp_seq].frame[sp_frame]].hardbox.right < -320) k[seq[sp_seq].frame[sp_frame]].hardbox.right = -320;



			goto b1end;

		      }


		    if (mode == MODE_SCREEN_SPRITES)
		      {
			// place sprite
			if ( (sjoy.key['v']) )
			  {
			    in_master = 32; // Set screen vision?
			  }

			int modif = 0;
			if (SDL_GetModState()&KMOD_SHIFT) modif = 9;

			if (sjoy.key['m'])
			  {
			    if (sp_screenmatch) sp_screenmatch = false; else sp_screenmatch = true;
			  }
               

			if (SDL_GetModState()&KMOD_ALT) // alt
			  {
			    //alt is held down 87
			    if (sjoy.key['w' /* 87 */])
			      {
				//pressed W
				if (  ((spr[1].pseq == 10) & (spr[1].pframe == 8)  ) )
						
				  {
                                    //a sprite is not chosen
				    hold_warp_map = cur_map;
				    hold_warp_x = spr[1].x;
				    hold_warp_y= spr[1].y;
				    EditorSoundPlayEffect( SOUND_JUMP );


				  } else
				  {
				    sp_warp_map = hold_warp_map ;
				    sp_warp_x = hold_warp_x;
				    sp_warp_y = hold_warp_y;
				    EditorSoundPlayEffect( SOUND_JUMP );
									   
				  }


			      }
							   
			  }

						
			if (    !((spr[1].pseq == 10) & (spr[1].pframe == 8))  )
			  {
			    //they are wheeling around a sprite
			    if (spr[1].x > 1500) spr[1].x = 1500;
			    if (spr[1].y > 1500) spr[1].y = 1500;

			    if (spr[1].size > 1500) spr[1].size = 1500;
	
			    /* if (GetKeyboard(VK_OEM_4 /\* 219 *\/)) // '[' for US */
			    if (GetKeyboard(SDLK_LEFTBRACKET))
			      spr[1].size -= 1+modif;
			    /* if (GetKeyboard(VK_OEM_6 /\* 221 *\/)) // ']' for US */
			    if (GetKeyboard(SDLK_RIGHTBRACKET))
			      spr[1].size += 1+modif;
						  	
						
			    if (SDL_GetModState()&KMOD_SHIFT)
			      {
				//shift is being held down
				if (getkey('1'))  in_master = 11;
				if (getkey('2'))  in_master = 12;
				if (getkey('3'))  in_master = 13;
				if (getkey('4'))  in_master = 14;
				if (getkey('5'))  in_master = 15;
					
				if (getkey('6'))  in_master = 16;
				if (getkey('7'))  in_master = 17;
				if (getkey('8'))  in_master = 18;
				if (getkey('9'))  in_master = 19;
						
						
                        
			      } else
			      if (SDL_GetModState()&KMOD_ALT)
				{
				  //alt is being held down
				  if (getkey('1' /* 49 */))  in_master = 20;
				  if (getkey('2'))  in_master = 21;
				  if (getkey('3'))  in_master = 22; 
				  /*(getkey('4' /\* 52 *\/))  in_master = 14;
				    if (getkey(53))  in_master = 15;
					
				    if (getkey(54))  in_master = 16;
				    if (getkey(55))  in_master = 17;
				    if (getkey(56))  in_master = 18;
				    if (getkey(57))  in_master = 19;
					
				  */
				} else
					
							
							
				{
				  //shift is not being held down
					
				  if (getkey('0')) 
				    {
				      in_master = 10;
						
				    }

								
				  if (getkey('1')) 
				    {
				      in_master = 1;
						
				    }

				  if (getkey('2')) 
				    {
				      in_master = 2;
							
				    }

				  if (getkey('3')) 
				    {
				      in_master = 3;
							
				    }
				  if (getkey('4')) 
				    {
				      in_master = 4;
							
				    }
				  if (getkey('5')) in_master = 5;
				  if (getkey('6')) in_master = 6;
				  if (getkey('7')) in_master = 7;
				  if (getkey('8')) in_master = 8;
				  if (getkey('9')) in_master = 9;
					 
				}


			    if (sjoy.key['s' /* 83 */])
			      {
				smart_add();
								
				draw_map();
			      }
							
			    if ( (sjoy.button[2]) | (mouse1) )
			      {
				smart_add();
				draw_map();
				spr[1].pseq = 10;
				spr[1].pframe = 8;
				spr[1].size = 100;
				SetRect(&spr[1].alt,0,0,0,0);	
		
			      }

			    if (sjoy.key[SDLK_DELETE /* 46 */])
			      {
								
				spr[1].pseq = 10;
				spr[1].pframe = 8;
				spr[1].size = 100;
				SetRect(&spr[1].alt,0,0,0,0);	
					
			      }

			  } else
			  {
			    //no sprite is currently selected
					

			    write_moves();




			    int max_spr = 0;
			    for (int jj=1; jj < 100; jj++)
			      {
				if ( pam.sprite[jj].active) if (pam.sprite[jj].vision == map_vision) max_spr++;
			      }
      
	
			    if (max_spr > 0)
			      {

				/* if (sjoy.key[VK_OEM_4 /\* 219 *\/]) // '[' for US */
				if (GetKeyboard(SDLK_LEFTBRACKET))
				  {
				    sp_cycle--;
				 
				    if (sp_cycle < 1) sp_cycle = max_spr;
				  }

				/* if (sjoy.key[VK_OEM_6 /\* 221 *\/]) // ']' for US */
				if (GetKeyboard(SDLK_RIGHTBRACKET))
				  {
				    sp_cycle++;
				 
				    if (sp_cycle > max_spr) sp_cycle = 1;
				 
				  }



			      }


			 
			    //Msg("Cycle is %d", sp_cycle);
			    int realpic = 0;

			    if (sp_cycle > 0)
			      { 
				//lets draw a frame around the sprite we want
				int dumbpic = 0;
				realpic = 0;
				for (int jh=1; dumbpic != sp_cycle; jh++)
				  {
				    if (pam.sprite[jh].active)  if ( pam.sprite[jh].vision == map_vision)
								  {
								    dumbpic++;
								    realpic = jh;
								  }
				    if (jh == 99) goto fail;
		
				  }

				last_sprite_added = realpic;	

    
/* 				ddbltfx.dwSize = sizeof(ddbltfx); */
/* 				ddbltfx.dwFillColor = 235; */

				int	sprite = add_sprite_dumb(pam.sprite[realpic].x,pam.sprite[realpic].y,0,
								 pam.sprite[realpic].seq, pam.sprite[realpic].frame,
								 pam.sprite[realpic].size);
				CopyRect(&spr[sprite].alt , &pam.sprite[realpic].alt);
				get_box(sprite, &box_crap, &box_real);
                            



				get_box(sprite, &box_crap, &box_real);
				box_crap.bottom = box_crap.top + 5;
/* 				ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
				// GFX
				{
				  SDL_Rect dst;
				  dst.x = box_crap.left;
				  dst.y = box_crap.top;
				  dst.w = box_crap.right - box_crap.left;
				  dst.h = 5;
				  SDL_FillRect(GFX_lpDDSBack, &dst, 235);
				}

				get_box(sprite, &box_crap, &box_real);
				box_crap.right = box_crap.left + 5;
/* 				ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
				// GFX
				{
				  SDL_Rect dst;
				  dst.x = box_crap.left;
				  dst.y = box_crap.top;
				  dst.w = 5;
				  dst.h = box_crap.bottom - box_crap.top;
				  SDL_FillRect(GFX_lpDDSBack, &dst, 235);
				}

				get_box(sprite, &box_crap, &box_real);
				box_crap.left = box_crap.right - 5;
/* 				ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
				// GFX
				{
				  SDL_Rect dst;
				  dst.x = box_crap.right - 5;
				  dst.y = box_crap.top;
				  dst.w = 5;
				  dst.h = box_crap.bottom - box_crap.top;
				  SDL_FillRect(GFX_lpDDSBack, &dst, 235);
				}

				get_box(sprite, &box_crap, &box_real);
				box_crap.top = box_crap.bottom - 5;
/* 				ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
				// GFX
				{
				  SDL_Rect dst;
				  dst.x = box_crap.left;
				  dst.y = box_crap.bottom - 5;
				  dst.w = box_crap.right - box_crap.left;
				  dst.h = 5;
				  SDL_FillRect(GFX_lpDDSBack, &dst, 235);
				}

				//	if (ddrval != DD_OK) dderror(ddrval);

				spr[sprite].active = false;
				
			      }

			  fail:

							
			    if ( (sjoy.button[2]) | (mouse1))
			      {
				//pick up a sprite already placed by hitting enter
							

				for (int uu = 100; uu > 0; uu--)
				  {
				    if ( pam.sprite[uu].active) if ( ( pam.sprite[uu].vision == 0) || (pam.sprite[uu].vision == map_vision))
								  {
							
								    int	sprite = add_sprite_dumb(pam.sprite[uu].x,pam.sprite[uu].y,0,
												 pam.sprite[uu].seq, pam.sprite[uu].frame,
												 pam.sprite[uu].size);
								    CopyRect(&spr[sprite].alt , &pam.sprite[uu].alt);			                 
								    get_box(sprite, &box_crap, &box_real);
								    if (realpic > 0) goto spwarp;
								    //Msg("Got sprite %d's info. X%d Y %d.",uu,box_crap.left,box_crap.right);

								    if (inside_box(spr[1].x,spr[1].y,box_crap)) 
									
								      {
									//this is the sprite they want to edit, lets convert them into it
									//						Msg("FOUND SPRITE!  It's %d, huh.",uu); 
								
									if ( 4 > 9) 
									  {
									  spwarp:
									    Msg("Ah yeah, using %d!",realpic);
									    uu = realpic;
									  }
									
									
									spr[1].x = pam.sprite[uu].x; 
									spr[1].y = pam.sprite[uu].y; 
									spr[1].size = pam.sprite[uu].size; 
									sp_type = pam.sprite[uu].type;
									sp_brain = pam.sprite[uu].brain;
									sp_speed = pam.sprite[uu].speed;
									sp_base_walk = pam.sprite[uu].base_walk;
									sp_base_idle = pam.sprite[uu].base_idle;
									sp_base_attack = pam.sprite[uu].base_attack;
									sp_base_hit = pam.sprite[uu].base_hit;
									sp_timer = pam.sprite[uu].timer;
									sp_que = pam.sprite[uu].que;
									sp_seq = pam.sprite[uu].seq;
									sp_hard = pam.sprite[uu].hard;
									CopyRect(&spr[1].alt , &pam.sprite[uu].alt);
									sp_frame = pam.sprite[uu].frame;
									spr[1].pseq = pam.sprite[uu].seq;
									spr[1].pframe = pam.sprite[uu].frame;
								
									sp_prop = pam.sprite[uu].prop;
								
									sp_warp_map = pam.sprite[uu].warp_map;
									sp_warp_x = pam.sprite[uu].warp_x;
									sp_warp_y = pam.sprite[uu].warp_y;
									sp_parm_seq = pam.sprite[uu].parm_seq;
									strcpy(sp_script, pam.sprite[uu].script);

									sp_base_die = pam.sprite[uu].base_die;
									sp_gold = pam.sprite[uu].gold;
									sp_hitpoints = pam.sprite[uu].hitpoints;	

									sp_exp = pam.sprite[uu].exp;	
									sp_nohit = pam.sprite[uu].nohit;	
									sp_touch_damage = pam.sprite[uu].touch_damage;	
									sp_defense = pam.sprite[uu].defense;	
									sp_strength = pam.sprite[uu].strength;	
									sp_sound = pam.sprite[uu].sound;	
							
									pam.sprite[uu].active = false; //erase sprite
									draw_map();
									spr[sprite].active = false;
									break;
								      }
								    spr[sprite].active = false;
				
								  }
				  }
							
							
			      }
					
							
			    if ((SDL_GetModState()&KMOD_ALT) & (GetKeyboard(SDLK_DELETE /* 46 */)))
			      {
				for (int ll = 1; ll < 100; ll++)
				  { 
				    pam.sprite[ll].active = false;
				  }
				draw_map();
				SetRect(&spr[h].alt,0,0,0,0);
			      }
			  }
						
			if ( (sjoy.realkey['z']) | (sjoy.realkey['x']) )
			  {
			    if (  (spr[h].alt.right == 0) & (spr[h].alt.left == 0) & (spr[h].alt.top == 0) &
				  (spr[h].alt.bottom == 0) ) CopyRect(&spr[h].alt, &k[getpic(h)].box);
			  }

			/* Trim a sprite? */
                        if (sjoy.realkey['z'])
			  {
             
			    if (sjoy.key[SDLK_RIGHT /* 39 */])
			      {
				spr[h].alt.left += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
						
			    if (sjoy.key[SDLK_LEFT /* 37 */])
			      {
				spr[h].alt.left -= spr[h].speed +modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    if (sjoy.key[SDLK_DOWN /* 40 */])
			      {
				spr[h].alt.top += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    
			    if (sjoy.key[SDLK_UP /* 38 */])
			      {
				spr[h].alt.top -= spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
							

			    if (spr[h].alt.top < 0) spr[h].alt.top = 0;
			    if (spr[h].alt.left < 0) spr[h].alt.left = 0;
			    goto b1end;
			  }
						
						
						
			if (sjoy.realkey['x'])
			  {
             
			    if (sjoy.key[SDLK_RIGHT /* 39 */])
			      {
				spr[h].alt.right += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
						
			    if (sjoy.key[SDLK_LEFT /* 37 */])
			      {
						
				spr[h].alt.right -= spr[h].speed +modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    if (sjoy.key[SDLK_DOWN /* 40 */])
			      {
				spr[h].alt.bottom += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
						
			    if (sjoy.key[SDLK_UP /* 38 */])
			      {
				spr[h].alt.bottom -= spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
						
				//	Msg("Bottom is %d..",spr[h].alt.bottom);
						
			      }
			    if (spr[h].alt.bottom > k[getpic(h)].box.bottom) spr[h].alt.bottom = k[getpic(h)].box.bottom;
			    if (spr[h].alt.right > k[getpic(h)].box.right) spr[h].alt.right = k[getpic(h)].box.right;
                        
			    goto b1end;

			  }


			if (spr[1].size < 1) spr[1].size = 1;
						
						
			if (SDL_GetModState()&KMOD_CTRL)
			  {
			    if (sjoy.key[SDLK_RIGHT /* 39 */])
			      {
						
				sp_cycle = 0;
							
				spr[h].x += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    
			    if (sjoy.key[SDLK_LEFT /* 37 */])
			      {
				spr[h].x -= spr[h].speed +modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;
			      }
			    if (sjoy.key[SDLK_UP /* 38 */])
			      {
				spr[h].y -= spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;
			      }
			    
			    if (sjoy.key[SDLK_DOWN /* 40 */])
			      {
				spr[h].y += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;
			      }
						
			  } else
					
			  {
			    if (sjoy.right)
			      {
				spr[h].x += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;	
			      }
						
			    if (sjoy.left)
			      {
				spr[h].x -= spr[h].speed +modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;
			      }
			    if (sjoy.up)
			      {
				spr[h].y -= spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;
			      }
						
			    if (sjoy.down)
			      {
				spr[h].y += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;

			      }
						

			  }





			if (  (sjoy.button[1]) ) 
			  {
			    //return to edit mode or drop sprite, depending..
			    if (((spr[1].pseq == 10) & (spr[1].pframe == 8))  )
						
			      {
								
				SetRect(&spr[1].alt,0,0,0,0);	
		
				spr[1].size = 100;
				mode = MODE_SCREEN_TILES;
				spr[1].x = m4x;
				spr[1].y = m4y;
				spr[1].seq = 3;
				spr[1].speed = 50;
			      } else
			      {
				smart_add();
				draw_map();
				SetRect(&spr[1].alt,0,0,0,0);	
		
				spr[1].pseq = 10;
				spr[1].pframe = 8;
				spr[1].size = 100;
								
			      }
			  }
						
						
			if (sjoy.key['e'])
			  {
			    //they hit E, go to sprite picker	
			    SetRect(&spr[1].alt,0,0,0,0);	
							
			    spr[1].size = 100;		
			    //mode = 5;
			    mode = MODE_SPRITE_PICKER;
			    m6x = spr[h].x;
			    m6y = spr[h].y;
			    spr[h].x = m5x;
			    spr[h].y = m5y;
							
			    spr[1].seq = 3;
			    spr[1].speed = 50;
			    if (sp_seq == 0) draw15(sp_picker); else draw96(sp_frame);
			    goto sp_edit_end;
							
			  }
			if (sjoy.button[5])
			  {
			    //they hit tab, return to tile edit mode
			    if (    !((spr[1].pseq == 10) & (spr[1].pframe == 8))  )
											
			      {
				smart_add();
				SetRect(&spr[1].alt,0,0,0,0);	
		
				draw_map();
			      }
			    spr[1].size = 100;		
			    mode = MODE_SCREEN_TILES;
			    spr[h].x = m4x;
			    spr[h].y = m4y;
							
			    spr[1].seq = 3;
			    spr[1].speed = 50;
			    //	if (sp_seq == 0) draw15(); else draw96();
			    goto sp_edit_end;
							
			  }
						
						
			goto b1end;
						
		      }
					
					
		    if ( (mode == MODE_SCREEN_TILES) & (sjoy.button[5]))
		      {

			//they chose sprite picker mode
			//while (kill_last_sprite()); 


			mode = MODE_SCREEN_SPRITES;
						
			spr[1].pseq = 10;
			spr[1].pframe = 8;
								
			spr[1].speed = 1;
			selx = 1;
			sely = 1;
			m4x = spr[h].x;
			m4y = spr[h].y;
			//spr[h].x = m5x;
			//spr[h].y = m5y;
						
			//if (sp_seq == 0)
			//	draw15(); else draw96();
						
		      } else
						
						
						
						
						
						
		      if (mode == MODE_SPRITE_PICKER)
			{
			  //picking a sprite	
			  if (sp_seq != 0)
			    {
			      //they are in select sprite phase 2
							
			      if (sjoy.key['e'])
				{
				  //they want to 'edit' the sprite
				  mode = MODE_SPRITE_HARDNESS;
				  m5x = spr[h].x;
				  m5y = spr[h].y;
									
				  //lets blank the screen
/* 				  ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/* 				  ddbltfx.dwSize = sizeof( ddbltfx); */
/* 				  ddbltfx.dwFillColor = 255; */
/* 				  crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
				  // GFX
				  SDL_FillRect(GFX_lpDDSTwo, NULL, 255);

				  holdx = (spr[1].x / 50);
				  int holdy = (spr[1].y / 50)+1;
				  holdx = holdx * 8;
				  if (seq[sp_seq].frame[holdx + holdy] == 0) goto sp_fin;
									
				  int spman = add_sprite_dumb(320,200 , 0,sp_seq,holdx + holdy,100 );

				  sp_frame = holdx + holdy;
				  spr[1].pseq = 10;
				  spr[1].pframe = 8;

									
				  spr[1].speed = 1;
				  goto sp_edit_end;
									
				}

			      if (  (sjoy.button[1]) ) 
				{
									
									
				  //returning to main sprite picker mode
				  sp_seq = 0;
									
				  draw15(sp_picker);
				  spr[h].x = m5ax;
				  spr[h].y = m5ay;
									
				  goto sp_edit_end;
				}
								
			      if (sjoy.button[5])
				{
				  //leave to screen editor
				sp_fin:
				  m5x = spr[h].x;
				  m5y = spr[h].y;
								
				  draw_map();
				  spr[h].x = m6x;
				  spr[h].y = m6y;

				  spr[1].pseq = 10;
				  spr[1].pframe = 8;

				  spr[h].speed = 1;
				  mode = MODE_SCREEN_SPRITES;
				  goto sp_edit_end;
								
				}
								
			      if (sjoy.button[2])
				{
									
				  // go to mode 6, sprite placement
				  m5x = spr[h].x;
				  m5y = spr[h].y;
									
								   
				  holdx = (spr[1].x / 50);
				  int holdy = (spr[1].y / 50)+1;
				  holdx = holdx * 8;
				  if (seq[sp_seq].frame[holdx + holdy] == 0) goto sp_fin;
				  spr[1].pseq = sp_seq;
				  spr[1].pframe = holdx + holdy;
				  sp_frame = holdx + holdy;
				  draw_map();
				  spr[h].x = m6x;
				  spr[h].y = m6y;
				  mode = MODE_SCREEN_SPRITES;
				  spr[h].speed = 1;
				  goto sp_edit_end;
									
				}
								
								
			      goto sp_edit_end;
			    }
							
							
			  if (   (sjoy.button[5]) | (sjoy.button[1]) ) 
			    {
								
			      //exit to main editor
			      /*m5x = spr[h].x;
				m5y = spr[h].y;
				draw_map();
				spr[h].x = m4x;
				spr[h].y = m4y;
				mode = 3;
				goto b1end;
			      */
			      m5x = spr[h].x;
			      m5y = spr[h].y;
								
			      draw_map();
			      spr[h].x = m6x;
			      spr[h].y = m6y;
			      spr[h].pseq = 10;
			      spr[h].pframe = 8;
								
			      spr[h].speed = 1;
			      mode = MODE_SCREEN_SPRITES;
			      goto b1end;
			      //goto sp_edit_end;
								
			    }
							
			  /* if (sjoy.key[VK_OEM_4 /\* 219 *\/]) // '[' for US */
			  if (GetKeyboard(SDLK_LEFTBRACKET))
			    {
			      if (sp_picker > 95) sp_picker -= 96; else
				{
				  sp_picker = (4 * 96);
				}
			      draw15(sp_picker);
			    }
			    /* if (sjoy.key[VK_OEM_6 /\* 221 *\/]) // ']' for US */
			  if (GetKeyboard(SDLK_RIGHTBRACKET))
			    {
			      if (sp_picker < 400) sp_picker += 96;
			      draw15(sp_picker);
			    }
							
			  if (sjoy.button[2])
			    {
								
			      //they chose a catagory, switch to phase 2, it will know cuz sp_seq > 0.
			      holdx = (spr[1].x / 50);
			      int holdy = (spr[1].y / 50)+1;
			      holdx = holdx * 8;
			      m5ax = spr[1].x;
			      m5ay = spr[1].y;
			      spr[1].x = 0;
			      spr[1].y = 0;
			      sp_seq = sp_get(sp_picker+ (holdx + holdy));
			      //	Msg("Sp_seq is %d",sp_seq);
			      draw96(0);
								
			    }
							
							
			}
		  sp_edit_end:
						

  
						
		    if (mode == MODE_SCREEN_TILES) draw_current();
						
						
		    if (mode == MODE_DIALOG)
		      {
							
			spr[h].seq = 2;
			spr[h].seq_orig = 2;
			draw_used();	
			spr[1].que = 20000;
			mode = MODE_MAP_PICKER;
			spr[2].active = /*FALSE*/0;
			spr[3].active = /*FALSE*/0;
			spr[4].active = /*FALSE*/0;
		      }
						
		    if (mode == MODE_DIALOG) goto b1end;			
						
						
						
						
		    //mode equals 4, they are in hardness edit mode, so lets do this thang
						
		    if (mode == MODE_TILE_HARDNESS)
		      {
							
							
			if (spr[h].seq == 0)
			  {
								
			    if ((SDL_GetModState()&KMOD_SHIFT) && (GetKeyboard(SDLK_RIGHT /* 39 */)) )
			      {
				spr[h].seq = 4;
				spr[h].frame = 1;
				if (selx < 8) selx++;
				goto b1fun;
			      }
								
			    if ((SDL_GetModState()&KMOD_SHIFT) && (GetKeyboard(SDLK_LEFT /* 37 */)) )
			      {
				spr[h].seq = 4;
				spr[h].frame = 1;
				if (selx > 1) selx--;
				goto b1fun;
			      }
								
			    if ((SDL_GetModState()&KMOD_SHIFT) && (GetKeyboard(SDLK_UP /* 38 */)) )
			      {
				spr[h].seq = 4;
				spr[h].frame = 1;
				if (sely > 1) sely--;
				goto b1fun;
			      }
								
			    if ((SDL_GetModState()&KMOD_SHIFT) && (GetKeyboard(SDLK_DOWN /* 40 */)) )
			      {
				spr[h].seq = 4;
				spr[h].frame = 1;
				if (sely <  8) sely++;
				goto b1fun;
			      }
								
								
			    if (sjoy.right)
			      {
				spr[h].x += 9;
				spr[h].seq = 4;
				spr[h].frame = 1;
				EditorSoundPlayEffect( SOUND_STOP );
									
			      }
			    if (sjoy.left)
			      {
				spr[h].x -= 9;
				spr[h].seq = 4;
				spr[h].frame = 1;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    if (sjoy.up)
			      {
				spr[h].y -= 9;
				spr[h].seq = 4;
				spr[h].frame = 1;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    if (sjoy.down)
			      {
				spr[h].y += 9;
				spr[h].seq = 4;
				spr[h].frame = 1;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
								
			  }
							
							
		      b1fun:
							
			//make sure they didn't go past the boundrys
			if (mode != 1)
			  {							
							
			    if (spr[h].x + (9 * (selx -1))> 95+441) spr[h].x = (95+441) - (9 * (selx-1));
			    if (spr[h].x < 95) spr[h].x = 95;
			    if (spr[h].y < 0) spr[h].y = 0;
			    if (spr[h].y + (9 * (sely -1))> 441) spr[h].y = 441 - (9 * (sely-1));	
			  }							
							
			//change a piece to hard
			if (  GetKeyboard('z'))
			  { 
								
			    for (int y = 0; y < sely; y++)
			      {
				for (int x = 0; x < selx; x++)
				  {
				    hmap.tile[hard_tile].x[((spr[h].x) + (x*9) - 95) / 9].y[(spr[h].y + (y *9)) / 9] = 1;
										
				  }
			      }
			  }
							
							
			//change a piece to soft
			if (GetKeyboard('x'))
			  { 
								
			    for (int y = 0; y < sely; y++)
			      {
				for (int x = 0; x < selx; x++)
				  {
				    hmap.tile[hard_tile].x[((spr[h].x) + (x*9) - 95) / 9].y[(spr[h].y + (y *9)) / 9] = 0;
										
				  }
			      }
								
								
			  }
							
					
			if ( (GetKeyboard('a')) & (SDL_GetModState()&KMOD_ALT ) )
			  {
			    //change ALL to 'low hard'
			    change_tile(hard_tile, 2);
			    Msg("Changing whole tile to 2");
								 
			    return;
			  }

			if ( (GetKeyboard('s')) & (SDL_GetModState()&KMOD_ALT ) )
			  {
			    //change ALL to 'low hard'
			    change_tile(hard_tile, 3);
			    Msg("Chaning whole tile to 3");
								 
			    return;
			  }
			if ( (GetKeyboard('x')) & (SDL_GetModState()&KMOD_ALT ) )
			  {
			    //change ALL to 'low hard'
			    change_tile(hard_tile, 1);
			    Msg("Changing whole tile to 1");
			    
			    return;
			  }


                        if (GetKeyboard('a'))
			  { 
								
			    for (int y = 0; y < sely; y++)
			      {
				for (int x = 0; x < selx; x++)
				  {
				    hmap.tile[hard_tile].x[((spr[h].x) + (x*9) - 95) / 9].y[(spr[h].y + (y *9)) / 9] = 2;
										
				  }
			      }
					
			  }
                        if (GetKeyboard('s'))
			  { 
								
			    for (int y = 0; y < sely; y++)
			      {
				for (int x = 0; x < selx; x++)
				  {
				    hmap.tile[hard_tile].x[((spr[h].x) + (x*9) - 95) / 9].y[(spr[h].y + (y *9)) / 9] = 3;
										
				  }
			      }
					
			  }
							

			//update frame with current hard blocks, slow
							
			draw_hard();
							
			if (   (sjoy.button[1])  | (sjoy.button[2]) )
			  {
			    //quit hardness edit		
								
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
							
			    if (last_modereal == 8)
			      {
				//return to alt hardness editor
				draw_map();
				last_modereal = 0;	
				spr[h].x = m4x;
				spr[h].y = m4y;
								
				selx = 1;
				sely = 1;
									
				mode = MODE_UNKNOWN;
				return;
				//goto skip_draw;
									
									
			      }
								
			    if (last_mode > 0) { 
									
			      loadtile(last_mode);
			      selx = 1;
			      sely = 1;
			      goto b1end;
			    }
			    fill_whole_hard();
							
			    draw_map();
			    spr[h].x = m4x;
			    spr[h].y = m4y;
			    mode = MODE_SCREEN_TILES;
			    selx = 1;
			    sely = 1;
			  }
							
			goto b1end;
		      }


		    //THEY WANT TO EDIT HARDNESS

		    if ( (sjoy.key['b']) )
		      {
			in_master = 31;
	

		      }

		    if ( (sjoy.key['v']) )
		      {
			in_master = 32;
		      }



		    if (    ( (mode == MODE_SCREEN_TILES) & (sjoy.button[2]) )  | (mode == MODE_TILE_PICKER) & (GetKeyboard(SDLK_SPACE)))

		      {
	
			if (mode == MODE_SCREEN_TILES) cur_tile = pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].num;
	
			if (mode == MODE_TILE_PICKER)
		
			  {
			    cur_tile = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50); 
			    cur_tile += (cur_screen * 128) - 128;
		
		
			  }
			while(kill_last_sprite());
			draw_current();
	
			if (cur_tile > 0)
		
			  {
		
			    if (hmap.index[cur_tile] == 0)
			      {
			
			
				for (int j = 1; j < 799; j++)
				  {
				    if (hmap.tile[j].used == /*FALSE*/0)
				      {
					
					hmap.index[cur_tile] = j;
					hmap.tile[j].used = /*TRUE*/1;
				    	hard_tile = j;
					goto tilesel;
					
				      }
		
				  }
	
		
			      } else hard_tile = hmap.index[cur_tile];
		
			  tilesel:
			    cool = cur_tile / 128;
			    xx = cur_tile - (cool * 128);
/* 			    Rect.left = (xx * 50- (xx / 12) * 600); */
/* 			    Rect.top = (xx / 12) * 50; */
/* 			    Rect.right = Rect.left + 50; */
/* 			    Rect.bottom = Rect.top + 50; */
		
/* 			    crapRec.top = 0; */
/* 			    crapRec.left = 95; */
/* 			    crapRec.bottom = 450; */
/* 			    crapRec.right = 95+450; */
		
/* 			    ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/* 			    ddbltfx.dwSize = sizeof( ddbltfx); */

			    spr[1].seq = 0;
			    spr[1].pseq = 10;
			    spr[1].pframe = 1;
								
		

			    // Display the given tile square fullscreen, for hardness editing
/* 			    lpDDSTwo->Blt(&crapRec , tiles[cool+1], */
/* 					  &Rect, DDBLT_DDFX | DDBLT_WAIT,&ddbltfx ); */
			    // GFX
			    /* Generic scaling */
			    /* Not perfectly accurate yet: move a 200% sprite to the
			       border of the screen to it is clipped: it's scaled size
			       will slighly vary. Maybe we need to clip the source zone
			       before scaling it.. */
			    {
			      SDL_Rect src, dst;
			      SDL_Surface *scaled, *source;
			      double sx, sy;
			      src.x = xx * 50 - xx/12 * 600;
			      src.y = xx/12 * 50;
			      src.w = 50;
			      src.h = 50;
			      dst.x = 95;
			      dst.y = 0;
			      dst.w = 450;
			      dst.h = 450;
			      sx = 1.0 * dst.w / src.w;
			      sy = 1.0 * dst.h / src.h;
			      /* In principle, double's are precised up to 15 decimal
				 digits */
			      if (fabs(sx-1) > 1e-10 || fabs(sy-1) > 1e-10)
				{
				  scaled = zoomSurface(GFX_tiles[cool+1], sx, sy, SMOOTHING_OFF);
				  /* Disable transparency if it wasn't active in the source surface
				     (SDL_gfx bug, report submitted to the author) */
				  if ((GFX_tiles[cool+1]->flags & SDL_SRCCOLORKEY) == 0)
				    SDL_SetColorKey(scaled, 0, 0);
				  src.x = (int) round(src.x * sx);
				  src.y = (int) round(src.y * sy);
				  src.w = (int) round(src.w * sx);
				  src.h = (int) round(src.h * sy);
				  SDL_BlitSurface(scaled, &src, GFX_lpDDSTwo, &dst);
				  SDL_FreeSurface(scaled);
				}
			      else
				{
				  /* No scaling */
				  SDL_BlitSurface(GFX_tiles[cool+1], &src, GFX_lpDDSTwo, &dst);
				}
			    }

			    m4x = spr[h].x;
			    m4y = spr[h].y;
		
			    spr[1].x = 95;
			    spr[1].y = 0;
			    selx = 1;
			    sely = 1;
		
			    mode = MODE_TILE_HARDNESS;
		
			    kickass = /*TRUE*/1;
			  }
	
    
	
		      }



		    if ( (mode == MODE_TILE_PICKER) | (mode == MODE_SCREEN_TILES) )
		      {
			//resizing the box
	
			if ((SDL_GetModState()&KMOD_SHIFT) && (GetKeyboard(SDLK_RIGHT /* 39 */)) )
			  {
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    if (selx < 8) selx++;
			    goto b1end;
			  }
	
			if ((SDL_GetModState()&KMOD_SHIFT) && (GetKeyboard(SDLK_LEFT /* 37 */)) )
			  {
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    if (selx > 1) selx--;
			    goto b1end;
	
			  }
	
			if ((SDL_GetModState()&KMOD_SHIFT) && (GetKeyboard(SDLK_UP /* 38 */)) )
			  {
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    if (sely > 1) sely--;
			    goto b1end;
			  }
	
			if ((SDL_GetModState()&KMOD_SHIFT) && (GetKeyboard(SDLK_DOWN /* 40 */)) )
			  {
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    if (sely <  8) sely++;
			    goto b1end;
			  }
	
		      }


		    if (GetKeyboard(SDLK_RIGHT /* 39 */)) 
		      {
			spr[h].x += spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect( SOUND_STOP );
			//PlayMidi("TOP.MID");
	
		      }


		    if ((GetKeyboard('s')) && (mode == MODE_SCREEN_TILES))
		      {
	
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			//EditorSoundPlayEffect( SOUND_JUMP );
	
	
			pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].num = cur_tile;
	
			for (int y = 0; y < sely; y++)
			  {
			    for (int x = 0; x < selx; x++)
			      {
				holdx = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50);
				holdx += (y * 12);
				holdx += x;
				pam.t[holdx].num = (cur_tile + (y * 12) + x);
			
			      }
			  }
	
	
			draw_map();	
		      }



		    if ( (GetKeyboard('c')) && (mode == MODE_SCREEN_TILES) ) 
		      {
	
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			//SoundPlayEffect( SOUND_JUMP );
			cur_tile = pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].num;
			draw_map();	
		      }

		    /* Tile selection */
		    if (mode == MODE_SCREEN_TILES || mode == MODE_TILE_PICKER)
		      {
			int unit = 0, tile_no = 0;
			if (GetKeyboard('1')) unit = 1;
			if (GetKeyboard('2')) unit = 2;
			if (GetKeyboard('3')) unit = 3;
			if (GetKeyboard('4')) unit = 4;
			if (GetKeyboard('5')) unit = 5;
			if (GetKeyboard('6')) unit = 6;
			if (GetKeyboard('7')) unit = 7;
			if (GetKeyboard('8')) unit = 8;
			if (GetKeyboard('9')) unit = 9;
			if (GetKeyboard('0')) unit = 10;
			
			tile_no = unit;
			if (SDL_GetModState()&KMOD_SHIFT)
			  tile_no = 10 + unit;
			if (SDL_GetModState()&KMOD_CTRL)
			  tile_no = 20 + unit;
			if (SDL_GetModState()&KMOD_ALT)
			  tile_no = 30 + unit;

			if (unit > 0) /* make sure one key was pressed */
			  loadtile(tile_no);

			/* Exception: tile #41 = Alt+` */
			if (SDL_GetModState()&KMOD_ALT
			    /* && GetKeyboard(192/\* VK_OEM_3 *\/ /\* 192/'`' for US *\/)) */
			    && GetKeyboard(SDLK_BACKQUOTE))
			  loadtile(41);
		      }

		    //if ( (GetKeyboard(48)) && ( (mode == 3) | (mode ==2)) ) loadtile(11);


		    if ( (sjoy.button[2]) && (mode == MODE_TILE_PICKER)) 
		      {
			// cut to map editer from tile selection
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			cur_tile = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50); 
			cur_tile += (cur_screen * 128) - 128;
			//SoundPlayEffect( SOUND_JUMP );
			m2x = spr[h].x;
			m2y = spr[h].y;
			spr[h].x = m3x;
			spr[h].y = m3y;
	
			mode = MODE_SCREEN_TILES;
			spr[h].speed = 50;
			draw_map();
			last_mode = 0;
		      }



		    if ( (sjoy.button[1]) && (mode == MODE_TILE_PICKER)) 
		      {
			// cut to map editer from tile selection
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			//cur_tile = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50); 
			//SoundPlayEffect( SOUND_JUMP );
			m2x = spr[h].x;
			m2y = spr[h].y;
			spr[h].x = m3x;
			spr[h].y = m3y;
	
			mode = MODE_SCREEN_TILES;
			draw_map();
			last_mode = 0;
			goto b1end;
		      }


		    if ( (sjoy.key[SDLK_SPACE])  && (mode == MODE_MAP_PICKER))
		      {  
			//make_map_tiny();
			draw_map_tiny = 0;

		      }

		    if ( (sjoy.key['l'])  && (mode == MODE_MAP_PICKER))
		      {  

			//if (map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)] != 0)
			//{
			buf_map = (((spr[1].y+1)*32) / 20)+(spr[1].x / 20);
			in_master = 30;
			//}

		      }


		    if ( (sjoy.key[SDLK_ESCAPE /* 27 */]) && (mode == MODE_MAP_PICKER))
		      {
			load_info();
			draw_used();
			buf_mode = false;

		      }


		    if ( (sjoy.key['m']) && (mode == MODE_MAP_PICKER))
		      {
			//set music # for this block
			in_int = &map.music[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
			in_master = 33;
		      }

		    if ( (sjoy.key['s']) && (mode == MODE_MAP_PICKER))
		      {
			//set music # for this block
			in_int = &map.indoor[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
			in_master = 34;
		      }

		    if ( (sjoy.button[2]) && (mode == MODE_MAP_PICKER))
		      {

			if (buf_mode)
			  {
			    //lets replace this screen
 
			    buf_mode = false;
	
		
			    if (!load_map_buf(buffmap.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)]))
			      {


				draw_used();
				sjoy.button[2] = false;
				return;
			      }
	  
		
			    load_info();

			    if (map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)] == 0)
			      {

				(
				 map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)]) = add_new_map();
				//wrongo, let's add the map
              
				//draw_used();
	 
	 
			      }
		
			    map.indoor[buf_map] = buffmap.indoor[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
			    map.music[buf_map] = buffmap.music[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
		
			    save_map(map.loc[buf_map]);
       
			    save_info();
			    draw_used();
			    return;
			  }
	
			load_info();
	
			cur_map=  (((spr[1].y+1)*32) / 20)+(spr[1].x / 20);
			if (map.loc[cur_map] == 0)
			  {
			    //new map screen
	
			    map.loc[cur_map] = add_new_map();
			    save_info();
	
	
			  } else load_map(map.loc[cur_map]);
	
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			k[seq[3].frame[1]].xoffset = -playl;
			//SoundPlayEffect( SOUND_JUMP );
	
			Msg("Y is %d X is %d", spr[h].y, spr[h].x);
	
			m1x = spr[h].x;
			m1y = spr[h].y;
			spr[h].x = m3x;
			spr[h].y = m3y;
	
	
			mode = MODE_SCREEN_TILES;
	
			spr[h].speed = 50;
			draw_map();
		      }



		    /* TODO: where is it in the editor, and what are
		       the keys in the original dinkedit? */
		    if ( (mode == MODE_SCREEN_TILES) && (GetKeyboard(189 /* VK_OEM_MINUS */)) )
		      {
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			cur_tile--;
			if (cur_tile < 0) cur_tile = 0;
		      }
		    if ( (mode == MODE_SCREEN_TILES) && (GetKeyboard(187 /* VK_OEM_PLUS */)) )
		      {
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
	
			cur_tile++;
			//if (cur_tile > 127) cur_tile = 127;
		      }


		    if ( (mode == MODE_SCREEN_TILES) && (sjoy.key['h']) )
		      {
			//start althard mode
	

			mode = MODE_UNKNOWN;
			goto skip_draw;



		      }


		    if (mode == MODE_SCREEN_HARDNESS)
		      {
			//mode for it
			if (sjoy.key[SDLK_ESCAPE]) 
			  {
			    //exit mode 8
			    mode = MODE_SCREEN_TILES;
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    draw_map();
			    goto b1end;
			  }

			/* if (sjoy.key[/\* VK_OEM_6 *\/ 221]) // ']' for US */
			if (GetKeyboard(SDLK_RIGHTBRACKET))
			  {
			    hard_tile++;
			    if (hard_tile > 799) hard_tile = 1;
			  }
			/* if (sjoy.key[/\* VK_OEM_4 *\/ 219]) // '[' for US */
			if (GetKeyboard(SDLK_LEFTBRACKET))
			  {
			    hard_tile--;
			    if (hard_tile < 1) hard_tile = 799;
			  }
 
			if (sjoy.key['c'])
			  {
			    //copy tile hardness from current block
			    hard_tile = realhard(   (((spr[1].y+1)*12) / 50)+(spr[1].x / 50)   );

			  }

			if (sjoy.key['s'])
			  {
			    //stamp tile hardness to selected
			    pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].althard = hard_tile;
			    draw_map();
			    mode = MODE_UNKNOWN;

			    return;
			  }

			if (sjoy.key[SDLK_DELETE])
			  {
			    //stamp tile hardness to selected
			    pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].althard = 0;
			    draw_map();
			    mode = MODE_UNKNOWN;

			    return;
			  }



			draw_hard_tile(spr[1].x,spr[1].y,hard_tile);

			char crapa[10];
			sprintf(crapa, "%d",hard_tile);
  
			SaySmall(crapa,580,400,255,255,255);

			if (sjoy.key[SDLK_RETURN])
			  {

			    //they want to edit this alt hardness, less do it'
			    cur_tile = pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].num;
	
  
      
			    xx = cur_tile - (cool * 128);
			    Rect.left = spr[1].x+20;
			    Rect.top = spr[1].y;
			    Rect.right = Rect.left + 50;
			    Rect.bottom = Rect.top + 50;
		
			    crapRec.top = 0;
			    crapRec.left = 95;
			    crapRec.bottom = 450;
			    crapRec.right = 95+450;
		
/* 			    ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/* 			    ddbltfx.dwSize = sizeof( ddbltfx); */
			    spr[1].seq = 0;
			    spr[1].pseq = 10;
			    spr[1].pframe = 1;
		
		
/* 			    lpDDSTwo->Blt(&crapRec , lpDDSBack, */
/* 					  &Rect, DDBLT_DDFX | DDBLT_WAIT,&ddbltfx ); */
			    // GFX
			    /* Generic scaling */
			    /* Not perfectly accurate yet: move a 200% sprite to the
			       border of the screen to it is clipped: it's scaled size
			       will slighly vary. Maybe we need to clip the source zone
			       before scaling it.. */
			    /* Moreover, in this particular case,
			       we're scaling the whole screen
			       backbuffer by 900% just to scale a
			       single 50x50 square of it... */
			    {
			      SDL_Rect src, dst;
			      SDL_Surface *scaled, *source;
			      double sx, sy;
			      src.x = spr[1].x+20;
			      src.y = spr[1].y;
			      src.w = 50;
			      src.h = 50;
			      dst.x = 95;
			      dst.y = 0;
			      dst.w = 450;
			      dst.h = 450;
			      sx = 1.0 * dst.w / src.w;
			      sy = 1.0 * dst.h / src.h;
			      /* In principle, double's are precised up to 15 decimal
				 digits */
			      if (fabs(sx-1) > 1e-10 || fabs(sy-1) > 1e-10)
				{
				  scaled = zoomSurface(GFX_lpDDSBack, sx, sy, SMOOTHING_OFF);
				  /* Disable transparency if it wasn't active in the source surface
				     (SDL_gfx bug, report submitted to the author) */
				  if ((GFX_lpDDSBack->flags & SDL_SRCCOLORKEY) == 0)
				    SDL_SetColorKey(scaled, 0, 0);
				  src.x = (int) round(src.x * sx);
				  src.y = (int) round(src.y * sy);
				  src.w = (int) round(src.w * sx);
				  src.h = (int) round(src.h * sy);
				  SDL_BlitSurface(scaled, &src, GFX_lpDDSTwo, &dst);
				  SDL_FreeSurface(scaled);
				}
			      else
				{
				  /* No scaling */
				  SDL_BlitSurface(GFX_lpDDSBack, &src, GFX_lpDDSTwo, &dst);
				}
			    }

			    m4x = spr[h].x;
			    m4y = spr[h].y;
		
			    spr[1].x = 95;
			    spr[1].y = 0;
			    selx = 1;
			    sely = 1;
		
			    mode = MODE_TILE_HARDNESS;
		
			    kickass = /*TRUE*/1;
          
			    hmap.tile[hard_tile].used = true;
			    last_modereal = 8;
			  }



		      }

		    if ((mode == MODE_SCREEN_TILES) && (SDL_GetModState()&KMOD_ALT)) if (sjoy.key['x'])
									     {
									       spr[h].seq = 2; 
									       spr[h].seq_orig = 2; 
									       m3x = spr[h].x;
									       m3y = spr[h].y;
									       spr[h].x = m1x;
									       spr[h].y = m1y;
									       mode = MODE_MAP_PICKER;
									       spr[h].speed = 20;
									       load_info();
									       draw_used();
									       while (kill_last_sprite()); 
									       return;
									     }


		    if ((mode == MODE_SCREEN_TILES) && (sjoy.button[1]) ) 
		      {
			// jump to map selector selector from map mode
			save_map(map.loc[cur_map]);
			spr[h].seq = 2; 
			spr[h].seq_orig = 2; 
			//SoundPlayEffect( SOUND_JUMP );
			m3x = spr[h].x;
			m3y = spr[h].y;
			//Msg("m1y is %d, math is %d",m1y, (20 * (m1y / 20)) < m1y);
			spr[h].x = m1x;
			spr[h].y = m1y;
			mode = 1;
			spr[h].speed = 20;
			load_info();
			draw_used();
			while (kill_last_sprite()); 
			return;	
		      }


		    if (GetKeyboard(SDLK_LEFT /* 37 */)) 
		      {
			spr[h].x -= spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect( SOUND_STOP );
		      }


		    //if (GetKeyboard(127) PostMessage(hWnd, WM_CLOSE, 0, 0);

		    if (GetKeyboard(SDLK_DOWN /* 40 */))
		      {
			spr[h].y += spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect( SOUND_STOP );
		      }

		    if (GetKeyboard(SDLK_UP /* 38 */)) 
		      {
			spr[h].y -= spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect( SOUND_STOP );
		      }

		    if (spr[h].speed < 1) spr[h].speed = 1;
		    if (spr[h].y > (y-k[getpic(h)].box.bottom)) spr[h].y = (y-k[getpic(h)].box.bottom);
		    if (spr[h].x > (x-k[getpic(h)].box.right)) spr[h].x = (x-k[getpic(h)].box.right);
		    if (spr[h].x < 0) spr[h].x = 0;
		    if (spr[h].y < 0) spr[h].y = 0;

		    // end human brain (1)

		
		
		    //if( (mode == 2) | (mode == 3) | (mode == 5) )
		    if( (mode == MODE_TILE_PICKER) | (mode == MODE_SCREEN_TILES) | (mode == MODE_SPRITE_PICKER) )
		      {
			if ((selx * 50 + spr[1].x) > 600)
			  { 
			    spr[1].x = 600 - (selx * 50);
			  }
		      }
		
		    //if( (mode == 2) )
		    if( (mode == MODE_TILE_PICKER) )
		      {
			if ((sely * 50 + spr[1].y) > 450)
			  { 
			    spr[1].y = 450 - (sely * 50);
			  }
		      }
		    //		if( (mode == 3) )
		    if( (mode == MODE_SCREEN_TILES) )
		      {
			if ((sely * 50 + spr[1].y) > 400)
			  { 
			    spr[1].y = 400 - (sely * 50);
			  }
		      }

		    //		if( (mode == 5) )
		    if( (mode == MODE_SPRITE_PICKER) )
		      {
			if ((sely * 50 + spr[1].y) > 400)
			  { 
			    spr[1].y = 400 - (sely * 50);
			  }
		      }
		

		  b1end:;

		  } //end if seq is 0
	      } //real end of human brain
		

	    if (spr[h].brain == 2)
	      {
			
			
		if (spr[h].y > (y-k[getpic(h)].box.bottom))
		  {
		    spr[h].my -= (spr[h].my * 2);
		    // SoundPlayEffect( SOUND_JUMP );
		  }         
			
		if (spr[h].x > (x-k[getpic(h)].box.right))
		  {
		    spr[h].mx -= (spr[h].mx * 2);
		    //SoundPlayEffect( SOUND_JUMP );
		  }         
			
		if (spr[h].y < 0)
		  {
		    spr[h].my -= (spr[h].my * 2);
		    //SoundPlayEffect( SOUND_JUMP );
		  }         
			
			
		if (spr[h].x < 0) 
		  {
		    spr[h].mx -= (spr[h].mx * 2);
		    //SoundPlayEffect( SOUND_JUMP );
		  }         
			
			
		spr[h].x += spr[h].mx;
		spr[h].y += spr[h].my;
			
			
	      }
	    // end robot(2)
	
	  
		
	    if (spr[h].seq > 0)
	      {
		if (spr[h].frame < 1)
		  {
		    // new anim
		    spr[h].pseq = spr[h].seq;
		    spr[h].pframe = 1;
				
		    spr[h].frame = 1;
		    spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[1]);
		  } else
		  {
		    // not new anim
				
		    //is it time?
				
		    if (thisTickCount > spr[h].delay)
		      {
					
					
			spr[h].frame++;
			spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);
					
			spr[h].pseq = spr[h].seq;
			spr[h].pframe = spr[h].frame;
					
			if (seq[spr[h].seq].frame[spr[h].frame] == -1)
			  {
			    spr[h].frame = 1;
			    spr[h].pseq = spr[h].seq;
			    spr[h].pframe = spr[h].frame;
			    spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);
						
			  }
					
			if (seq[spr[h].seq].frame[spr[h].frame] == 0)
			  {
			    spr[h].pseq = spr[h].seq;
			    spr[h].pframe = spr[h].frame-1;
						
			    spr[h].frame = 0;
			    spr[h].seq = 0;
						
						
			  }
					
					
		      }
				
		  }		
	      }
	    int greba;			
	
		
		
	    if ( (mode == MODE_SCREEN_TILES) )
	      {
		//need offset to look right
		k[seq[3].frame[1]].xoffset = -20;
		greba = 20;
	      }
	    if ( (mode == MODE_TILE_PICKER) | (mode == MODE_SPRITE_PICKER) ) 
			
	      {
		//pick a tile, needs no offset
		k[seq[3].frame[1]].xoffset = 0;
		greba = 0;
	      }
	
	    //		if (  !(( h == 1) & (mode == 9)) )
	    if (  !(( h == 1) & (mode == MODE_UNKNOWN)) )
		
	      {

		if (draw_map_tiny == -1)
		  draw_sprite(GFX_lpDDSBack, h);            
		else
		  draw_sprite(GFX_lpDDSTwo, h);            
	      }
			
	    //Msg("Drew %d.",h);
		
	  skip_draw:
	    if (spr[h].brain == 1)
	      {
		if ( (mode == MODE_TILE_PICKER) || (mode == MODE_SCREEN_TILES))
		  {
		    /* Draw the tile squares selector, an expandable
		       array of white non-filled squares */
		    for (int y = 0; y < sely; y++)
		      {
			for (int x = 0; x < selx; x++)
			  {
						
/* 			    ddrval = lpDDSBack->BltFast( (spr[h].x+(50 *x))+greba,spr[h].y+(50 * y), k[getpic(h)].k, */
/* 							 &k[getpic(h)].box  , DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT ); */
			    // GFX
			    {
			      SDL_Rect dst;
			      dst.x = spr[h].x + 50*x + greba;
			      dst.y = spr[h].y + 50*y;
			      SDL_BlitSurface(GFX_k[getpic(h)].k, NULL, GFX_lpDDSBack, &dst);
			    }
			  }
		      }
				
		  }
			
            
		if ( (mode == MODE_TILE_HARDNESS))
		  {
		    /* Display the current "pencil"/square to draw hardness with */
		    for (int yy = 0; yy < sely; yy++)
		      {
			for (int xx = 0; xx < selx; xx++)
			  {
						
/* 			    ddrval = lpDDSBack->BltFast( spr[h].x+(9 * xx),spr[h].y+(9 * yy), k[getpic(h)].k, */
/* 							 &k[getpic(h)].box  , DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT ); */
			    // GFX
			    {
			      SDL_Rect dst;
			      dst.x = spr[h].x + 9*xx;
			      dst.y = spr[h].y + 9*yy;
			      SDL_BlitSurface(GFX_k[getpic(h)].k, NULL, GFX_lpDDSBack, &dst);
			    }
			  }
		      }
				
				
		  }
			
	      }
		
	
 	

	  }

      }
		
			
  //if (mode == 9)
  if (mode == MODE_UNKNOWN)
    {
      //mode = 8;
      mode = MODE_SCREEN_HARDNESS;
	
      fill_whole_hard();    
      while(kill_last_sprite());
      place_sprites();
							
      /*	draw_map();
        
					
		rcRect.top = 0;
		rcRect.right = x;
		rcRect.bottom = y;
		ddrval = lpDDSBack->BltFast( 0, 0, lpDDSTwo,
		&rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
      */					
      drawallhard();	 
		
      rcRect.left = 0;
      rcRect.right = 640;
      rcRect.top = 0;
      rcRect.bottom = 400;
/*       ddrval = lpDDSTwo->BltFast( 0, 0, lpDDSBack, */
/* 				  &rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
/*       if (ddrval != DD_OK) dderror(ddrval); */
      // GFX
      SDL_BlitSurface(GFX_lpDDSBack, NULL, GFX_lpDDSTwo, NULL);

      while(kill_last_sprite());


  


    }


  //sprintf(msg, "k[1] top is %d",k[1].box.top);

  //prepare to display misc messages for debug purposes

/*   if (lpDDSBack->GetDC(&hdc) == DD_OK) */
/*     { */
/*       SetBkMode(hdc, TRANSPARENT);  */
	
/*       //   SetBkMode(hdc, OPAQUE); */
/*       SetBkColor(hdc, 1); */
/*       SetTextColor(hdc,RGB(200,200,200)); */
      FONTS_SetTextColor(200, 200, 200);
	   
      //	TextOut(hdc,0,0, msg,lstrlen(msg));
      //	   if (mode == 0) strcpy(msg,"");
      if (mode == MODE_DIALOG)
	strcpy(msg,"");
	  
      if (mode == MODE_MAP_PICKER)
	{
	  if (20 * (spr[1].y / 20) != spr[1].y)
	    {
	      spr[1].y += 10;
	    }
	  sprintf(msg, "Map # %d - Press ENTER to edit, SPACE to detail map. (%d)  (Q) to quit and save. L to replace a "
		  "screen from another map file.  Z to toggle this help text.",mode,(((spr[1].y+1)*32) / 20)+(spr[1].x / 20),
		  map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20) ]);
	}
      if (mode == MODE_SCREEN_TILES)
	{
	  crap = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50);
	  //((x-1) - (x / 12))*50, (x / 12) * 50, 
	  sprintf(msg, "Map # %d, (C)opy or (S)tamp tile. ESC to exit to map picker. ENTER to edit hardness. TAB for sprite edit mode. 1-10 for tilescreens. (hold alt, crtl or shift for more) SPACE to show hardness of screen. (I)nfo on sprites."
		  "V to change vision, B to change maps base .C file.",mode,cur_map
		  ,cur_tile, pam.t[crap].num);
	}
      if (mode == MODE_TILE_PICKER)
	sprintf(msg, "Map # %d - Current tile # %d - ENTER to choose, SPACE to edit hardness.",mode,cur_map,
		(((spr[1].y+1)*12) / 50)+(spr[1].x / 50) );
      if (mode == MODE_TILE_HARDNESS)
	{
	  sprintf(msg, "X:%d Y:%d: Density index %d. (for tile %d) Z to harden, X to soften.  Shift+direction for larger brush. ENTER or ESC to exit.",
		  (spr[1].x / 9) -9,(spr[1].y / 9) +1,hmap.index[cur_tile], cur_tile);
	}
	   
      if (mode == MODE_SPRITE_PICKER)
	{
		   
	  if (sp_seq == 0)
	    {
	      sprintf(msg, "Choose sequence and press ENTER.  ] for next page, [ for previous. ESC or TAB to exit. (now on page %d)",
		      1+(sp_picker / 96));
	    } else
	    {
	      sprintf(msg, "Choose a sprite from sequence %d.  Enter to place sprite, TAB to exit or ESC to return to previous screen."
		      " E to edit depth dot and hardbox",
		      sp_seq);
			   
	    }
	}
	   
      if (mode == MODE_SCREEN_SPRITES)
	{
		   
	  char crap7[10];
	  if (sp_screenmatch) strcpy(crap7, "ScreenMatch is ON."); else strcpy(crap7, "ScreenMatch is OFF");

	  sprintf(msg, "Press ENTER to pickup/putdown sprite. Space to show hardness.  E to edit/pick new sprite. SHIFT to move fast. (S)tamp sprite. ] &"
		  "[ to scale (now at %d). DEL to erase sprite.  Press 1 through 9 to change sprite attributes. (shift for more)  Last sprite touched: %d  %s (M to toggle)"
		  "Hold Z or X + arrow keys to trim a sprite. V to change Vision mode. X: %d Y: %d",
		  spr[1].size,last_sprite_added,crap7, spr[1].x, spr[1].y);
		   
	  //lets draw the depth dot
			

		   
		   
	}
	   
	   
      if (mode == MODE_SPRITE_HARDNESS)
	{

	
	  if (sp_mode == 0)
	    {
	      sprintf(msg, "Editting depth dot for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini.",
		      sp_seq,sp_frame);
		   
	    }	   
		   
	  if ( (sp_mode == 1) )
		
	    {
	      sprintf(msg, "Editting hardbox up left cordinate for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini. X: %d Y: %d",
		      sp_seq,sp_frame, k[seq[sp_seq].frame[sp_frame]].hardbox.left,
		      k[seq[sp_seq].frame[sp_frame]].hardbox.top);
		   
	    }	   
		   
	  if ( (sp_mode == 2) )
		
	    {
	      sprintf(msg, "Editting hardbox down right cordinate for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini. X: %d Y: %d",
		      sp_seq,sp_frame, k[seq[sp_seq].frame[sp_frame]].hardbox.right,
		      k[seq[sp_seq].frame[sp_frame]].hardbox.bottom);
		   
	    }	   
	
 

   


	}
	   

      if (mode == MODE_SCREEN_HARDNESS)
	{

	  sprintf(msg, "Alternative Tile Hardness Selector: Press S to stamp this tiles hardness."
		  "  DEL to remove alternate hardness.  C to copy from current block. [ & ] to cycle.  ESCAPE to exit.");
	}


      for (int x=0; x<256; x++)
	{ 
	  if (GetKeyboard(x))
	    {
	      sprintf(msg, "%s (Key %i)",msg,x);
	    }
	}
      rcRect.left = 0;
      rcRect.top = 400;
      if (mode == MODE_TILE_HARDNESS) rcRect.top = 450;
      rcRect.right = 590;
      rcRect.bottom = 480;
      if (show_display)
	{
	  /* Display help message at the bottom of the screen */
/* 	  DrawText(hdc,msg,lstrlen(msg),&rcRect,DT_WORDBREAK); */
	  // FONTS
	  /* TODO: use Say() or Saysmall()? */
	  print_text_wrap(msg, &rcRect, 0, 0);
	}
	   
/*       lpDDSBack->ReleaseDC(hdc); */

/*     } /\* GetDC(&hdc) *\/ */


  if ( (mode == MODE_MAP_PICKER) )
    {

      if (sjoy.key['z']) if (show_display) show_display = false; else
	  show_display = true;

    }


  if ( (mode == MODE_SCREEN_SPRITES) | (mode == MODE_SCREEN_TILES) )
    {
      /* Show sprites info */
      if (sjoy.realkey['i'])
	{
	  for (int j =1; j < 100; j++)
	    {
	      if (pam.sprite[j].active == true)
		{
         
/* 		  ddbltfx.dwSize = sizeof(ddbltfx); */
/* 		  ddbltfx.dwFillColor = 230; */
	
		  //info on the sprites  sprite info
		  int temp = s_index[pam.sprite[j].seq].s + pam.sprite[j].frame;
			

		  int sprite2 = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y,0,
						pam.sprite[j].seq,pam.sprite[j].frame,
						pam.sprite[j].size);
		  CopyRect(&spr[sprite2].alt , &pam.sprite[j].alt);                 
				
		  get_box(sprite2, &box_crap, &box_real);
                                
		  spr[sprite2].active = false;

		  //box_crap.top = box_crpam.sprite[j].y - 25;
		  box_crap.bottom = box_crap.top + 50;
		  box_crap.left = box_crap.left + ( (box_crap.right - box_crap.left) / 2);
		  box_crap.right = box_crap.left+ 50;
				
/* 		  ddrval = lpDDSBack->Blt(&box_crap ,k[seq[10].frame[5]].k,&k[seq[10].frame[5]].box, DDBLT_WAIT, &ddbltfx); */
		  //       	if (ddrval != DD_OK) dderror(ddrval);
		  // GFX
		  {
		    SDL_Rect dst;
		    SDL_Surface *sprite = GFX_k[seq[10].frame[5]].k;
		    dst.x = box_crap.left;
		    dst.y = box_crap.top;
		    /* Simplified blit, no scaling, the sprite is already 50x50 */
		    /* We need to avoid transparency though */
		    Uint32 colorkey = sprite->format->colorkey;
		    SDL_SetColorKey(sprite, 0, 0); /* Temporarily disable transparent color */
		    SDL_BlitSurface(sprite, NULL, GFX_lpDDSBack, &dst);
		    SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, colorkey);
		  }
				
		  char crap5[200];
		  
                  char crap6[20];

		  strcpy(crap6,"");
		  if (pam.sprite[j].hard == 0) strcpy(crap6,"HARD");

		  sprintf(crap5, "B: %d %s",pam.sprite[j].brain,crap6);
				
		  if (pam.sprite[j].type == 0)
		    {
				
		      SaySmall(crap5,box_crap.left+3,box_crap.top+3,255,255,255);
		    }

		  if (pam.sprite[j].type > 0)
		    {
		      SaySmall(crap5,box_crap.left+3,box_crap.top+3,255,0,0);
		    }
		  if (strlen(pam.sprite[j].script) > 1)
		    SaySmall(pam.sprite[j].script,box_crap.left+3,box_crap.top+35,255,0,0);
				
		  sprintf(crap6,"%d",j);
			
		  SaySmall(crap6,box_crap.left+20,box_crap.top-15,0,255,0);
	

		}
	    }
	}
	
    }
  if (mode == MODE_SPRITE_HARDNESS)
    {
/*       ddbltfx.dwSize = sizeof(ddbltfx); */
/*       ddbltfx.dwFillColor = 230; */
	
      if (sp_mode == 0)
	{
	  //draw depth dot for sprite attribute edit
	  
/* 	  box_crap.top = spr[2].y; */
/* 	  box_crap.bottom = spr[2].y+1; */
/* 	  box_crap.left = spr[2].x - 20; */
/* 	  box_crap.right = spr[2].x + 20; */
				
				
/* 	  ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
/* 	  if (ddrval != DD_OK) dderror(ddrval); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = spr[2].x - 20;
	    dst.y = spr[2].y;
	    dst.w = 40;
	    dst.h = 1;
	    SDL_FillRect(GFX_lpDDSBack, &dst, 230);
	  }

/* 	  box_crap.top = spr[2].y-20; */
/* 	  box_crap.bottom = spr[2].y+20; */
/* 	  box_crap.left = spr[2].x; */
/* 	  box_crap.right = spr[2].x +1; */
				
				
/* 	  ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
/* 	  if (ddrval != DD_OK) dderror(ddrval); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = spr[2].x;
	    dst.y = spr[2].y - 20;
	    dst.w = 1;
	    dst.h = 40;
	    SDL_FillRect(GFX_lpDDSBack, &dst, 230);
	  }
	}

      if  ((sp_mode == 1) | (sp_mode == 2) )
	{
	  //draw hardbox dot for sprite attribute edit
	  
/* 	  box_crap = k[seq[sp_seq].frame[sp_frame]].hardbox; */
	  
/* 	  OffsetRect(&box_crap,320,200); */


/* 	  ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
/* 	  if (ddrval != DD_OK) Msg("Error with drawing hard block... you know why."); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = k[seq[sp_seq].frame[sp_frame]].hardbox.left + 320;
	    dst.y = k[seq[sp_seq].frame[sp_frame]].hardbox.top + 200;
	    dst.w = k[seq[sp_seq].frame[sp_frame]].hardbox.right - k[seq[sp_seq].frame[sp_frame]].hardbox.left;
	    dst.h = k[seq[sp_seq].frame[sp_frame]].hardbox.bottom - k[seq[sp_seq].frame[sp_frame]].hardbox.top;
	    SDL_FillRect(GFX_lpDDSBack, &dst, 230);
	  }
	}
    }


  if (in_enabled)
    {
      //text window is open, lets act accordingly
      //check_joystick();

	
      if (getkey(SDLK_RETURN))
	{

	  //exit text mode


	  if (in_command == 2)
	    {
	      if (in_string != NULL)
		{
            
		
		  strcpy(in_string,in_temp);
		 
		   
		} else Msg("Error, in_char pointer not set, can't issue a value.");
		 
	    }

	
         
	  if (in_command == 1)
	    {
	      if (in_int != NULL)
		{
		  char *stop;
		  int in_crap = strtol(in_temp, &stop,10);
	    
		  in_crap2 = in_crap;
		
		  if (   (old_command == 33) | (old_command == 34)  )
		    {
		      load_info();
		    }

		  *in_int = in_crap2;
		
		
		
		  if (   (old_command == 33) | (old_command == 34)  )
		    {
		      save_info();
		    }
		 
		   
		} else Msg("Error, in_int pointer not set, can't issue a value.");
		 
	    }

		

	  in_command = 0;



	  if (in_huh == 3)
	    {
	      if (in_crap2 == 3)
		{
		  //default duck settings
		  sp_speed = 1;
		  sp_base_walk = 20;
		  sp_base_idle = -1;
		  sp_base_hit = -1;
		  sp_base_attack = -1;
		  sp_timer = 33;
		  sp_type = 1;
		  sp_que = 0;
		  sp_hard = 1;
			
		}
			
	      if (in_crap2 == 4)
		{
		  //default pig settings
		  sp_speed = 1;
		  sp_base_walk = 40;
		  sp_base_idle = -1;
		  sp_base_hit = -1;
		  sp_base_attack = -1;
		  sp_timer = 66;
		  sp_type = 1;
		  sp_que = 0;
		  sp_hard = 1;
		}

	      if (in_crap2 == 9)
		{
		  //default diag settings
		  sp_speed = 1;
		  sp_base_walk =  (sp_seq / 10) * 10;
		  sp_base_idle = -1;
		  sp_base_hit = -1;
		  sp_base_attack = -1;
		  sp_timer = 66;
		  sp_type = 1;
		  strcpy(sp_script, "");
		  sp_que = 0;
		  sp_hard = 1;
		}

	      if (in_crap2 == 10)
		{
		  //default diag settings
		  sp_speed = 1;
		  sp_base_walk =  (sp_seq / 10) * 10;
		  sp_base_idle = -1;
		  sp_base_hit = -1;
		  sp_base_attack = -1;
		  sp_timer = 66;
		  sp_type = 1;
		  strcpy(sp_script, "");
		  sp_que = 0;
		  sp_hard = 1;
		}


	    }
	  if (old_command == 32)
	    {
	      draw_map();

	    }
		
	  in_enabled = false;
		
	  if (mode == MODE_MAP_PICKER) if (old_command == 30)
					 {
					   draw_used_buff();
					   return;
					 }
		
	  if (mode == MODE_MAP_PICKER) if (old_command == 33)
					 {
		
					   draw_used();
					   return;
					 }
		
	  if (mode == MODE_MAP_PICKER) if (old_command == 34)
					 {
		
					   Msg("drawing used");
					   draw_used();
					   return;
					 }
		

	  draw_map();
	
	  return;	
	}
	
      if (sjoy.key[SDLK_BACKSPACE])

	//	if (getkey(8)) //this is a much faster backspace than the above
	{
	  if (strlen(in_temp) > 0)
	    in_temp[strlen(in_temp)-1] = 0;
		
	} 


      if (in_max > strlen(in_temp))
	for (int x=32; x<255; x++)
	  {
	    if (sjoy.key[x])
									
	      {
		int key = key_convert(x);
								
		sprintf(in_temp, "%s%c",in_temp,key);
				                    
	      }
	  }
      Say(in_temp,260,200);

    }


  if (in_master != 0) check_in();


  //open a text window?

  if (in_onflag)
    {
      //start it up

      //copy screen to Two
/*       SetRect(&rcRect, 0, 0, 640, 480); */
/*       ddrval = lpDDSTwo->Blt( &rcRect, lpDDSBack, &rcRect, DDBLT_WAIT, NULL); */
/*       if (ddrval != DD_OK) dderror(ddrval); */
      // GFX
      // TODO: use copy_front_to_two()
      SDL_BlitSurface(GFX_lpDDSBack, NULL, GFX_lpDDSTwo, NULL);

		
      strcpy(in_temp,in_default);
      in_x = 270;
      in_y = 190;
      in_onflag = false;
      in_enabled = true;
         


    }



  //MAIN PAGE FLIP DONE HERE


  if (GetKeyboard(SDLK_SPACE) && (mode != 1))
    {
      drawallhard();
						
    }




  if (draw_map_tiny != -1)
    {
          
      int huh = 0;
      //if (draw_map_tiny > 32) huh = 1;
      shrink_screen_to_these_cords(  (draw_map_tiny-1) * 20 - ((((draw_map_tiny-1) / 32) * 640) )   ,   ((((draw_map_tiny-1) / 32)- huh) * 20));
      //Msg("Just flipped # %d", draw_map_tiny);

    }



/*   if (!windowed) */
/*     { */
	
/*       while( 1 ) */
/* 	{ */
/* 	  ddrval = lpDDSPrimary->Flip(NULL,DDFLIP_WAIT ); */
/* 	  if( ddrval == DD_OK ) */
/* 	    { */
/* 	      break; */
/* 	    } */
/* 	  if( ddrval == DDERR_SURFACELOST ) */
/* 	    { */
/* 	      ddrval = restoreAll(); */
/* 	      if( ddrval != DD_OK ) */
/* 		{ */
/* 		  break; */
/* 		} */
/* 	    } */
/* 	  if( ddrval != DDERR_WASSTILLDRAWING ) */
/* 	    { */
			
/* 	      dderror(ddrval); */
/* 	    } */
/* 	}  */

/*       if (draw_map_tiny != -1)  */
/* 	{ */
	  //extra flip
		
/* 	  ddrval = lpDDSPrimary->Flip(NULL,DDFLIP_WAIT ); */
/* 	} */


/*     } else */
/*     { */
      /* Replaced by a call to flip_it_second() */
      flip_it_second();
      /*
      //windowed mode, no flipping		 
      p.x = 0; p.y = 0;    
      ClientToScreen(hWndMain, &p);
      GetClientRect(hWndMain, &rcRectDest);
	
      //rcRectDest.top += winoffset;
      rcRectDest.bottom = 480;
      rcRectDest.right = 640;
	
      OffsetRect(&rcRectDest, p.x, p.y);
      SetRect(&rcRectSrc, 0, 0, 640, 480);
	
	
	
				
      ddbltfx.dwSize = sizeof(ddbltfx);
	
      ddbltfx.dwDDFX = DDBLTFX_NOTEARING;
      ddrval = lpDDSPrimary->Blt( &rcRectDest, lpDDSBack, &rcRectSrc, DDBLT_DDFX | DDBLT_WAIT, &ddbltfx);
      */
/*     } */
} /* updateFrame */

/*
 * finiObjects
 *
 * finished with all objects we use; release them
 */
/* TODO: merge with freedink.cpp */
void finiObjects( void )
{
  printf("Running cleanup (finiObjects)\n");
	  
/* 	  if( lpDD != NULL ) */
/* 	  { */
/* 		  if( lpDDSPrimary != NULL ) */
/* 		  { */
/* 			  lpDDSPrimary->Release(); */
/* 			  lpDDSPrimary = NULL; */
/* 		  } */
		  
/* 		  if( lpDDSTwo != NULL ) */
/* 		  { */
/* 			  lpDDSTwo->Release(); */
/* 			  lpDDSTwo = NULL; */
/* 		  } */
		  
		  
/* 		  if( lpDDPal != NULL ) */
/* 		  { */
/* 			  lpDDPal->Release(); */
/* 			  lpDDPal = NULL; */
/* 		  } */
/* 		  lpDD->Release(); */
/* 		  lpDD = NULL; */
/* 	  } */

	   
/* 	  //destroy direct input mouse stuff */
/* 	  if (g_pdi)      g_pdi   ->Release(), g_pdi    = NULL; */


/*     if (g_pMouse)   g_pMouse->Release(), g_pMouse = NULL; */

/*     if (g_hevtMouse) CloseHandle(g_hevtMouse), g_hevtMouse = NULL; */



if (sound_on)
	  DestroySound();

	FastFileFini(); 
	void kill_fonts();
	kill_fonts();


  } /* finiObjects */
  
/* long FAR PASCAL WindowProc(HWND hWnd, UINT message,  */
/* 			   WPARAM wParam, LPARAM lParam) */
/* { */
/* 	  switch( message ) */
/* 	  { */
/* 	  case WM_ACTIVATEAPP: */
/* 		  bActive = wParam; */
/* 		  break; */
		  
/* 	  case WM_SETCURSOR: */
/* 		  SetCursor(NULL); */
/* 		  return /\*TRUE*\/1; */
		  
/* 	  case WM_CREATE: */
/* 		  break; */
		  
/* 	  case WM_KEYDOWN: */
/* 		  switch( wParam ) */
/* 		  { */
/* 		    /\* TODO: put it outside of the Windows loop *\/ */
/* 		  case 'Q' /\* 81 *\/: */
			  
			  
/* 			  if (mode == MODE_MAP_PICKER) */
/* 			  { */
				  
/* 				  save_hard(); */
/* 				  Msg("Info saved."); */
/* 				  PostMessage(hWnd, WM_CLOSE, 0, 0); */
/* 			  } */
/* 			  break; */
			  
/*         		case VK_F1: */
/* 					{ */
/* 					Msg("F1 pressed"); */
/* 					} */
/* 					break; */
					
					
/* 		  } */
/* 		  break; */
		  
/* 		  case WM_DESTROY: */
/* 			  finiObjects(); */
/* 			  PostQuitMessage( 0 ); */
/* 			  break; */
/* 	  } */
	  
/* 	  return DefWindowProc(hWnd, message, wParam, lParam); */
	  
/*   } /\* WindowProc *\/ */
  
  

/*
 * This function is called if the initialization function fails
 */
/* TODO: merge with freedink.cpp */
/*BOOL*/int initFail(char mess[200])
{
/* 	  MessageBox( hwnd, mess, TITLE, MB_OK ); */
    printf("%s\n", mess);
    finiObjects();
/* 	  DestroyWindow( hwnd ); */
    return /*FALSE*/0;
} /* initFail */
  

  
bool check_arg(int argc, char *argv[])
{
	  char shit[200];
	  
	  //	strupr(crap);
	  strcpy(dir, "dink");
	  for (int i=1; i < argc; i++)
	  {
/* 		  separate_string(crap, i,' ',shit); */
	    strcpy(shit, argv[i]);

		  if (strncasecmp(shit,"-window",strlen("-window")) == 0)
		  {
			  windowed = true;
			  
		  }

         if (strncasecmp(shit,"-debug",strlen("-debug")) == 0)
		  {
			 remove("debug.txt"); 
			 debug_mode = true;
			  
		  }


  		  if (strncasecmp(shit,"-game",strlen("-game")) == 0)
		  {
/* 		separate_string(crap, i+1,' ',shit); */
		    strcpy(shit, argv[i+1]);
		strcpy(dir, shit);
		Msg("Working directory %s requested.",dir);  
		  
		  }

if (strncasecmp(shit,"-nosound",strlen("-nosound")) == 0)  sound_on = false;

		  
	  }
	  
	  
if (chdir(dir) == -1) 
{
    sprintf(shit,"Game dir \"%s\" not found!",dir);

	TRACE(shit);
}	  
 
  return(true);
  }
  
void load_batch(void)
{
  char tmp_filename[PATH_MAX];
	  FILE *stream;  
	  char line[255];

	  Msg("Loading .ini");	  
	  if (!exist("dink.ini")) 
		  	  {
	Msg("File not found.");	  
	  	
		  sprintf(line,"Error finding the dink.ini file in the %s dir.",dir);
		 TRACE(line);
		  
	  }

	  if( (stream = fopen(ciconvertbuf("dink.ini", tmp_filename), "r" )) != NULL )   
	  {
		  
		  while(1)
		  {
			  if( fgets( line, 255, stream ) == NULL) 
				  goto done;
			  else    
			  {
				  
				  pre_figure_out(line, 0);
			  }
			  
		  }
		  
done:
	
		      program_idata();

		  
		  fclose( stream );  
	  } else
	  {
		  TRACE("Dink.ini missing.");
	  }
	  
  }
  
  


  

  
int SInitSound()
{
  int i;

  if (!InitSound())
    return 0;

  /*
   * Load all sounds
   */
  for(i = 0; i < NUM_SOUND_EFFECTS; i++)
    {
      Msg("Loading sound %s [%d]", szSoundEffects[i], i);
      if (!CreateBufferFromWaveFile(szSoundEffects[i], i))
	Msg("cant load sound effect %s", szSoundEffects[i]);
    }
  return 1;
}


/*
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
static /*BOOL*/int doInit(int argc, char *argv[])
{
/* 	  HWND                hwnd; */
	  //    HRESULT             dsrval;
	  // BOOL                bUseDSound;
/* 	  WNDCLASS            wc; */
/* 	  DDSURFACEDESC       ddsd; */
/* 	  DDSCAPS             ddscaps; */
/* 	  HRESULT             ddrval; */
	  RECT                rcRect;
	  char crap[100];
	  char crap1[50];
	  RECT rcRectSrc;    RECT rcRectDest;
/* 	  POINT p; */
       char tdir[100];
       char tmp_filename[PATH_MAX];
	  /*
	  * set up and register window class
	  */
	  
	  //initFail(hwnd, "Couldn't make Back buffer in Windowed mode.");
dinkedit = true;

/* 	  wc.style = CS_HREDRAW | CS_VREDRAW; */
/* 	  wc.lpfnWndProc = WindowProc; */
/* 	  wc.cbClsExtra = 0; */
/* 	  wc.cbWndExtra = 0; */
/* 	  wc.hInstance = hInstance; */
/* 	  wc.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON1)); */
/* 	  wc.hCursor = LoadCursor( NULL, IDC_ARROW ); */
/* 	  wc.hbrBackground = GetStockBrush(BLACK_BRUSH); */
/* 	  wc.lpszMenuName = NAME; */
/* 	  wc.lpszClassName = NAME; */
/* 	  RegisterClass( &wc ); */
	  
	  /*
	  * create a window
	  */
	  
	  windowed = false;
	  check_arg(argc, argv);

/* 	  if (windowed) */
/* 	  { */
/* 		  hwnd = CreateWindowEx( */
/* 			  0, */
/* 			  NAME, */
/* 			  TITLE, */
/* 			  //        WS_POPUP, */
			  
/* 			  WS_SYSMENU|WS_CAPTION, */
			  
/* 			  0, */
/* 			  0, */
			  
/* 			  640+winoffsetx, 480+winoffset, */
/* 			  //        GetSystemMetrics(SM_CXSCREEN), */
/* 			  //      GetSystemMetrics(SM_CYSCREEN), */
/* 			  NULL, */
/* 			  NULL, */
/* 			  hInstance, */
/* 			  NULL ); */
/* 		  hWndMain = hwnd;    */
		  
/* 		  if( !hwnd ) */
/* 		  { */
/* 			  return FALSE; */
/* 		  } */
		  
		  
/* 		  ShowWindow( hwnd, nCmdShow ); */
/* 		  UpdateWindow( hwnd ); */
/* 		  SetFocus( hwnd ); */
		  
/* 		  /\* */
/* 		  * create the main DirectDraw object */
/* 		  *\/ */
/* 		  ddrval = DirectDrawCreate( NULL, &lpDD, NULL ); */
/* 		  if( ddrval != DD_OK ) */
/* 		  { */
/* 			  return initFail(hwnd, "Couldn't use DirectX 3+...  Install it first."); */
/* 		  } */
		  
/* 		  // Get exclusive mode */
		  
		  
/* 		  // using DDSCL_NORMAL means we will coexist with GDI */
/* 		  ddrval = lpDD->SetCooperativeLevel( hwnd, DDSCL_NORMAL ); */
		  
/* 		  if( ddrval != DD_OK )   */
/* 		  {         */
/* 			  lpDD->Release();  */
/* 			  return initFail(hwnd, "Couldn't make windowed screen."); */
			  
/* 		  }    */
/* 		  memset( &ddsd, 0, sizeof(ddsd) ); */
/* 		  ddsd.dwSize = sizeof( ddsd ); */
/* 		  ddsd.dwFlags = DDSD_CAPS; */
/* 		  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE; */
		  
/* 		  // The primary surface is not a page flipping surface this time */
/* 		  ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ); */
		  
/* 		  if( ddrval != DD_OK )     */
/* 		  {        lpDD->Release();   */
/* 		  return initFail(hwnd, "Couldn't make primary surface."); */
		  
		  
/* 		  }  */
		  
		  
		  
/* 		  memset( &ddsd, 0, sizeof(ddsd) );  */
/* 		  ddsd.dwSize = sizeof( ddsd ); */
/* 		  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; */
/* 		  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;  */
/* 		  ddsd.dwWidth = 640; */
/* 		  ddsd.dwHeight = 480;    // create the backbuffer separately */
/* 		  ddrval = lpDD->CreateSurface( &ddsd, &lpDDSBack, NULL ); */
/* 		  if( ddrval != DD_OK ) */
/* 		  {        lpClipper-> Release(); */
		  
/* 		  lpDDSPrimary->Release();     */
/* 		  lpDD->Release();  */
/* 		  return initFail(hwnd, "Couldn't make Back buffer in Windowed mode."); */
		  
		  
/* 		  }    */
		  

		  // Create a clipper to ensure that our drawing stays inside our window
/* 	  ddrval = lpDD->CreateClipper( 0, &lpClipper, NULL ); */
/* 	  if( ddrval != DD_OK )    */
/* 	  {       */
/* 		  lpDDSPrimary->Release(); */
		  
/* 		  lpDD->Release();   */
/* 		  return initFail(hwnd, "Couldn't make a Clipper object, god knows why."); */
		  
/* 	  } */
	  
	  
/* 	  // setting it to our hwnd gives the clipper the coordinates from our window */
/* 	  ddrval = lpClipper->SetHWnd( 0, hwnd );    */
/* 	  if( ddrval != DD_OK )   */
/* 	  { */
/* 		  lpClipper-> Release();    */
/* 		  lpDDSPrimary->Release(); */
		  
/* 		  lpDD->Release();      */
/* 		  return initFail(hwnd, "Couldn't give Clipper window cords."); */
		  
/* 	  } */
/* 	  // attach the clipper to the primary surface */
/* 	  ddrval = lpDDSPrimary->SetClipper( lpClipper );  */
/* 	  if( ddrval != DD_OK ) */
/* 	  {   */
/* 		  lpClipper-> Release();    */
/* 		  lpDDSPrimary->Release(); */
/* 		  lpDD->Release();     */
		  
/* 		  return initFail(hwnd, "Couldn't attach Clipper to primary buffer."); */
/* 	  } */
	  

/* 	  } */
	  
	  
	  
	  
/* 	  if (!windowed) */
/* 	  { */
		  
/* 		  hwnd = CreateWindowEx( */
/* 			  0, */
/* 			  NAME, */
/* 			  TITLE, */
/* 			  WS_POPUP, */
			  
/* 			  //WS_SYSMENU|WS_CAPTION, */
			  
/* 			  0, */
/* 			  0, */
/* 			  640, 480, */
/* 			  //        GetSystemMetrics(SM_CXSCREEN), */
/* 			  //      GetSystemMetrics(SM_CYSCREEN), */
/* 			  NULL, */
/* 			  NULL, */
/* 			  hInstance, */
/* 			  NULL ); */
/* 		  hWndMain = hwnd;    */
		  
/* 		  if( !hwnd ) */
/* 		  { */
/* 			  return FALSE; */
/* 		  } */
		  
/* 		  ShowWindow( hwnd, nCmdShow ); */
/* 		  UpdateWindow( hwnd ); */
/* 		  SetFocus( hwnd ); */
		  
/* 		  /\* */
/* 		  * create the main DirectDraw object */
/* 		  *\/ */
/* 		  ddrval = DirectDrawCreate( NULL, &lpDD, NULL ); */
/* 		  if( ddrval != DD_OK ) */
/* 		  { */
/* 			  return initFail(hwnd, "Couldn't use DirectX 3+...  Install it first."); */
/* 		  } */
/* //		  	  return initFail(hwnd, "Couldn't use DirectX 3+...  Install it first."); */
		  
/* 		  // Get exclusive mode */
		  
		  
/* 		  ddrval = lpDD->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ); */
		  
		  
/* 		  if( ddrval != DD_OK ) */
/* 		  { */
/* 			  return initFail(hwnd, "Whatup?  Couldn't set to full screen."); */
/* 		  } */
		  
/* 		  // Set the video mode to 640x480x8 */
/* 		  ddrval = lpDD->SetDisplayMode( x, y, 8); */
/* 		  if(ddrval != DD_OK) */
/* 		  { */
/* 			  return initFail(hwnd, "640 X 480, 8 bit not supported."); */
/* 		  } */
		  
/* 		  // Create the primary surface with 1 back buffer */
		  
/* 		  ddsd.dwSize = sizeof( ddsd ); */
/* 		  ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT; */
/* 		  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | */
/* 			  DDSCAPS_FLIP | */
/* 			  DDSCAPS_COMPLEX; */
		  
		  
		  
/* 		  ddsd.dwBackBufferCount = 1; */
/* 		  ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ); */
/* 		  if( ddrval != DD_OK ) */
/* 		  { */
/* 			  return initFail(hwnd, "Could not create primary surface."); */
/* 		  } */
		  
/* 		  ddscaps.dwCaps = DDSCAPS_BACKBUFFER; */
		  
/* 		  /\* if (ddsd.ddsCaps.dwCaps == DDCAPS_BLTSTRETCH) */
/* 		  { */
/* 		  return initFail(hwnd, "Hardware blit stretching available."); */
/* 		  } */
/* 		  *\/ */
/* 		  ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, &lpDDSBack); */
/* 		  if( ddrval != DD_OK ) */
/* 		  { */
/* 			  return initFail(hwnd, "Could not create backbuffer,"); */
/* 		  } */
		  
/* 	  } */
	  
	  
	  //done with major initting of graphics engine
	  

  /* New initialization */
  if (init() == 0)
    {
      exit(1);
    }

	  
  memset(&hm, 0, sizeof(hit_map));
		
	  
		//return initFail(hwnd, "CHEESEBURGERS RULE!");
	  
	  // create and set the palette

	  sprintf(tdir, "TILES\\ESPLASH.BMP");	  
     if (!exist(tdir))
	{
        sprintf(tdir, "..\\DINK\\TILES\\ESPLASH.BMP");	  
	
		
	}
	Msg("Checking %s", tdir);	

if (!exist(tdir))
	{
		return initFail("Did you enter a bad -game command?  Dir doesn't exist or is missing files.");
    }

/* 	  lpDDPal = DDLoadPalette(lpDD, tdir); */
	  // GFX
	  load_palette_from_bmp(tdir, GFX_real_pal);
	  
/* 	  if (lpDDPal) */
/* 	    { */
/* 	      lpDDSPrimary->SetPalette(lpDDPal); */
/* 	    } */
	  // Create the offscreen surface, by loading our bitmap.
	  srand( (unsigned)time( NULL ) );
/* 	  if(lpDDPal->GetEntries(0,0,256,real_pal)!=DD_OK) */
/*     { */
/*    Msg("error with getting entries in beginning"); */
/*     } */


/* 	  lpDDSTwo = DDLoadBitmap(lpDD, tdir, 0, 0); */
	  // GFX
	  GFX_lpDDSTwo = SDL_LoadBMP(ciconvert(tdir));
	  if (!GFX_lpDDSTwo)
	  {
	   return initFail("Couldn't load esplash.bmp.");

	  }
/* 	  DDSetColorKey(lpDDSTwo, RGB(0,0,0)); */
	  // GFX
	  // needed?

	  
	  rcRect.left = 0;
	  rcRect.top = 0;
	  rcRect.right = x;
	  rcRect.bottom = y;
	  
	  //if (lpDDSBack->GetBltStatus( DDGBS_ISBLTDONE) == DD_OK)
	  
	  // Display esplash screen
	  if (GFX_lpDDSTwo)
	  {
/* 	    ddrval = lpDDSBack->BltFast( 0, 0, lpDDSTwo, */
/* 					 &rcRect, DDBLTFAST_NOCOLORKEY); */
	    // GFX
	    SDL_BlitSurface(GFX_lpDDSTwo, NULL, GFX_lpDDSBack, NULL);
	  }


	  // GFX
	  {
	    change_screen_palette(GFX_real_pal);
	    
	    /* When a new image is loaded in DX, it's color-converted
	       using the main palette; currently we don't do that
	       (although that'd be more efficient that conversion each
	       time the original image is used). We work around this
	       by making the conversion happen at the first blit to a
	       buffer surface - and we never change the buffer's
	       palette again, so we're sure there isn't any conversion
	       even if we change the screen palette: */
	    SDL_SetPalette(GFX_lpDDSTwo, SDL_LOGPAL, cur_screen_palette, 0, 256);
	    SDL_SetPalette(GFX_lpDDSBack, SDL_LOGPAL, cur_screen_palette, 0, 256);
	    SDL_SetPalette(GFX_lpDDSPrimary, SDL_LOGPAL, cur_screen_palette, 0, 256);

	    /* TODO: wrap LoadBMP, and move buffer initialization
	       right after palette initialization */
	    SDL_Surface *splashscreen = NULL;
	    if (exist("tiles/esplash.BMP") &&
		(splashscreen = SDL_LoadBMP(ciconvertbuf("tiles/esplash.BMP", tmp_filename))) == NULL)
	      printf("Error loading tiles/splash.BMP: %s\n", SDL_GetError());
	    else if ((splashscreen = SDL_LoadBMP(ciconvertbuf("../dink/tiles/esplash.BMP", tmp_filename))) == NULL)
	      printf("Error loading tiles/splash.BMP: %s\n", SDL_GetError());
	    
	    if (splashscreen != NULL) {
	      SDL_BlitSurface(splashscreen, NULL, GFX_lpDDSTwo, NULL);
	      SDL_BlitSurface(splashscreen, NULL, GFX_lpDDSBack, NULL);
	      SDL_FreeSurface(splashscreen);
	    }
	  }

	  flip_it_second();

    load_batch();	



//game[15] = DDLoadBitmap(lpDD, "TILES\\SPLASH.BMP", 0, 0);
    //game[15] = DDSetColorKey(game[15], RGB(0,0,0));
load_hard();

	  

/*  for (int oo = 1; oo < 9; oo++)
  {
  sprintf(crap, "TILES\\S%d.BMP",oo);
  
     if (!exist(crap))  sprintf(crap, "..\\DINK\\TILES\\S%d.BMP",oo);
  
    k[oo].k = DDSethLoad(lpDD, crap, 0, 0,oo); 
	if( k[oo].k == NULL )
    {
		return initFail(hwnd, "Couldn't find a sprite.");
    }

  DDSetColorKey(k[oo].k, RGB(0,0,0));
   k[oo].yoffset = (k[oo].box.bottom  - ( k[oo].box.bottom  ));
         k[oo].xoffset = (k[oo].box.right - (k[oo].box.right ));


  }

*/

// Load the tiles from the BMPs
 load_tiles();
	
	for (int i=1; i <= 4; i++)
{
spr[i].active = /*FALSE*/0;
spr[i].x = 10;
spr[i].y = 10;
spr[i].my = (rand() % 3)+1;
spr[i].mx = (rand() % 3)+1;
spr[i].seq = 1;
spr[i].speed = (rand() % 40)+1;
spr[i].brain = 2;
spr[i].pseq = 10;
spr[i].pframe = 3;
spr[i].size = 100;
}

	// ** SETUP **
    spr[1].active = /*TRUE*/1;
	spr[1].x = 0;
    spr[1].y = 0;
    spr[1].speed = 20;
    spr[1].brain = 1;
   SetRect(&spr[1].alt,0,0,0,0);
    spr[1].pseq = 10;
    spr[1].pframe = 3;
	spr[1].seq = 0;
    spr[1].seq = 2;

    rcRect.left = 0;
    rcRect.top = 0;
    rcRect.right = 639;
    rcRect.bottom = 79;

	//lpDDSTwo->BltFast( 0, 400, game[1],
      //      &rcRect, DDBLTFAST_NOCOLORKEY );


    //sprite sequence setup
	seq[1].frame[1] = seq[10].frame[1];
    seq[1].frame[2] = seq[10].frame[2];
    seq[1].frame[3] = seq[10].frame[3];

    // FIX: end of sequence is 0, not -1. This made the editor crash.
    // seq[1].frame[4] = -1;
    seq[1].frame[4] = 0;

    seq[1].delay[1] = 50;
    seq[1].delay[2] = 50;
    seq[1].delay[3] = 50;
    seq[1].delay[4] = 50;
    
	seq[2].frame[1] = seq[10].frame[4];
    seq[2].frame[2] = seq[10].frame[4];
    seq[2].frame[3] = seq[10].frame[4];
    seq[2].frame[4] = 0;

	seq[2].delay[1] = 10;
    seq[2].delay[2] = 10;
    seq[2].delay[3] = 10;
    seq[2].delay[4] = 10;

	seq[3].frame[1] = seq[10].frame[5];
    seq[3].frame[2] = seq[10].frame[5];
    seq[3].frame[3] = seq[10].frame[5];
    seq[3].frame[4] = 0;

	seq[3].delay[1] = 5;
    seq[3].delay[2] = 5;
    seq[3].delay[3] = 5;
    seq[3].delay[4] = 5;
 
	seq[4].frame[1] = seq[10].frame[1];
    seq[4].frame[2] = seq[10].frame[1];
    seq[4].frame[3] = 0;
    seq[4].frame[4] = 0;

	seq[4].delay[1] = 2;
    seq[4].delay[2] = 2;
    seq[4].delay[3] = 2;
    seq[4].delay[4] = 2;
	
	

	if (sound_on) SInitSound();
	//Go Pap!!
	//	PlayMidi("sound\\TOP.MID");
    mode = MODE_DIALOG;
    cur_tile = 1; 
	load_info();        
/* initfonts("Arial"); */
  // FONTS
	FONTS_initfonts("../LiberationSans-Regular.ttf");
/*   FONTS_initfonts("C:/WINNT/FONTS/Arial.ttf"); */
  FONTS_SetFont(FONTS_hfont_small);

	
//	 Msg("Hi, you suck.");
	

	memset(&sjoy,0,sizeof(sjoy)); //clear key/joystick values


playl = 20;
playx = 620;	
playy = 480;
sp_seq = 0;

// init_mouse(hwnd);

	
// g_pMouse->Acquire();


	return /*TRUE*/1;

} /* doInit */

/* TODO: maybe use BinReloc, or something more portable (for *BSD?) */
#ifdef _WIN32
void getdir(char *dir, char *final)
{
	//converted to non CString version that spits back path + filename seperately.
	//Using	GetModuleFileName instead of ParamStr, works with Win2000/nt.
	char path[255];
	GetModuleFileName(NULL, path, 255);
	char c_cur = 0;

	for (int k=strlen(path);path[k] != '\\'; k--)
	{
	  c_cur = k;
	}
	strcpy(dir, "");
	//copy file name
	strncat(dir, &path[c_cur], strlen(path)-c_cur);
    path[c_cur] = 0; //truncate
	strcpy(final, path);
}
#endif



/* int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, */
/* 		   LPSTR lpCmdLine, int nCmdShow) */
int main(int argc, char *argv[])
{
/*   MSG msg; */
  
  char dir_temp[256], dir_final[256];
#ifdef _WIN32
  getdir(dir_temp, dir_final);
#else
  strcpy(dir_final, ".");
#endif
  
  strcpy(dinkpath, dir_final);
  printf("Switching to dir %s.",dinkpath);
  if (chdir(dinkpath)) 
    {
      char crap[255];
      sprintf(crap, "Dink Error: Couldn't change to dir %s.  Why?", dinkpath);
      initFail(crap);
      return(0);   
    }
  
/*   lpCmdLine = lpCmdLine; */
/*   command_line = lpCmdLine;  */
/*   MyhInstance = hInstance; */
/*   hPrevInstance = hPrevInstance; */
  
  if(!doInit(argc, argv))
    return /*FALSE*/0;
  
  
  while( 1 )
    {
      SDL_PumpEvents();
      updateFrame();
    }
  
  return(true); 
} /* WinMain */


/* Read key "is pressed?" status from cache */
bool
getkey(int key)
{
  if (sjoy.realkey[key])
    return(true);
  else
    return(false);      
}

/* Human-readable representation of the keycode, used to display which
   key is currently pressed */
/* Note: key constants may be found in winuser.h. The
   "Keycodes"/keycodes.txt file by Dan Walma also brings some clues
   about the _OEM keys. */
char
key_convert(int key)
{
  if (SDL_GetModState()&KMOD_SHIFT) key = toupper(key);
  
  if (key == 190 /* VK_OEM_PERIOD */) if (SDL_GetModState()&KMOD_SHIFT) key = '>'; else key = '.';
  if (key == 188 /* VK_OEM_COMMA */) if (SDL_GetModState()&KMOD_SHIFT) key = '<'; else key = ',';
  
  if (key == '1') if (SDL_GetModState()&KMOD_SHIFT) key = '!';
  if (key == '2') if (SDL_GetModState()&KMOD_SHIFT) key = '@';
  if (key == '3') if (SDL_GetModState()&KMOD_SHIFT) key = '#';
  if (key == '4') if (SDL_GetModState()&KMOD_SHIFT) key = '$';
  if (key == '5') if (SDL_GetModState()&KMOD_SHIFT) key = '%';
  if (key == '6') if (SDL_GetModState()&KMOD_SHIFT) key = '^';
  if (key == '7') if (SDL_GetModState()&KMOD_SHIFT) key = '&';
  if (key == '8') if (SDL_GetModState()&KMOD_SHIFT) key = '*'; 
  if (key == '9') if (SDL_GetModState()&KMOD_SHIFT) key = '('; 
  if (key == '0') if (SDL_GetModState()&KMOD_SHIFT) key = ')'; 
  
  if (key == 189 /* VK_OEM_MINUS */) if (SDL_GetModState()&KMOD_SHIFT) key = '_'; else key = '-';
  if (key == 187 /* VK_OEM_PLUS */) if (SDL_GetModState()&KMOD_SHIFT) key = '+'; else key = '=';
  if (key == 186 /* VK_OEM_1 */) if (SDL_GetModState()&KMOD_SHIFT) key = ':'; else key = ';';
  if (key == 222 /* VK_OEM_7 */) if (SDL_GetModState()&KMOD_SHIFT) key = '\"'; else key = '\'';
  if (key == 191 /* VK_OEM_2 */) if (SDL_GetModState()&KMOD_SHIFT) key = '?'; else key = '/';
  if (key == 220 /* VK_OEM_5 */) if (SDL_GetModState()&KMOD_SHIFT) key = '|'; else key = '\\';
  
  return(key); 
}
