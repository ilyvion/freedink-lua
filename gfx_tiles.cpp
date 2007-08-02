/**
 * Draw background from tiles

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2007  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with program; see the file COPYING. If not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "dinkvar.h"
#include "gfx.h"
#include "gfx_tiles.h"
#include "sfx.h"

/* Tiles */
LPDIRECTDRAWSURFACE     tiles[tile_screens];       // Game pieces
SDL_Surface             *GFX_tiles[tile_screens];   // Game pieces (SDL)

/* Animated tiles current status */
int water_timer;
bool fire_forward;
int fire_flip;


/* Draw the background from tiles */
void draw_map_game(void)
{
  RECT                rcRect;
  int pa, cool;
                
  *pvision = 0;
                
  while (kill_last_sprite());
  kill_repeat_sounds();
  kill_all_scripts();

  /* 96 = 12 * 8 tiles; 1 tile = 50x50 pixels */
  for (int x=0; x<96; x++)
    {
      cool = pam.t[x].num / 128;
      pa = pam.t[x].num - (cool * 128);

      rcRect.left = (pa * 50- (pa / 12) * 600);
      rcRect.top = (pa / 12) * 50;
      rcRect.right = rcRect.left + 50;
      rcRect.bottom = rcRect.top + 50;
      lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+1],
			 &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT );
                        
      // GFX
      {
	SDL_Rect src;
	SDL_Rect dst;
	src.x = (pa * 50- (pa / 12) * 600);
	src.y = (pa / 12) * 50;
	src.w = 50;
	src.h = 50;
	dst.x = (x * 50 - ((x / 12) * 600))+playl;
	dst.y = (x / 12) * 50, tiles[cool+1];
	SDL_BlitSurface(GFX_tiles[cool+1], &src, GFX_lpDDSTwo, &dst);
      }
    }
                
                
  if (strlen(pam.script) > 1)
    {
      int ms = load_script(pam.script,0, true);
                        
      if (ms > 0) 
	{
	  locate(ms, "main");
	  no_running_main = true;
	  run_script(ms);
	  no_running_main = false;
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
  RECT rcRect;
  int pa, cool;
                
  for (int x=0; x<96; x++)
    {
      cool = pam.t[x].num / 128;
      pa = pam.t[x].num - (cool * 128);

      rcRect.left = (pa * 50- (pa / 12) * 600);
      rcRect.top = (pa / 12) * 50;
      rcRect.right = rcRect.left + 50;
      rcRect.bottom = rcRect.top + 50;
      lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+1],
			 &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT );

      // GFX
      {
	SDL_Rect src;
	SDL_Rect dst;
	src.x = (pa * 50- (pa / 12) * 600);
	src.y = (pa / 12) * 50;
	src.w = 50;
	src.h = 50;
	dst.x = (x * 50 - ((x / 12) * 600))+playl;
	dst.y = (x / 12) * 50, tiles[cool+1];
	SDL_BlitSurface(GFX_tiles[cool+1], &src, GFX_lpDDSTwo, &dst);
      }
                        
    }
                
  place_sprites_game_background();
}
        
/* Game-specific: animate background (water, fire, ...) */        
void process_animated_tiles(void)
{
  RECT rcRect;
  int cool;
  int flip;
  int pa;
	
  //process water tiles
	
  if (water_timer < thisTickCount)
    {
		
      water_timer = thisTickCount + ((rand() % 2000));
		
      flip = ((rand() % 2)+1);		
		
		
		
		
      for (int x=0; x<96; x++)
	{
	  if (pam.t[x].num > 896) if (pam.t[x].num < (896+128))
				    {
				
				      cool = pam.t[x].num / 128;
				      pa = pam.t[x].num - (cool * 128);
				      rcRect.left = (pa * 50- (pa / 12) * 600);
				      rcRect.top = (pa / 12) * 50;
				      rcRect.right = rcRect.left + 50;
				      rcRect.bottom = rcRect.top + 50;
				
				
				      lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+flip],
							 &rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT );

				      // GFX
				      {
					SDL_Rect src;
					SDL_Rect dst;
					src.x = (pa * 50- (pa / 12) * 600);
					src.y = (pa / 12) * 50;
					src.w = 50;
					src.h = 50;
					dst.x = (x * 50 - ((x / 12) * 600))+playl;
					dst.y = (x / 12) * 50, tiles[cool+flip];
					SDL_BlitSurface(GFX_tiles[cool+flip], &src, GFX_lpDDSTwo, &dst);
				      }
				    }	
	}
		
    }
	
  //end of water processing
	
	
  //if (water_timer < thisTickCount)
  {
		
    //	water_timer = thisTickCount + ((rand() % 2000)+1000);
		
    if (fire_forward) fire_flip++;
    if (!fire_forward) fire_flip--;
		
    if (fire_flip < 1)
      {
	fire_flip = 5;
	fire_forward = false;
      }
		
    //	if (fire_flip > 4)
    //	{
    //  fire_flip = 4;
    //fire_forward = false;
    //}
		
		
		
    for (int x=0; x<96; x++)
      {
	if (pam.t[x].num > 2304) if (pam.t[x].num < (2304+128))
				   {
				
				     cool = pam.t[x].num / 128;
				     pa = pam.t[x].num - (cool * 128);
				     rcRect.left = (pa * 50- (pa / 12) * 600);
				     rcRect.top = (pa / 12) * 50;
				     rcRect.right = rcRect.left + 50;
				     rcRect.bottom = rcRect.top + 50;
				
				
				     lpDDSTwo->BltFast( (x * 50 - ((x / 12) * 600))+playl, (x / 12) * 50, tiles[cool+fire_flip],
							&rcRect, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT );
				
				     // GFX
				     {
				       SDL_Rect src;
				       SDL_Rect dst;
				       src.x = (pa * 50- (pa / 12) * 600);
				       src.y = (pa / 12) * 50;
				       src.w = 50;
				       src.h = 50;
				       dst.x = (x * 50 - ((x / 12) * 600))+playl;
				       dst.y = (x / 12) * 50, tiles[cool+fire_flip];
				       SDL_BlitSurface(GFX_tiles[cool+fire_flip], &src, GFX_lpDDSTwo, &dst);
				     }
				   }	
      }
		
  }
	
  //end of water processing
}
