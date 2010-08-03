/**
 * Draw background from tiles

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

#include <stdlib.h>
#include <string.h>
#include "game_engine.h"
#include "screen.h"
#include "dinkvar.h"
#include "gfx.h"
#include "gfx_tiles.h"
#include "io_util.h"
#include "paths.h"
#include "sfx.h"
#include "log.h"
#include "meminfo.h"

/* Tiles */
/* Game pieces */
/* +1 to avoid the -1 in array indexes.. */
SDL_Surface* gfx_tiles[GFX_TILES_NB_SETS+1];

/* Animated tiles current status */
static int water_timer = 0;
static int fire_flip = 0;


/* Local functions */


// Load the tiles from the BMPs
void tiles_load_default() {
  char crap[30];
  char crap1[10];
  int h;

  log_info("loading tilescreens...");
  for (h = 1; h <= GFX_TILES_NB_SETS; h++)
    {
      if (h < 10)
	strcpy(crap1,"0");
      else
	strcpy(crap1, "");      
      sprintf(crap, "tiles/Ts%s%d.BMP", crap1, h);

      tiles_load_slot(crap, h);

      if (gfx_tiles[h] == NULL)
	exit(0);
    }
  
  log_info("Done with tilescreens...");
}

void tiles_load_slot(char* relpath, int slot)
{
  FILE* in = paths_dmodfile_fopen(relpath, "rb");
  if (in == NULL)
    in = paths_fallbackfile_fopen(relpath, "rb");
  
  if (gfx_tiles[slot] != NULL)
    {
      SDL_FreeSurface(gfx_tiles[slot]);
      gfx_tiles[slot] = NULL;
    }

  gfx_tiles[slot] = load_bmp_from_fp(in);

  /* Note: attempting SDL_RLEACCEL showed no improvement for the
     memory usage, including when using a transparent color and
     blitting the surface once. It did show a decrease of 400kB (out
     of 6000kB) when using transparent color 255, but in this case the
     color is not supposed to be transparent. */

  if (gfx_tiles[slot] == NULL) {
    fprintf(stderr, "Couldn't find tilescreen %s: %s\n", relpath, SDL_GetError());
  }
}

/**
 * Free memory used by tiles
 */
void tiles_unload_all(void) {
  int h = 0;
  for (h=1; h <= GFX_TILES_NB_SETS; h++)
    {
      if (gfx_tiles[h] != NULL)
	SDL_FreeSurface(gfx_tiles[h]);
      gfx_tiles[h] = NULL;
    }
}

/**
 * Draw tile number 'dsttile_square_id0x' (in [0, 96-1]) in the
 * current screen
 */
void gfx_tiles_draw(int srctileset_idx0, int srctile_square_idx0, int dsttile_square_idx0)
{
  SDL_Rect src;
  int srctile_square_x = srctile_square_idx0 % GFX_TILES_SCREEN_W;
  int srctile_square_y = srctile_square_idx0 / GFX_TILES_SCREEN_W;
  src.x = srctile_square_x * GFX_TILES_SQUARE_SIZE;
  src.y = srctile_square_y * GFX_TILES_SQUARE_SIZE;
  src.w = GFX_TILES_SQUARE_SIZE;
  src.h = GFX_TILES_SQUARE_SIZE;
  
  int dsttile_x = dsttile_square_idx0 % GFX_TILES_SCREEN_W;
  int dsttile_y = dsttile_square_idx0 / GFX_TILES_SCREEN_W;
  SDL_Rect dst;
  dst.x = GFX_PLAY_LEFT + dsttile_x * GFX_TILES_SQUARE_SIZE;
  dst.y = GFX_PLAY_TOP  + dsttile_y * GFX_TILES_SQUARE_SIZE;

  SDL_BlitSurface(gfx_tiles[srctileset_idx0 + 1], &src, GFX_lpDDSTwo, &dst);
}

/**
 * Draw all background tiles in the current screen
 */
void gfx_tiles_draw_screen()
{
  int x = 0;
  for (; x < GFX_TILES_PER_SCREEN; x++)
    {
      int srctileset_idx0 = pam.t[x].square_full_idx0 / 128;
      int srctile_square_idx0 = pam.t[x].square_full_idx0 % 128;
      gfx_tiles_draw(srctileset_idx0, srctile_square_idx0, x);
    }
}

/* Draw the background from tiles */
void draw_map_game(void)
{
  *pvision = 0;
                
  while (kill_last_sprite());
  kill_repeat_sounds();
  kill_all_scripts();

  gfx_tiles_draw_screen();
                
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

  // lets add the sprites hardness to the real hardness, adding it's
  // own uniqueness to our collective.
  place_sprites_game();
  
  thisTickCount = game_GetTicks();
                
  // Run active sprites' scripts
  init_scripts();

  // Display some memory stats after loading a screen
  meminfo_log_mallinfo();
  gfx_log_meminfo();
  sfx_log_meminfo();
}
        
        
/* It's used at: freedink.cpp:restoreAll(), DinkC's draw_background(),
   stop_entire_game(). What's the difference with draw_map_game()?? */
void draw_map_game_background(void)
{
  gfx_tiles_draw_screen();
  place_sprites_game_background();
}
        
/* Game-specific: animate background (water, fire, ...) */        
void process_animated_tiles(void)
{
  int flip;
	
  // Water:
  if (water_timer < thisTickCount)
    {

      water_timer = thisTickCount + (rand() % 2000);
      flip = rand() % 2;
		
      int x = 0;
      for (; x < 96; x++)
	{
	  int screen_square_full_idx0 = pam.t[x].square_full_idx0;
	  int start_full_idx0 = (8-1) * 128; // 8th tileset -> 896
	  if (screen_square_full_idx0 >= start_full_idx0
	      && screen_square_full_idx0 < (start_full_idx0 + 128))
	    gfx_tiles_draw((8-1) + flip, screen_square_full_idx0 % 128, x);
	}
    }
	
	
  // Fire:
  {
    fire_flip--;
    if (fire_flip < 0)
      fire_flip = 4;
		
    int x = 0;
    for (; x < 96; x++)
      {
	int screen_square_full_idx0 = pam.t[x].square_full_idx0;
	int start_full_idx0 = (19-1) * 128; // 19th tileset -> 2304
	if (screen_square_full_idx0 >= start_full_idx0
	    && screen_square_full_idx0 < (start_full_idx0 + 128))
	  gfx_tiles_draw((19-1) + fire_flip, screen_square_full_idx0 % 128, x);
      }
  }
}
