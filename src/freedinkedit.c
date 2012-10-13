/**
 * FreeDink editor-specific code

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2011, 2012  Sylvain Beucler

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

#include "progname.h"

/* #define WIN32_LEAN_AND_MEAN */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* for tolower */
#include <ctype.h>
/* #include <io.h> */
/* #include <direct.h> */
#include <unistd.h>
#ifdef _WIN32
/* GetModuleFileName */
#include <windows.h>
#endif
#include <getopt.h>

#include "SDL.h"
#include "SDL_rotozoom.h"
#include "SDL_framerate.h"

#include "init.h"
#include "dinkvar.h"
#include "screen.h"
#include "fastfile.h"
#include "gfx.h"
#include "game_engine.h"
#include "screen.h"
#include "gfx_sprites.h"
#include "gfx_tiles.h"
#include "gfx_fonts.h"
#include "sfx.h"
#include "input.h"
#include "io_util.h"
#include "paths.h"
#include "log.h"

//Dinkedit-only vars


/*
 * I count 10 modes in Dinkedit: map(1), screen tiles(3), screen
 * sprites(6), screen hardness initialization (9, switches to 8
 * immediately), screen hardness(8), tile hardness(4), sprite
 * chooser(5), tile chooser(2), sprite hardness editor(7), plus input
 * dialog(0). */
/* TODO: The goal is to split the big keybinding functions into these
 * modes, and in each mode, call a function instead of inlining the
 * code. And we may use 'else if', or even a hashmap to do the
 * bindings.
*/

/* Use constants for readability */
#define MODE_MINIMAP 1
#define MODE_TILE_PICKER 2
#define MODE_SPRITE_PICKER 5

#define MODE_SCREEN_TILES 3
#define MODE_SCREEN_SPRITES 6
#define MODE_SCREEN_HARDNESS 8
#define MODE_SCREEN_HARDNESS_INIT 9

#define MODE_TILE_HARDNESS 4
#define MODE_SPRITE_HARDNESS 7

#define MODE_DIALOG 0


#define INPUT_MINIMAP_LOAD  30
#define INPUT_SCREEN_VISION 32
#define INPUT_SCREEN_MIDI   33
#define INPUT_SCREEN_TYPE   34

enum editor_buttons {
  EDITOR_ACTION_FIRST  = 0, // min constant for loops, like SDLKey
  EDITOR_ACTION_NOOP = 0,
  EDITOR_ACTION_ESCAPE = 1,
  EDITOR_ACTION_RETURN,
  EDITOR_ACTION_X,
  EDITOR_ACTION_Z,
  EDITOR_ACTION_TAB,
  EDITOR_ACTION_LAST // max+1 constant for loops
};

struct map_info buffmap;
/*bool*/int buf_mode = /*false*/0;
static char buf_path[100];
static int buf_map = 0;

/* Save x and y coordinates for mode 4, 5 and 6 */
static int m4x,m4y,m5x,m5y,m6x,m6y,m5ax,m5ay;
/* LPDIRECTDRAWCLIPPER lpClipper; */

static int sp_base_walk = -1;
static int sp_base_idle = -1;
static int sp_base_attack = -1;
static int sp_base_hit = -1;
static int sp_base_die = -1;
static int sp_gold, sp_defense, sp_strength, sp_exp, sp_hitpoints;
static int sp_timer = 33;
static int sp_que;
static int sp_hard = 1;
static int sp_sound = 0;
static int sp_type = 1;
static int sp_is_warp = 0;
static int sp_warp_map = 0;
static /*bool*/int show_display = /*true*/1;
static int sp_picker = 0;
static int sp_nohit = 0;
static int sp_touch_damage = 0;
static int sp_warp_x = 0;
static int sp_warp_y = 0;
static int sp_parm_seq = 0;
static char sp_script[15];

static int old_command;
static int sp_cycle = 0;
static   int cur_screen;
static int selx = 1;
static int sely = 1;
static int last_mode = 0;
static int last_modereal = 0;
static int hold_warp_map, hold_warp_x,hold_warp_y;
static int sp_seq,sp_frame = 0;


/* const int NUM_SOUND_EFFECTS = 6; */
#define NUM_SOUND_EFFECTS 2

typedef enum enum_EFFECT
{
    SOUND_STOP = 0,
/*     SOUND_THROW, */
    SOUND_JUMP,
/*     SOUND_STUNNED, */
/*     SOUND_BEARSTRIKE, */
/*     SOUND_BEARMISS, */
} EFFECT;

static char *szSoundEffects[NUM_SOUND_EFFECTS] =
{
    "stop.wav",
/*     "THROW.WAV", */
    "jump.wav"
/*     "STUNNED.WAV", */
/*     "STRIKE02.WAV", */
/*     "MISS02.WAV" */
};


static int x = 640;
static int y = 480;

static char in_temp[200];
static int in_command;
static int in_onflag;
static int in_max = 10;
static int in_huh = 0;
static char in_default[200];
static int in_master = 0;

static int *in_int;
static char *in_string;
static int in_x, in_y;
static /*bool*/int in_enabled;

static int sp_mode;
static int sp_speed = 0;
static int sp_brain = 0;
static /*bool*/int sp_screenmatch = 0;
static int hard_tile = 0;
static int last_sprite_added = 0;
static int map_vision = 0;

static int m1x,m1y;
static int m2x,m2y;
static int m3x,m3y;
static int cur_tile;

/**
 * Get the screen tile under coords x,y
 */
int xy2screentile(int x, int y)
{
  return
    ((y + 1) * 12) / 50
    + x / 50;
}



// PROC NAMES

/*BOOL*/int initFail(char mess[200] );

int getkeystate(int key);
int getcharstate(Uint16 character);
char key_convert(int key);

void draw_map(void);
void draw_minimap(void);
/* void dderror(HRESULT hErr); */


void draw_sprite(SDL_Surface *GFX_lpdest, int h)
{
  rect box_crap,box_real;
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
	  {
	    SDL_Rect src, dst;
	    src.x = box_real.left;
	    src.y = box_real.top;
	    src.w = box_real.right - box_real.left;
	    src.h = box_real.bottom - box_real.top;
	    dst.x = box_crap.left;
	    dst.y = box_crap.top;
	    dst.w = box_crap.right - box_crap.left;
	    dst.h = box_crap.bottom - box_crap.top;
	    gfx_blit_stretch(GFX_k[getpic(h)].k, &src, GFX_lpdest, &dst);
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


/**
 * Draw all screen sprites, ordered by queue.
 * 
 * Also cf. game's place_sprites_game(...) and
 * place_sprites_game_background(...).
 */
void place_sprites()
{
  int rank[MAX_SPRITES_EDITOR];
  screen_rank_map_sprites(rank);
  
  int r1 = 0;
  for (; r1 < MAX_SPRITES_EDITOR && rank[r1] > 0; r1++)
    {
      //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
      int j = rank[r1];
      
      if (pam.sprite[j].active == 1
	  && (pam.sprite[j].vision == 0 || pam.sprite[j].vision == map_vision))
	{
	  //we have instructions to make a sprite
	  
	  if (pam.sprite[j].type == 0 || pam.sprite[j].type == 2)
	    {
	      //make it part of the background (much faster)
	      int sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y, 0,
					   pam.sprite[j].seq,pam.sprite[j].frame,
					   pam.sprite[j].size);

	      spr[sprite].hard = pam.sprite[j].hard;
	      check_sprite_status(sprite);
	      spr[sprite].sp_index = j;
	      rect_copy(&spr[sprite].alt , &pam.sprite[j].alt);
	      
	      if (pam.sprite[j].type == 0)
		draw_sprite(GFX_lpDDSTwo, sprite);
	      
	      if (spr[sprite].hard == 0)
		{
		  if (pam.sprite[j].is_warp == 0)
		    add_hardness(sprite, 1);
		  else add_hardness(sprite, 100 + j);
		}
	      
	      spr[sprite].active = 0;
	    }
	  
	  if (pam.sprite[j].type == 1)
	    {
	      //make it a living sprite
	      
	      int sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y, 0,
					   pam.sprite[j].seq,pam.sprite[j].frame,
					   pam.sprite[j].size);
	      
	      spr[sprite].que = pam.sprite[j].que;
	      check_sprite_status(sprite);
	      spr[sprite].hard = pam.sprite[j].hard;
	      
	      rect_copy(&spr[sprite].alt , &pam.sprite[j].alt);
	      
	      if (spr[sprite].hard == 0)
		{
		  if (pam.sprite[j].is_warp == 0)
		    add_hardness(sprite, 1); else add_hardness(sprite,100+j);
		}
	    }
	}
    }
}



/* Draw background from tiles */
void draw_map(void)
{
  fill_screen(0);
  fill_whole_hard();

  while (kill_last_sprite());

  gfx_tiles_draw_screen();
  place_sprites();
}




/**
 * Draw the currently selected tile square (in the bottom-right corner
 * of the screen)
 */
void draw_current()
{
  int srctileset_idx0 = cur_tile / 128;
  int srctile_square_idx0 = cur_tile % 128;

  SDL_Rect src;
  int srctile_square_x = srctile_square_idx0 % GFX_TILES_SCREEN_W;
  int srctile_square_y = srctile_square_idx0 / GFX_TILES_SCREEN_W;
  src.x = srctile_square_x * GFX_TILES_SQUARE_SIZE;
  src.y = srctile_square_y * GFX_TILES_SQUARE_SIZE;
  src.w = GFX_TILES_SQUARE_SIZE;
  src.h = GFX_TILES_SQUARE_SIZE;
  
  SDL_Rect dst = {590, 430};
  SDL_BlitSurface(gfx_tiles[srctileset_idx0 + 1], &src, GFX_lpDDSTwo, &dst);
}

/* Edit a tile hardness - show semi-transparent red/blue/orange
   squares on pixels with different kinds of hardness */
void draw_hard( void)
{
 //RECT                rcRect;
  int x;
  for (x = 0; x < 50; x++)
    {
      int y;
      for (y = 0; y < 50; y++)
	{
	  /* red */
	  if (hmap.htile[hard_tile].x[x].y[y] == 1)
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
	  if (hmap.htile[hard_tile].x[x].y[y] == 2)
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
	  if (hmap.htile[hard_tile].x[x].y[y] == 3)
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
draw_this_map(struct map_info* pmap)
{
  int x;
  for (x = 0; x < 768; x++)
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
	    gfx_blit_nocolorkey(sprite, NULL, GFX_lpDDSTwo, &dst);
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
	    gfx_blit_nocolorkey(sprite, NULL, GFX_lpDDSTwo, &dst);
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
void draw_minimap(void)
{
  /*box_crap.top = spr[h].y + k[spr[h].pic].hardbox.top;
    box_crap.bottom = spr[h].y + k[spr[h].pic].hardbox.bottom;
    box_crap.left = spr[h].x + k[spr[h].pic].hardbox.left;
    box_crap.right = spr[h].x + k[spr[h].pic].hardbox.right;
  */
  //lpDDSBack->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL, &ddbltfx);
  draw_this_map(&map);
}




/*bool*/int load_map_buf(const int num)
{
  char crap[120];
  /* TODO: Dinkedit historically loads map with a filename relative to
     the current D-Mod directory. Maybe change that to handle absolute
     paths and paths relative to the refdir. */
  sprintf(crap, "%sMAP.DAT", buf_path);
  load_map_to(crap, num, &pam);
  
  return /*true*/1;
}

void load_info_buff(void)
{
  char crap[120];

  sprintf(crap, "%sDINK.DAT", buf_path);

  if (load_info_to(crap, &buffmap) < 0)
    {
      log_error("World not found in %s.", buf_path);
      buf_mode = /*false*/0;
    }
  else
    {
      log_info("World data loaded.");
      buf_mode = /*true*/1;
    }
}

/* draw_minimap() but on a different map ('L' in map picker mode) */
void draw_minimap_buff(void)
{
  load_info_buff();

  if (!buf_mode)
    {
      //failed
      /* TODO: display error message to the user */
      draw_minimap();
      return;
    }

  draw_this_map(&buffmap);
}


int add_new_map(void)
{
  FILE *fp;
  long now;

  fp = paths_dmodfile_fopen("MAP.DAT", "a+b");
  if (fp == NULL)
    {
      perror("Cannot open MAP.DAT");
      return -1;
    }
  fwrite(&pam,sizeof(struct small_map), 1, fp);
  now = (ftell(fp) / (sizeof(struct small_map)));
  fclose(fp);

  return(now);
}




/* Read key "is pressed?" status from cache */
int
getkeystate(int key)
{
  if (sjoy.keystate[key])
    return 1;
  else
    return 0;
}

/* Read key "is pressed?" status from cache - but with a character
   code*/
int
getcharstate(Uint16 character)
{
  if (sjoy.charstate[character])
    return 1;
  else
    return 0;
}

/* Set the keyboard state in sjoy */
void check_keyboard()
{
  /* Keys state */
  /* Check if the key was just pressed, or only maintained pressed */
  int keystate_size;
  Uint8 *new_keystate;
  new_keystate = SDL_GetKeyState(&keystate_size);
  int x = 0;
  for (x = 0; x < keystate_size; x++)
    {
      /* Put the current keyboard state in cache */
      /* getkey() then can check sjoy.keystate - that is, from the cache */
      int last_state = sjoy.keystate[x];
      sjoy.keystate[x] = new_keystate[x];

      if (last_state == 0 && sjoy.keystate[x] == 1)
	/* We just changed from "released" to "pressed" */
	sjoy.keyjustpressed[x] = 1;
      else
	sjoy.keyjustpressed[x] = 0;
    }

  /* Layout-independant character state */
  /* Reset 'just pressed' field */
  memset(sjoy.charjustpressed, 0, sizeof(sjoy.charjustpressed));
  sjoy.last_unicode = 0;
  if (sjoy.last_nokey_unicode != 0)
    {
      sjoy.charstate[sjoy.last_nokey_unicode] = 0;
      sjoy.charjustpressed[sjoy.last_nokey_unicode] = 0;
      sjoy.last_nokey_unicode = 0;
    }

  /* Pick ONE event, so as not to miss keystorkes in text input
     mode */
  SDL_Event event;
  if (SDL_PeepEvents(&event, 1, SDL_GETEVENT,
		     SDL_EVENTMASK(SDL_KEYDOWN)|SDL_EVENTMASK(SDL_KEYUP)) > 0)
    {
      int key = event.key.keysym.sym;
      Uint16 unicode = event.key.keysym.unicode;
      switch (event.type)
	{
	case SDL_KEYDOWN:
	  sjoy.charstate[unicode] = 1;
	  sjoy.charjustpressed[unicode] = 1;
	  sjoy.last_unicode = unicode;
	  if (key != 0)
	    sjoy.key2char[key] = unicode;
	  else
	    /* No possible 'kept pressed' support, so will be marked
	       'not pressed' next time */
	    sjoy.last_nokey_unicode = unicode;
	  break;
	case SDL_KEYUP:
	  /* No Unicode on KEYUP :/ */
	  if (key != 0)
	    {
	      unicode = sjoy.key2char[key];
	      sjoy.charstate[unicode] = 0;
	    }
	  break;
	}
    }
}


void check_joystick(void)
{
  /* Clean-up */
  /* Actions */
  {
    int a = EDITOR_ACTION_FIRST;
    for (a = EDITOR_ACTION_FIRST; a < EDITOR_ACTION_LAST; a++)
      sjoy.joybit[a] = 0;
  }

  /* Arrows */
  sjoy.right = 0;
  sjoy.left = 0;
  sjoy.up = 0;
  sjoy.down = 0;

  if (joystick)
    {
      SDL_JoystickUpdate();
      Sint16 x_pos = 0, y_pos = 0;
      /* SDL counts buttons from 0, not from 1 */
      int i = 0;
      for (i = 0; i < NB_BUTTONS; i++)
	if (SDL_JoystickGetButton(jinfo, i))
	  {
	    if (i == 1-1)
	      sjoy.joybit[EDITOR_ACTION_ESCAPE] = 1;
	    else if (i == 2-1)
	      sjoy.joybit[EDITOR_ACTION_RETURN] = 1;
	    else if (i == 3-1)
	      sjoy.joybit[EDITOR_ACTION_X] = 1;
	    else if (i == 4-1)
	      sjoy.joybit[EDITOR_ACTION_Z] = 1;
	    else if (i == 5-1)
	      sjoy.joybit[EDITOR_ACTION_TAB] = 1;
	  }

      x_pos = SDL_JoystickGetAxis(jinfo, 0);
      y_pos = SDL_JoystickGetAxis(jinfo, 1);
      /* Using thresold=10% (original game) is just enough to get rid
	 of the noise. Let's use 30% instead, otherwise Dink will go
	 diags too easily. */
      {
	Sint16 threshold = 32767 * 30/100;
	if (x_pos < -threshold) sjoy.left  = 1;
	if (x_pos > +threshold) sjoy.right = 1;
	if (y_pos < -threshold) sjoy.up    = 1;
	if (y_pos > +threshold) sjoy.down  = 1;
      }
    }

  /* Refresh keyboard state */
  check_keyboard();

  if (getkeystate(SDLK_ESCAPE)) sjoy.joybit[EDITOR_ACTION_ESCAPE] = 1;
  if (getkeystate(SDLK_RETURN)) sjoy.joybit[EDITOR_ACTION_RETURN] = 1;
  if (getcharstate('x')) sjoy.joybit[EDITOR_ACTION_X] = 1;
  if (getcharstate('z')) sjoy.joybit[EDITOR_ACTION_Z] = 1;
  if (getkeystate(SDLK_TAB)) sjoy.joybit[EDITOR_ACTION_TAB] = 1;
  if (getkeystate(SDLK_RIGHT)) sjoy.right = 1;
  if (getkeystate(SDLK_LEFT)) sjoy.left = 1;
  if (getkeystate(SDLK_DOWN)) sjoy.down = 1;
  if (getkeystate(SDLK_UP)) sjoy.up = 1;
  
  {
    int a = EDITOR_ACTION_FIRST;
    for (a = EDITOR_ACTION_FIRST; a < EDITOR_ACTION_LAST; a++)
      {
	sjoy.button[a] = 0;
	if (sjoy.joybit[a] && sjoy.joybitold[a] == 0)
	  /* Button was just pressed */
	  sjoy.button[a] = 1;
	sjoy.joybitold[a] = sjoy.joybit[a];
      }
  }
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

  if (key == 190 /* VK_OEM_PERIOD */) { if (SDL_GetModState()&KMOD_SHIFT) key = '>'; else key = '.'; }
  if (key == 188 /* VK_OEM_COMMA */)  { if (SDL_GetModState()&KMOD_SHIFT) key = '<'; else key = ','; }

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

  if (key == 189 /* VK_OEM_MINUS */) { if (SDL_GetModState()&KMOD_SHIFT) key = '_'; else key = '-'; }
  if (key == 187 /* VK_OEM_PLUS */) { if (SDL_GetModState()&KMOD_SHIFT) key = '+'; else key = '='; }
  if (key == 186 /* VK_OEM_1 */) { if (SDL_GetModState()&KMOD_SHIFT) key = ':'; else key = ';'; }
  if (key == 222 /* VK_OEM_7 */) { if (SDL_GetModState()&KMOD_SHIFT) key = '\"'; else key = '\''; }
  if (key == 191 /* VK_OEM_2 */) { if (SDL_GetModState()&KMOD_SHIFT) key = '?'; else key = '/'; }
  if (key == 220 /* VK_OEM_5 */) { if (SDL_GetModState()&KMOD_SHIFT) key = '|'; else key = '\\'; }

  return(key);
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
  SDL_FillRect(GFX_lpDDSTwo, NULL, SDL_MapRGB(GFX_lpDDSTwo->format, 0, 0, 0));

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
  SDL_BlitSurface(gfx_tiles[tileset], NULL, GFX_lpDDSTwo, NULL);
  cur_screen = tileset;

  last_mode = tileset;

  while(kill_last_sprite());
}


int sp_get( int num)
{

int t = 1;
 int j;
	for (j = 1; j < MAX_SEQUENCES; j++)
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
  int x2, x3;
  for (x2 = 1; x2 <= 12; x2++)
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
	SDL_FillRect(GFX_lpDDSTwo, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 123, 132, 99));
      }
    }

  /* Draw horizontal lines */
  for (x3 = 1; x3 <= 8; x3++)
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
	SDL_FillRect(GFX_lpDDSTwo, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 123, 132, 99));
      }
    }
}

/* Main sprite selection screen - display a page of the sequence
   selector */
void draw15(int num)
{
/*   int crap; */
/*   DDBLTFX ddbltfx; */
/*   rect  crapRec, Rect, box_crap; */
  int frame;
/*   int ddrval; */
  int se;
/*   int dd; */
  int x1;
  //get_sp_seq(2);

  while(kill_last_sprite());


/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
/*   ddbltfx.dwFillColor = 0; */
/*   crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
  // GFX
  SDL_FillRect(GFX_lpDDSTwo, NULL, SDL_MapRGB(GFX_lpDDSTwo->format, 0, 0, 0));

/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */

  Say("Please wait, loading sprite data into SmartCache system...", 147,160);

  flip_it();

  /* Draw sprites */
  for (x1 = 0; x1 <= 11; x1++)
    {
      int y1;
      for (y1=0; y1 <= 7; y1++)
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
	      {
		SDL_Rect dst;
		dst.x = x1 * 50;
		dst.y = y1 * 50;
		dst.w = 50;
		dst.h = 50;
		gfx_blit_stretch(GFX_k[seq[se].frame[frame]].k, NULL, GFX_lpDDSTwo, &dst);
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
/*   int crap; */
/*   DDBLTFX ddbltfx; */
/*   rect crapRec, Rect, box_crap; */
/*   int ddrval; */
  int se;
/*   int dd; */
  int x1;

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
  SDL_FillRect(GFX_lpDDSTwo, NULL, SDL_MapRGB(GFX_lpDDSTwo->format, 0, 0, 0));

/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */

  for (x1=0; x1 <= 11; x1++)
    {
      int y1;
      for (y1=0; y1 <= 7; y1++)
	{
	  num++;
	  if (seq[se].frame[num] == 0)
	    {
	      //all done displaying
	      goto pass;
	    }
	  //se = sp_get(num);

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
	  {
	    SDL_Rect dst;
	    dst.x = x1 * 50;
	    dst.y = y1 * 50;
	    dst.w = 50;
	    dst.h = 50;
	    gfx_blit_stretch(GFX_k[seq[se].frame[num]].k, NULL, GFX_lpDDSTwo, &dst);
	  }

/* 	  if (dd != DD_OK) Msg("Error with drawing sprite! Seq %d, Spr %d.",se,frame); */
	}
    }

 pass:
  draw_sprite_picker_grid();
}


void sp_add( void )
{
  int j;
	for (j = 1; j < 100; j++)
	{
		if (pam.sprite[j].active == /*false*/0)
		{

			last_sprite_added = j;
			//Msg("Adding sprite %d, seq %d, frame %d.",j,sp_seq,sp_frame);
			pam.sprite[j].active = /*true*/1;
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
			pam.sprite[j].is_warp = sp_is_warp;
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
			rect_copy(&pam.sprite[j].alt , &spr[1].alt);
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
	log_debug("need to add it to the right");

		//need to add it to the right
		if (map.loc[cur_map+1] > 0)
		{
        	save_map(map.loc[cur_map]);
          log_debug("loading right map");
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
	log_debug("need to add it to the right");

		//need to add it to the left
		if (map.loc[cur_map-1] > 0)
		{
        	save_map(map.loc[cur_map]);
          log_debug("loading right map");
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
	log_debug("need to add it to the bottom");

		//need to add it to the bottom
		if (map.loc[cur_map+32] > 0)
		{
        	save_map(map.loc[cur_map]);
          log_debug("loading bottom ");
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
	log_debug("need to add it to the top");

		//need to add it to the left
		if (map.loc[cur_map-32] > 0)
		{
        	save_map(map.loc[cur_map]);
          log_debug("loading top map");
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


/**
 * Display the appropriate input dialog
 */
void check_in(void)
{
  in_huh = in_master;

  /**
   * - Sprite dialogs:
   *  1 = size
   *  2 = type (background|sprite)
   *  3 = brain
   *  4 = speed
   *  5 = timer (timing)
   *  6 = base_walk
   *  7 = base_idle
   *  8 = que
   *  9 = hard (hardness_type - background|sprite)
   * 10 = is_warp
   * 11 = warp_map
   * 12 = warp_x
   * 13 = warp_y
   * 14 = parm_seq
   * 15 = script
   * 16 = base_die
   * 17 = sound (looping sound)
   * 18 = hitpoints
   * 19 = nohit
   * 20 = touch_damage
   * 21 = base_attack
   * 22 = defense
   * 
   * - Minimap dialogs:
   * 30 = 'L' - load screen from another map
   * 
   * - Screen dialogs:
   * (called from minimap or screen mode)
   * 31 = 'B' - screen script
   * 32 = 'V' - vision
   * (called from minimap mod)
   * 33 = 'M' - screen midi
   * 34 = 'S' - screen type (indoors / outside)
   **/

  /* In: */
  /* in_master = identifier for the property we need to edit */
  /* Out: */
  /* in_max = maximum length of input (as text) + 1 */
  /* in_command = data type (int|string) */
  /* in_int =  pointer to the integer value to update */
  /* in_string = pointer to the string to update */
  /* in_onflag = trigger the creation of an input window */
  /* in_enabled ? */

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
      Say("Type controls the sprite's basic type - 0 means it is ornamental only"
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
	  " 9 = monster (all diag), 10 = monster(no diag)"
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
      in_int = &sp_is_warp;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_is_warp); //set default
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
	Say("How strong is this creature?  (0 = not alive/invincible)"
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
	Say("Can this be punched? 0 if yes.  Either way it will"
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
	Say("This will be deducted from any attack."
	    ,10,10);
      }

    if (in_master == INPUT_MINIMAP_LOAD)
      {
	in_command = 2; //string
	sprintf(in_default, "%s",  buf_path);
	in_max = 80;
	in_string = buf_path;
	blit(30,1,GFX_lpDDSBack,250,170);
	Say("Path:",260,175);
	Say("Enter the path with trailing backslash to a dir containing"
	    " another dink.dat and map.dat file to choose a replacement"
	    " for this block. (or enter to choose a replacement from the"
	    " current map)"
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

    if (in_master == INPUT_SCREEN_VISION)
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

    if (in_master == INPUT_SCREEN_MIDI)
      {
	in_command = 1; //number
	in_max = 10; //max _length
	sprintf(in_default,"%d",*in_int); //set default
	blit(30,1,GFX_lpDDSBack,250,170);
	Say("Music # for screen?:",260,175);
	Say("Will play #.MID for this screen if nothing else is playing."
	    ,10,10);
      }

    if (in_master == INPUT_SCREEN_TYPE)
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
    in_onflag = /*true*/1;
}


//this changes all none 0 blocks in this tile to num
void change_tile(int tile, int num)
{
  int x;
  for (x = 0; x < 50; x++)
    {
      int y;
      for (y = 0; y < 50; y++)
	if (hmap.htile[tile].x[x].y[y] != 0)
	  hmap.htile[tile].x[x].y[y] = num;
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
  /* Generic scaling - except no transparency */
  {
    SDL_Rect src, dst;
    src.x = playl;
    src.y = 0;
    src.w = playx - playl;
    src.h = 400;
    dst.x = x1;
    dst.y = y1;
    dst.w = 20;
    dst.h = 20;
    gfx_blit_stretch(GFX_lpDDSTwo, &src, GFX_lpDDSBack, &dst);
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
void UpdateCursorPosition(int dx, int dy)
{

    /*
     *  Pick up any leftover fuzz from last time.  This is important
     *  when scaling down mouse motions.  Otherwise, the user can
     *  drag to the right extremely slow for the length of the table
     *  and not get anywhere.
     */
    sp_cycle = 0;
    spr[1].x += dx;
    spr[1].y += dy;
    /* Clip the cursor to our client area */

    /* Try to get the mouse (and the focus) within the window, not
       100% safe but good enough */
    SDL_WarpMouse(320, 240);
    /* Ignore the mouse event generated by SDL_WarpMouse: */
    SDL_PumpEvents();
    SDL_GetRelativeMouseState(NULL, NULL);
    /* Alternatively, we can do this all the time, even when dx and dy
       are zero, and the mouse will always return to the
       application. We'd need to avoid that when the application is
       backgrounded though, otherwise FreeDink will keep warping the
       mouse. */
}


void  Scrawl_OnMouseInput(void)
{
  SDL_Event event;
  int dx, dy;

  mouse1 = /*false*/0;
  if (mode != 6) return;

  SDL_PumpEvents();
  SDL_GetRelativeMouseState(&dx, &dy);
  if (dx != 0 || dy != 0)
    UpdateCursorPosition(dx, dy);

  /* Process stacked clicks */
  while (SDL_PeepEvents(&event, 1, SDL_GETEVENT,
			SDL_EVENTMASK(SDL_MOUSEBUTTONDOWN)) > 0)
    {
      SDL_MouseButtonEvent *button_event = (SDL_MouseButtonEvent*)&event;
      if (button_event->button == SDL_BUTTON_LEFT)
	mouse1 = /*true*/1;
    }
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


/**
 * So-called "Movie2000 sprite movie maker" feature. It will dump a
 * series of DinkC moves that you specify using the mouse
 * (destination) and the numpad (direction). Check
 * http://www.dinknetwork.com/file/movie2000_tutorial
 */
void write_moves(void)
{
  char crap[100];
  char move[100];
  char fname[100];

  strcpy(fname, sp_script);
  if (strlen(sp_script) <= 2)
    strcpy(fname, "CRAP");


  if (sjoy.keyjustpressed[SDLK_KP8 /* 104 */])
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 8, %d, 1)\n", spr[1].y);
      add_text(move, crap);
    }
  if (sjoy.keyjustpressed[SDLK_KP4 /* 100 */])
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 4, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (sjoy.keyjustpressed[SDLK_KP5 /* 101 */])
    {
      EditorSoundPlayEffect(SOUND_JUMP);
      sprintf(crap, "story/%s.c",fname);
      add_text("//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n", crap);
    }
  if (sjoy.keyjustpressed[SDLK_KP2 /* 98 */])
    {
      EditorSoundPlayEffect(SOUND_JUMP);
      sprintf(crap, "story/%s.c", fname);
      sprintf(move, "move_stop(&current_sprite, 2, %d, 1)\n", spr[1].y);
      add_text(move, crap);
    }
  if (sjoy.keyjustpressed[SDLK_KP6 /* 102 */])
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 6, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (sjoy.keyjustpressed[SDLK_KP7 /* 103 */])
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 7, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (sjoy.keyjustpressed[SDLK_KP1 /* 97 */])
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 1, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (sjoy.keyjustpressed[SDLK_KP9 /* 105 */])
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 9, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (sjoy.keyjustpressed[SDLK_KP3 /* 99 */])
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 3, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
}


/**
 * Draw hardness single tile #'tile', pixel by pixel, in white. Used
 * to draw the currently selected hardness tile.
 */
void draw_hard_tile(int x1, int y1, int tile)
{
  SDL_Rect dst;
  dst.w = 1;
  dst.h = 1;

  int x = 0;
  for (x = 0; x < 50; x++)
    {
      int y = 0;
      for (y = 0; y < 50; y++)
	{
	  if (hmap.htile[tile].x[x].y[y] == 1)
	    {
	      //draw it
	      dst.x = x1 + x
		+ 20/* status bar length */;
	      dst.y = y1 + y;
	      /* TODO: not very efficient */
	      SDL_FillRect(GFX_lpDDSBack, &dst,
			   SDL_MapRGB(GFX_lpDDSBack->format, 255, 255, 255));
	    }
	}
    }
}


/**
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
  rect                rcRect;
  rect  Rect;
/*   rect rcRectSrc; */
/*   rect rcRectDest; */
  rect box_crap,box_real;
/*   POINT p; */
  char msg[500];
  char buff[200];
  //	DWORD               delay[4] = {0, 0, 0, 20};
/*   HDC         hdc; */
  int in_crap2 = 0;
  int                 holdx;
  //PALETTEENTRY        pe[256];
/*   HRESULT             ddrval; */
  int xx;
/*   DDBLTFX     ddbltfx; */
  /*BOOL*/int cool;
  /*BOOL*/int bs[MAX_SPRITES_AT_ONCE];

  int rank[MAX_SPRITES_AT_ONCE];
  int highest_sprite;

  int jj;

  SDL_framerateDelay(&framerate_manager);

  // Decide which frame will be blitted next
  thisTickCount = SDL_GetTicks();
  strcpy(buff,"Nothing");
  check_joystick();
  Scrawl_OnMouseInput();
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
    {
      int r1;
    for (r1 = 1; r1 < max_s+1; r1++)
      {
	int h1;
	highest_sprite = 22024; //more than it could ever be

	rank[r1] = 0;

	for (h1 = 1; h1 < max_s+1; h1++)
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
    }





  if (!in_enabled)

    for (jj = 1; jj < max_s; jj++)
      {

	int h = rank[jj];
	//Msg("Studying %d.,",h);

	if (spr[h].active)
	  {

	    //        Msg("Sprite %d is active.",h);

	    int greba = 0;

	    if (spr[h].brain == 1)
	      {
		if ((spr[h].seq == 0) || (mode == MODE_TILE_HARDNESS))
		  {
		    //if (mode == 7)
		    if (mode == MODE_SPRITE_HARDNESS)
		      {
			//editing a sprite, setting hard box and depth dot.
			spr[1].pseq = 1;
			spr[1].pframe = 1;

			if (sjoy.button[EDITOR_ACTION_ESCAPE])
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

			if (sjoy.keyjustpressed[SDLK_TAB /* 9 */])
			  {

			    //they hit tab, lets toggle what mode they are in
			    if (sp_mode == 0) sp_mode = 1; else if (sp_mode == 1) sp_mode = 2; else if (sp_mode == 2) sp_mode = 0;


			  }
			if (sjoy.charjustpressed['s' /* 83 */])
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

			    //ok, we are editing depth dot

			    if (SDL_GetModState()&KMOD_CTRL)
			      {
				if (sjoy.keyjustpressed[SDLK_RIGHT /* 39 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].xoffset += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }

				if (sjoy.keyjustpressed[SDLK_LEFT /* 37 */])
				  {

				    k[seq[sp_seq].frame[sp_frame]].xoffset -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.keyjustpressed[SDLK_UP /* 38 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].yoffset -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }

				if (sjoy.keyjustpressed[SDLK_DOWN /* 40 */])
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
				if (sjoy.keyjustpressed[SDLK_RIGHT /* 39 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.right += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }

				if (sjoy.keyjustpressed[SDLK_LEFT /* 37 */])
				  {

				    k[seq[sp_seq].frame[sp_frame]].hardbox.right -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.keyjustpressed[SDLK_UP /* 38 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.bottom -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }

				if (sjoy.keyjustpressed[SDLK_DOWN /* 40 */])
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
				if (sjoy.keyjustpressed[SDLK_RIGHT /* 39 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.left += modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }

				if (sjoy.keyjustpressed[SDLK_LEFT /* 37 */])
				  {

				    k[seq[sp_seq].frame[sp_frame]].hardbox.left -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }
				if (sjoy.keyjustpressed[SDLK_UP /* 38 */])
				  {
				    k[seq[sp_seq].frame[sp_frame]].hardbox.top -= modif;
				    EditorSoundPlayEffect( SOUND_STOP );
				  }

				if (sjoy.keyjustpressed[SDLK_DOWN /* 40 */])
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
			if ( (sjoy.charjustpressed['v']) )
			  {
			    in_master = INPUT_SCREEN_VISION; // Set screen vision?
			  }

			int modif = 0;
			if (SDL_GetModState()&KMOD_SHIFT)
			  modif = 9;

			if (sjoy.charjustpressed['m'])
			  {
			    if (sp_screenmatch)
			      sp_screenmatch = /*false*/0;
			    else
			      sp_screenmatch = /*true*/1;
			  }


			if (SDL_GetModState()&KMOD_ALT) // alt
			  {
			    //alt is held down 87
			    if (sjoy.charjustpressed['w' /* 87 */])
			      {
				//pressed W
				if (((spr[1].pseq == 10) && (spr[1].pframe == 8)))
				  {
                                    //a sprite is not chosen
				    hold_warp_map = cur_map;
				    hold_warp_x = spr[1].x;
				    hold_warp_y= spr[1].y;
				    EditorSoundPlayEffect(SOUND_JUMP);
				  }
				else
				  {
				    sp_warp_map = hold_warp_map ;
				    sp_warp_x = hold_warp_x;
				    sp_warp_y = hold_warp_y;
				    EditorSoundPlayEffect(SOUND_JUMP);
				  }
			      }
			  }

			/**
			 * Edit sprite properties
			 */
			if (!((spr[1].pseq == 10) && (spr[1].pframe == 8)))
			  {
			    //they are wheeling around a sprite
			    if (spr[1].x > 1500) spr[1].x = 1500;
			    if (spr[1].y > 1500) spr[1].y = 1500;

			    if (spr[1].size > 1500) spr[1].size = 1500;

			    /* if (GetKeyboard(VK_OEM_4 /\* 219 *\/)) // '[' for US */
			    if (getcharstate('['))
			      spr[1].size -= 1+modif;
			    /* if (GetKeyboard(VK_OEM_6 /\* 221 *\/)) // ']' for US */
			    if (getcharstate(']'))
			      spr[1].size += 1+modif;


			    if (SDL_GetModState()&KMOD_SHIFT)
			      {
				//shift is being held down
				if (getkeystate('1') || getkeystate(SDLK_KP1) || getkeystate(SDLK_F1))  in_master = 11;
				if (getkeystate('2') || getkeystate(SDLK_KP2) || getkeystate(SDLK_F2))  in_master = 12;
				if (getkeystate('3') || getkeystate(SDLK_KP3) || getkeystate(SDLK_F3))  in_master = 13;
				if (getkeystate('4') || getkeystate(SDLK_KP4) || getkeystate(SDLK_F4))  in_master = 14;
				if (getkeystate('5') || getkeystate(SDLK_KP5) || getkeystate(SDLK_F5))  in_master = 15;

				if (getkeystate('6') || getkeystate(SDLK_KP6) || getkeystate(SDLK_F6))  in_master = 16;
				if (getkeystate('7') || getkeystate(SDLK_KP7) || getkeystate(SDLK_F7))  in_master = 17;
				if (getkeystate('8') || getkeystate(SDLK_KP8) || getkeystate(SDLK_F8))  in_master = 18;
				if (getkeystate('9') || getkeystate(SDLK_KP9) || getkeystate(SDLK_F9))  in_master = 19;



			      }
			    else if (SDL_GetModState()&KMOD_ALT)
			      {
				  //alt is being held down
				  if (getkeystate('1') || getkeystate(SDLK_KP1) || getkeystate(SDLK_F1))  in_master = 20;
				  if (getkeystate('2') || getkeystate(SDLK_KP2) || getkeystate(SDLK_F2))  in_master = 21;
				  if (getkeystate('3') || getkeystate(SDLK_KP3) || getkeystate(SDLK_F3))  in_master = 22;
				  /*(getkeystate('4' /\* 52 *\/))  in_master = 14;
				    if (getkeystate(53))  in_master = 15;

				    if (getkeystate(54))  in_master = 16;
				    if (getkeystate(55))  in_master = 17;
				    if (getkeystate(56))  in_master = 18;
				    if (getkeystate(57))  in_master = 19;

				  */
			      }
			    else
			      {
				  //shift is not being held down
				  if (getkeystate('1') || getkeystate(SDLK_KP1) || getkeystate(SDLK_F1)) in_master = 1;
				  if (getkeystate('2') || getkeystate(SDLK_KP2) || getkeystate(SDLK_F2)) in_master = 2;
				  if (getkeystate('3') || getkeystate(SDLK_KP3) || getkeystate(SDLK_F3)) in_master = 3;
				  if (getkeystate('4') || getkeystate(SDLK_KP4) || getkeystate(SDLK_F4)) in_master = 4;
				  if (getkeystate('5') || getkeystate(SDLK_KP5) || getkeystate(SDLK_F5)) in_master = 5;
				  if (getkeystate('6') || getkeystate(SDLK_KP6) || getkeystate(SDLK_F6)) in_master = 6;
				  if (getkeystate('7') || getkeystate(SDLK_KP7) || getkeystate(SDLK_F7)) in_master = 7;
				  if (getkeystate('8') || getkeystate(SDLK_KP8) || getkeystate(SDLK_F8)) in_master = 8;
				  if (getkeystate('9') || getkeystate(SDLK_KP9) || getkeystate(SDLK_F9)) in_master = 9;
				  if (getkeystate('0') || getkeystate(SDLK_KP0) || getkeystate(SDLK_F10)) in_master = 10;
			      }


			    if (sjoy.charjustpressed['s' /* 83 */])
			      {
				smart_add();

				draw_map();
			      }

			    if ( (sjoy.button[EDITOR_ACTION_RETURN]) | (mouse1) )
			      {
				smart_add();
				draw_map();
				spr[1].pseq = 10;
				spr[1].pframe = 8;
				spr[1].size = 100;
				rect_set(&spr[1].alt,0,0,0,0);

			      }

			    if (sjoy.keyjustpressed[SDLK_DELETE /* 46 */])
			      {

				spr[1].pseq = 10;
				spr[1].pframe = 8;
				spr[1].size = 100;
				rect_set(&spr[1].alt,0,0,0,0);

			      }

			  }
			else
			  {
			    //no sprite is currently selected

			    int max_spr = 0;
			    int jj;

			    write_moves();



			    for (jj=1; jj < 100; jj++)
			      {
				if ( pam.sprite[jj].active) if (pam.sprite[jj].vision == map_vision) max_spr++;
			      }


			    if (max_spr > 0)
			      {

				/* if (sjoy.keyjustpressed[VK_OEM_4 /\* 219 *\/]) // '[' for US */
				if (sjoy.charjustpressed['['])
				  {
				    sp_cycle--;
				    if (sp_cycle < 1)
				      sp_cycle = max_spr;
				  }

				/* if (sjoy.keyjustpressed[VK_OEM_6 /\* 221 *\/]) // ']' for US */
				if (sjoy.charjustpressed[']'])
				  {
				    sp_cycle++;
				    if (sp_cycle > max_spr)
				      sp_cycle = 1;
				  }



			      }



			    //Msg("Cycle is %d", sp_cycle);
			    int realpic = 0;

			    if (sp_cycle > 0)
			      {
				//lets draw a frame around the sprite we want
				int dumbpic = 0;
				int jh;
				realpic = 0;
				for (jh = 1; dumbpic != sp_cycle; jh++)
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
				rect_copy(&spr[sprite].alt , &pam.sprite[realpic].alt);
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
				  SDL_FillRect(GFX_lpDDSBack, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 33, 41, 16));
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
				  SDL_FillRect(GFX_lpDDSBack, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 33, 41, 16));
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
				  SDL_FillRect(GFX_lpDDSBack, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 33, 41, 16));
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
				  SDL_FillRect(GFX_lpDDSBack, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 33, 41, 16));
				}

				//	if (ddrval != DD_OK) dderror(ddrval);

				spr[sprite].active = /*false*/0;

			      }

			  fail:


			    if ( (sjoy.button[EDITOR_ACTION_RETURN]) | (mouse1))
			      {
				//pick up a sprite already placed by hitting enter
				int uu;

				for (uu = 100; uu > 0; uu--)
				  {
				    if ( pam.sprite[uu].active) if ( ( pam.sprite[uu].vision == 0) || (pam.sprite[uu].vision == map_vision))
								  {

								    int	sprite = add_sprite_dumb(pam.sprite[uu].x,pam.sprite[uu].y,0,
												 pam.sprite[uu].seq, pam.sprite[uu].frame,
												 pam.sprite[uu].size);
								    rect_copy(&spr[sprite].alt , &pam.sprite[uu].alt);
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
									    log_debug("Ah yeah, using %d!", realpic);
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
									rect_copy(&spr[1].alt , &pam.sprite[uu].alt);
									sp_frame = pam.sprite[uu].frame;
									spr[1].pseq = pam.sprite[uu].seq;
									spr[1].pframe = pam.sprite[uu].frame;

									sp_is_warp = pam.sprite[uu].is_warp;

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

									pam.sprite[uu].active = /*false*/0; //erase sprite
									draw_map();
									spr[sprite].active = /*false*/0;
									break;
								      }
								    spr[sprite].active = /*false*/0;

								  }
				  }


			      }


			    if ((SDL_GetModState()&KMOD_ALT) && (getkeystate(SDLK_DELETE /* 46 */)))
			      {
				int ll;
				for (ll = 1; ll < 100; ll++)
				  {
				    pam.sprite[ll].active = /*false*/0;
				  }
				draw_map();
				rect_set(&spr[h].alt,0,0,0,0);
			      }
			  }

			/** Trim a sprite **/
			if (getcharstate('z') || (getcharstate('x')))
			  {
			    if ((spr[h].alt.right == 0) && (spr[h].alt.left == 0)
				&& (spr[h].alt.top == 0) && (spr[h].alt.bottom == 0))
			      rect_copy(&spr[h].alt, &k[getpic(h)].box);
			  }

                        if (getcharstate('z'))
			  {

			    if (sjoy.keyjustpressed[SDLK_RIGHT /* 39 */])
			      {
				spr[h].alt.left += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }

			    if (sjoy.keyjustpressed[SDLK_LEFT /* 37 */])
			      {
				spr[h].alt.left -= spr[h].speed +modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    if (sjoy.keyjustpressed[SDLK_DOWN /* 40 */])
			      {
				spr[h].alt.top += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }

			    if (sjoy.keyjustpressed[SDLK_UP /* 38 */])
			      {
				spr[h].alt.top -= spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }


			    if (spr[h].alt.top < 0) spr[h].alt.top = 0;
			    if (spr[h].alt.left < 0) spr[h].alt.left = 0;
			    goto b1end;
			  }



			if (getcharstate('x'))
			  {

			    if (sjoy.keyjustpressed[SDLK_RIGHT /* 39 */])
			      {
				spr[h].alt.right += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }

			    if (sjoy.keyjustpressed[SDLK_LEFT /* 37 */])
			      {

				spr[h].alt.right -= spr[h].speed +modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    if (sjoy.keyjustpressed[SDLK_DOWN /* 40 */])
			      {
				spr[h].alt.bottom += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }

			    if (sjoy.keyjustpressed[SDLK_UP /* 38 */])
			      {
				spr[h].alt.bottom -= spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );

				//	Msg("Bottom is %d..",spr[h].alt.bottom);

			      }
			    if (spr[h].alt.bottom > k[getpic(h)].box.bottom) spr[h].alt.bottom = k[getpic(h)].box.bottom;
			    if (spr[h].alt.right > k[getpic(h)].box.right) spr[h].alt.right = k[getpic(h)].box.right;

			    goto b1end;

			  }


			if (spr[1].size < 1)
			  spr[1].size = 1;

			/* Precise positionning: move the sprite just
			   1 pixel left/right/up/down, then don't do
			   move until an arrow key is released and
			   pressed again. */
			if (SDL_GetModState()&KMOD_CTRL)
			  {
			    if (sjoy.keyjustpressed[SDLK_RIGHT /* 39 */])
			      {
				
				sp_cycle = 0;
				
				spr[h].x += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
			      }
			    
			    if (sjoy.keyjustpressed[SDLK_LEFT /* 37 */])
			      {
				spr[h].x -= spr[h].speed +modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;
			      }
			    if (sjoy.keyjustpressed[SDLK_UP /* 38 */])
			      {
				spr[h].y -= spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;
			      }
			    
			    if (sjoy.keyjustpressed[SDLK_DOWN /* 40 */])
			      {
				spr[h].y += spr[h].speed + modif;
				EditorSoundPlayEffect( SOUND_STOP );
				sp_cycle = 0;
			      }
			    
			  }
			else
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





			if (  (sjoy.button[EDITOR_ACTION_ESCAPE]) )
			  {
			    //return to edit mode or drop sprite, depending..
			    if (((spr[1].pseq == 10) && (spr[1].pframe == 8))  )

			      {

				rect_set(&spr[1].alt,0,0,0,0);

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
				rect_set(&spr[1].alt,0,0,0,0);

				spr[1].pseq = 10;
				spr[1].pframe = 8;
				spr[1].size = 100;

			      }
			  }


			if (sjoy.charjustpressed['e'])
			  {
			    //they hit E, go to sprite picker
			    rect_set(&spr[1].alt,0,0,0,0);

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
			if (sjoy.button[EDITOR_ACTION_TAB])
			  {
			    //they hit tab, return to tile edit mode
			    if (    !((spr[1].pseq == 10) && (spr[1].pframe == 8))  )

			      {
				smart_add();
				rect_set(&spr[1].alt,0,0,0,0);

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


		    if ( (mode == MODE_SCREEN_TILES)
			 && (sjoy.button[EDITOR_ACTION_TAB]))
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

			      if (sjoy.charjustpressed['e'])
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
				  SDL_FillRect(GFX_lpDDSTwo, NULL, SDL_MapRGB(GFX_lpDDSTwo->format, 255, 255, 255));

				  holdx = (spr[1].x / 50);
				  int holdy = (spr[1].y / 50)+1;
				  holdx = holdx * 8;
				  if (seq[sp_seq].frame[holdx + holdy] == 0) goto sp_fin;

				  add_sprite_dumb(320,200 , 0,sp_seq,holdx + holdy,100 );

				  sp_frame = holdx + holdy;
				  spr[1].pseq = 10;
				  spr[1].pframe = 8;


				  spr[1].speed = 1;
				  goto sp_edit_end;

				}

			      if ((sjoy.button[EDITOR_ACTION_ESCAPE]))
				{


				  //returning to main sprite picker mode
				  sp_seq = 0;

				  draw15(sp_picker);
				  spr[h].x = m5ax;
				  spr[h].y = m5ay;

				  goto sp_edit_end;
				}

			      if (sjoy.button[EDITOR_ACTION_TAB])
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

			      if (sjoy.button[EDITOR_ACTION_RETURN])
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


			  if (sjoy.button[EDITOR_ACTION_TAB] || sjoy.button[EDITOR_ACTION_ESCAPE])
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

			  /* if (sjoy.keyjustpressed[VK_OEM_4 /\* 219 *\/]) // '[' for US */
			  if (sjoy.charjustpressed['['])
			    {
			      if (sp_picker > 95) sp_picker -= 96; else
				{
				  sp_picker = (4 * 96);
				}
			      draw15(sp_picker);
			    }
			    /* if (sjoy.keyjustpressed[VK_OEM_6 /\* 221 *\/]) // ']' for US */
			  if (sjoy.charjustpressed[']'])
			    {
			      if (sp_picker < 400) sp_picker += 96;
			      draw15(sp_picker);
			    }

			  if (sjoy.button[EDITOR_ACTION_RETURN])
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
			draw_minimap();
			spr[1].que = 20000;
			mode = MODE_MINIMAP;
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
			    if ((SDL_GetModState()&KMOD_SHIFT) && (getkeystate(SDLK_RIGHT)))
			      {
				spr[h].seq = 4;
				spr[h].frame = 1;
				if (selx < 8) selx++;
				goto b1fun;
			      }

			    if ((SDL_GetModState()&KMOD_SHIFT) && (getkeystate(SDLK_LEFT)))
			      {
				spr[h].seq = 4;
				spr[h].frame = 1;
				if (selx > 1) selx--;
				goto b1fun;
			      }

			    if ((SDL_GetModState()&KMOD_SHIFT) && (getkeystate(SDLK_UP)))
			      {
				spr[h].seq = 4;
				spr[h].frame = 1;
				if (sely > 1) sely--;
				goto b1fun;
			      }

			    if ((SDL_GetModState()&KMOD_SHIFT) && (getkeystate(SDLK_DOWN)))
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
				EditorSoundPlayEffect(SOUND_STOP);
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
			if (getcharstate('z'))
			  {
			    int y;
			    for (y = 0; y < sely; y++)
			      {
				int x;
				for (x = 0; x < selx; x++)
				  {
				    hmap.htile[hard_tile].x[((spr[h].x) + (x*9) - 95) / 9].y[(spr[h].y + (y *9)) / 9] = 1;

				  }
			      }
			  }


			//change a piece to soft
			if (getcharstate('x'))
			  {
			    int y;
			    for (y = 0; y < sely; y++)
			      {
				int x;
				for (x = 0; x < selx; x++)
				  {
				    hmap.htile[hard_tile].x[((spr[h].x) + (x*9) - 95) / 9].y[(spr[h].y + (y *9)) / 9] = 0;

				  }
			      }
			  }


			if ( (getcharstate('a')) && (SDL_GetModState()&KMOD_ALT ) )
			  {
			    //change ALL to 'low hard'
			    change_tile(hard_tile, 2);
			    log_debug("Changing whole tile to 2");

			    return;
			  }

			if ( (getcharstate('s')) && (SDL_GetModState()&KMOD_ALT ) )
			  {
			    //change ALL to 'low hard'
			    change_tile(hard_tile, 3);
			    log_debug("Chaning whole tile to 3");

			    return;
			  }
			if ( (getcharstate('x')) && (SDL_GetModState()&KMOD_ALT ) )
			  {
			    //change ALL to 'low hard'
			    change_tile(hard_tile, 1);
			    log_debug("Changing whole tile to 1");

			    return;
			  }


                        if (getcharstate('a'))
			  {
			    int y;
			    for (y = 0; y < sely; y++)
			      {
				int x;
				for (x = 0; x < selx; x++)
				  {
				    hmap.htile[hard_tile].x[((spr[h].x) + (x*9) - 95) / 9].y[(spr[h].y + (y *9)) / 9] = 2;

				  }
			      }

			  }
                        if (getcharstate('s'))
			  {
			    int y;
			    for (y = 0; y < sely; y++)
			      {
				int x;
				for (x = 0; x < selx; x++)
				  {
				    hmap.htile[hard_tile].x[((spr[h].x) + (x*9) - 95) / 9].y[(spr[h].y + (y *9)) / 9] = 3;

				  }
			      }

			  }


			//update frame with current hard blocks, slow

			draw_hard();

			if (sjoy.button[EDITOR_ACTION_ESCAPE] == 1
			    || sjoy.button[EDITOR_ACTION_RETURN] == 1)
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

				mode = MODE_SCREEN_HARDNESS_INIT;
				return;
				//goto skip_draw;
			      }

			    if (last_mode > 0)
			      {
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

		    if ( (sjoy.charjustpressed['b']) )
		      {
			in_master = 31;


		      }

		    if ( (sjoy.charjustpressed['v']) )
		      {
			in_master = INPUT_SCREEN_VISION;
		      }



		    if (((mode == MODE_SCREEN_TILES) && (sjoy.button[EDITOR_ACTION_RETURN]))
			|| ((mode == MODE_TILE_PICKER) && (getkeystate(SDLK_SPACE))))

		      {

			if (mode == MODE_SCREEN_TILES)
			  cur_tile = pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].square_full_idx0;

			if (mode == MODE_TILE_PICKER)
			  {
			    cur_tile = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50);
			    cur_tile += (cur_screen * 128) - 128;
			  }

			while(kill_last_sprite());
			draw_current();

			if (cur_tile > 0)
			  {
			    if (hmap.btile_default[cur_tile] == 0)
			      {
				int j;
				for (j = 1; j < 799; j++)
				  {
				    if (hmap.htile[j].used == /*FALSE*/0)
				      {
					hmap.btile_default[cur_tile] = j;
					hmap.htile[j].used = /*TRUE*/1;
				    	hard_tile = j;
					goto tilesel;
				      }
				  }
			      }
			    else
			      hard_tile = hmap.btile_default[cur_tile];

			  tilesel:
			    xx = cur_tile % 128;
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
			      src.x = (xx % 12) * 50;
			      src.y = (xx / 12) * 50;
			      src.w = 50;
			      src.h = 50;
			      dst.x = 95;
			      dst.y = 0;
			      dst.w = 450;
			      dst.h = 450;
			      
			      cool = cur_tile / 128;
			      gfx_blit_stretch(gfx_tiles[cool+1], &src, GFX_lpDDSTwo, &dst);
			    }

			    m4x = spr[h].x;
			    m4y = spr[h].y;

			    spr[1].x = 95;
			    spr[1].y = 0;
			    selx = 1;
			    sely = 1;

			    mode = MODE_TILE_HARDNESS;
			  }
		      }


		    if ((mode == MODE_TILE_PICKER) || (mode == MODE_SCREEN_TILES))
		      {
			//resizing the box

			if ((SDL_GetModState()&KMOD_SHIFT) && (getkeystate(SDLK_RIGHT /* 39 */)) )
			  {
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    if (selx < 8) selx++;
			    goto b1end;
			  }

			if ((SDL_GetModState()&KMOD_SHIFT) && (getkeystate(SDLK_LEFT /* 37 */)) )
			  {
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    if (selx > 1) selx--;
			    goto b1end;

			  }

			if ((SDL_GetModState()&KMOD_SHIFT) && (getkeystate(SDLK_UP /* 38 */)) )
			  {
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    if (sely > 1) sely--;
			    goto b1end;
			  }

			if ((SDL_GetModState()&KMOD_SHIFT) && (getkeystate(SDLK_DOWN /* 40 */)) )
			  {
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    if (sely <  8) sely++;
			    goto b1end;
			  }

		      }


		    if (getkeystate(SDLK_RIGHT))
		      {
			spr[h].x += spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect( SOUND_STOP );
		      }


		    if ((getcharstate('s')) && (mode == MODE_SCREEN_TILES))
		      {
			int y;
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			//EditorSoundPlayEffect( SOUND_JUMP );


			pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].square_full_idx0 = cur_tile;

			for (y = 0; y < sely; y++)
			  {
			    int x;
			    for (x = 0; x < selx; x++)
			      {
				holdx = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50);
				holdx += (y * 12);
				holdx += x;
				pam.t[holdx].square_full_idx0 = (cur_tile + (y * 12) + x);

			      }
			  }

			draw_map();
		      }



		    if ((getcharstate('c')) && (mode == MODE_SCREEN_TILES))
		      {
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			//SoundPlayEffect( SOUND_JUMP );
			cur_tile = pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].square_full_idx0;
			draw_map();
		      }

		    /* Tile selection */
		    if (mode == MODE_SCREEN_TILES || mode == MODE_TILE_PICKER)
		      {
			int unit = 0, tile_no = 0;
			if (getkeystate('1') || getkeystate(SDLK_KP1) || getkeystate(SDLK_F1)) unit = 1;
			if (getkeystate('2') || getkeystate(SDLK_KP2) || getkeystate(SDLK_F2)) unit = 2;
			if (getkeystate('3') || getkeystate(SDLK_KP3) || getkeystate(SDLK_F3)) unit = 3;
			if (getkeystate('4') || getkeystate(SDLK_KP4) || getkeystate(SDLK_F4)) unit = 4;
			if (getkeystate('5') || getkeystate(SDLK_KP5) || getkeystate(SDLK_F5)) unit = 5;
			if (getkeystate('6') || getkeystate(SDLK_KP6) || getkeystate(SDLK_F6)) unit = 6;
			if (getkeystate('7') || getkeystate(SDLK_KP7) || getkeystate(SDLK_F7)) unit = 7;
			if (getkeystate('8') || getkeystate(SDLK_KP8) || getkeystate(SDLK_F8)) unit = 8;
			if (getkeystate('9') || getkeystate(SDLK_KP9) || getkeystate(SDLK_F9)) unit = 9;
			if (getkeystate('0') || getkeystate(SDLK_KP0) || getkeystate(SDLK_F10)) unit = 10;

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
			    && getkeystate(SDLK_BACKQUOTE))
			  loadtile(41);
			/* alternatives for non-US keyboards */
			if (getkeystate(SDLK_F11) || getkeystate(SDLK_KP_PERIOD))
			  loadtile(41);
		      }

		    //if ( (GetKeyboard(48)) && ( (mode == 3) | (mode ==2)) ) loadtile(11);


		    if ((sjoy.button[EDITOR_ACTION_RETURN]) && (mode == MODE_TILE_PICKER))
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



		    if (sjoy.button[EDITOR_ACTION_ESCAPE] && (mode == MODE_TILE_PICKER))
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


		    if ( (sjoy.keyjustpressed[SDLK_SPACE])  && (mode == MODE_MINIMAP))
		      {
			//make_map_tiny();
			draw_map_tiny = 0;

		      }

		    if ( (sjoy.charjustpressed['l'])  && (mode == MODE_MINIMAP))
		      {

			//if (map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)] != 0)
			//{
			buf_map = (((spr[1].y+1)*32) / 20)+(spr[1].x / 20);
			in_master = INPUT_MINIMAP_LOAD;
			//}

		      }


		    if ( (sjoy.keyjustpressed[SDLK_ESCAPE /* 27 */]) && (mode == MODE_MINIMAP))
		      {
			load_info();
			draw_minimap();
			buf_mode = /*false*/0;

		      }


		    if ( (sjoy.charjustpressed['m']) && (mode == MODE_MINIMAP))
		      {
			//set music # for this block
			in_int = &map.music[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
			in_master = INPUT_SCREEN_MIDI;
		      }

		    if ( (sjoy.charjustpressed['s']) && (mode == MODE_MINIMAP))
		      {
			//set music # for this block
			in_int = &map.indoor[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
			in_master = INPUT_SCREEN_TYPE;
		      }

		    if ( (sjoy.charjustpressed['q']) && (mode == MODE_MINIMAP))
		      {
			save_hard();
			log_info("Info saved.");
			SDL_Event ev;
			ev.type = SDL_QUIT;
			SDL_PushEvent(&ev);
			return;
		      }

		    if ( (sjoy.button[EDITOR_ACTION_RETURN]) && (mode == MODE_MINIMAP))
		      {

			if (buf_mode)
			  {
			    //lets replace this screen

			    buf_mode = /*false*/0;


			    if (!load_map_buf(buffmap.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)]))
			      {
				draw_minimap();
				sjoy.button[EDITOR_ACTION_RETURN] = /*false*/0;
				return;
			      }


			    load_info();

			    if (map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)] == 0)
			      {

				(
				 map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)]) = add_new_map();
				//wrongo, let's add the map

				//draw_minimap();


			      }

			    map.indoor[buf_map] = buffmap.indoor[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
			    map.music[buf_map] = buffmap.music[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];

			    save_map(map.loc[buf_map]);

			    save_info();
			    draw_minimap();
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

			log_debug("Y is %d X is %d", spr[h].y, spr[h].x);

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
		    if ( (mode == MODE_SCREEN_TILES) && (getkeystate(189 /* VK_OEM_MINUS */)) )
		      {
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			cur_tile--;
			if (cur_tile < 0) cur_tile = 0;
		      }
		    if ( (mode == MODE_SCREEN_TILES) && (getkeystate(187 /* VK_OEM_PLUS */)) )
		      {
			spr[h].seq = 3;
			spr[h].seq_orig = 3;

			cur_tile++;
			//if (cur_tile > 127) cur_tile = 127;
		      }


		    if ((mode == MODE_SCREEN_TILES) && (sjoy.charjustpressed['h']))
		      {
			//start althard mode

			mode = MODE_SCREEN_HARDNESS_INIT;
			goto skip_draw;
		      }

		    if (mode == MODE_SCREEN_HARDNESS)
		      {
			//mode for it
			if (sjoy.keyjustpressed[SDLK_ESCAPE])
			  {
			    //exit mode 8
			    mode = MODE_SCREEN_TILES;
			    spr[h].seq = 3;
			    spr[h].seq_orig = 3;
			    draw_map();
			    goto b1end;
			  }

			/* if (sjoy.keyjustpressed[/\* VK_OEM_4 *\/ 219]) // '[' for US */
			if (sjoy.charjustpressed['['])
			  {
			    hard_tile--;
			    if (hard_tile < 0) hard_tile = 799;
			  }
			/* if (sjoy.keyjustpressed[/\* VK_OEM_6 *\/ 221]) // ']' for US */
			if (sjoy.charjustpressed[']'])
			  {
			    hard_tile++;
			    if (hard_tile > 799) hard_tile = 0;
			  }

			if (sjoy.charjustpressed['c'])
			  {
			    //copy tile hardness from current block
			    hard_tile = realhard(xy2screentile(spr[1].x, spr[1].y));
			  }

			if (sjoy.charjustpressed['s'])
			  {
			    //stamp tile hardness to selected
			    pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].althard = hard_tile;
			    draw_map();
			    mode = MODE_SCREEN_HARDNESS_INIT;

			    return;
			  }

			if (sjoy.keyjustpressed[SDLK_DELETE])
			  {
			    //stamp tile hardness to selected
			    pam.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].althard = 0;
			    draw_map();
			    mode = MODE_SCREEN_HARDNESS_INIT;

			    return;
			  }

			/* Display the hard tile in the clipboard */
			char crapa[20];
			sprintf(crapa, "Copy: %d",hard_tile);
			SaySmall(crapa, 580,400, 255,255,255);

			draw_hard_tile(spr[1].x,spr[1].y, hard_tile);

			if (sjoy.keyjustpressed[SDLK_RETURN])
			  {
			    //they want to edit this alt hardness, let's do it
			    cur_tile = pam.t[xy2screentile(spr[1].x, spr[1].y)].square_full_idx0;

			    xx = cur_tile - (cool * 128);
			    Rect.left = spr[1].x+20;
			    Rect.top = spr[1].y;
			    Rect.right = Rect.left + 50;
			    Rect.bottom = Rect.top + 50;

/*			    crapRec.top = 0; */
/*			    crapRec.left = 95; */
/*			    crapRec.bottom = 450; */
/*			    crapRec.right = 95+450; */

/* 			    ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/* 			    ddbltfx.dwSize = sizeof( ddbltfx); */
			    spr[1].seq = 0;
			    spr[1].pseq = 10;
			    spr[1].pframe = 1;


/* 			    lpDDSTwo->Blt(&crapRec , lpDDSBack, */
/* 					  &Rect, DDBLT_DDFX | DDBLT_WAIT,&ddbltfx ); */
			    // GFX
			    /* In this particular case, we're scaling
			       the whole screen backbuffer by 900%
			       just to scale a single 50x50 square of
			       it... */
			    {
			      SDL_Rect src, dst;
			      src.x = spr[1].x+20;
			      src.y = spr[1].y;
			      src.w = 50;
			      src.h = 50;
			      dst.x = 95;
			      dst.y = 0;
			      dst.w = 450;
			      dst.h = 450;
			      gfx_blit_stretch(GFX_lpDDSBack, &src, GFX_lpDDSTwo, &dst);
			    }

			    m4x = spr[h].x;
			    m4y = spr[h].y;

			    spr[1].x = 95;
			    spr[1].y = 0;
			    selx = 1;
			    sely = 1;

			    mode = MODE_TILE_HARDNESS;

			    hmap.htile[hard_tile].used = /*true*/1;
			    last_modereal = 8;
			  }
		      }

		    if ((mode == MODE_SCREEN_TILES)
			&& (SDL_GetModState()&KMOD_ALT)
			&& sjoy.charjustpressed['x'])
		      {
			spr[h].seq = 2;
			spr[h].seq_orig = 2;
			m3x = spr[h].x;
			m3y = spr[h].y;
			spr[h].x = m1x;
			spr[h].y = m1y;
			mode = MODE_MINIMAP;
			spr[h].speed = 20;
			load_info();
			draw_minimap();
			while (kill_last_sprite());
			return;
		      }

		    if ((mode == MODE_SCREEN_TILES) && (sjoy.button[EDITOR_ACTION_ESCAPE]))
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
			draw_minimap();
			while (kill_last_sprite());
			return;
		      }


		    if (getkeystate(SDLK_LEFT))
		      {
			spr[h].x -= spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect(SOUND_STOP);
		      }

		    if (getkeystate(SDLK_DOWN))
		      {
			spr[h].y += spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect(SOUND_STOP);
		      }

		    if (getkeystate(SDLK_UP))
		      {
			spr[h].y -= spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect(SOUND_STOP);
		      }

		    if (spr[h].speed < 1)
		      spr[h].speed = 1;
		    if (spr[h].y > (y - k[getpic(h)].box.bottom))
		      spr[h].y = y - k[getpic(h)].box.bottom;
		    if (spr[h].x > (x - k[getpic(h)].box.right))
		      spr[h].x = x - k[getpic(h)].box.right;
		    if (spr[h].x < 0)
		      spr[h].x = 0;
		    if (spr[h].y < 0)
		      spr[h].y = 0;

		    // end human brain (1)


		    if ((mode == MODE_TILE_PICKER) || (mode == MODE_SCREEN_TILES)
			|| (mode == MODE_SPRITE_PICKER) || (mode == MODE_SCREEN_HARDNESS))
		      {
			if ((selx * 50 + spr[1].x) > 600)
			  {
			    spr[1].x = 600 - (selx * 50);
			  }
		      }

		    if ((mode == MODE_TILE_PICKER))
		      {
			if ((sely * 50 + spr[1].y) > 450)
			  {
			    spr[1].y = 450 - (sely * 50);
			  }
		      }
		    if ((mode == MODE_SCREEN_TILES) || (mode == MODE_SPRITE_PICKER)
			|| (mode == MODE_SCREEN_HARDNESS))
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


	    if (mode == MODE_SCREEN_TILES)
	      {
		//need offset to look right
		k[seq[3].frame[1]].xoffset = -20;
		greba = 20;
	      }
	    if (mode == MODE_TILE_PICKER)
	      {
		//pick a tile, needs no offset
		k[seq[3].frame[1]].xoffset = 0;
		greba = 0;
	      }

	    //		if (  !(( h == 1) & (mode == 9)) )
	    if (!((h == 1) && (mode == MODE_SCREEN_HARDNESS_INIT)))
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
		if (mode == MODE_SCREEN_TILES || mode == MODE_TILE_PICKER)
		  {
		    /* Draw the tile squares selector, an expandable
		       array of white non-filled squares */
		    int y;
		    for (y = 0; y < sely; y++)
		      {
			int x;
			for (x = 0; x < selx; x++)
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


		if ((mode == MODE_TILE_HARDNESS))
		  {
		    /* Display the current "pencil"/square to draw hardness with */
		    int yy;
		    for (yy = 0; yy < sely; yy++)
		      {
			int xx;
			for (xx = 0; xx < selx; xx++)
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


  if (mode == MODE_SCREEN_HARDNESS_INIT)
    {
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

      if (mode == MODE_MINIMAP)
	{
	  if (20 * (spr[1].y / 20) != spr[1].y)
	    {
	      spr[1].y += 10;
	    }
	  sprintf(msg,
		  "Map # %d - Press ENTER to edit, SPACE to detail map. (%d)  (Q) to quit and save. L to replace a "
		  "screen from another map file.  Z to toggle this help text. S to modify screentype. M to modify midi.",
		  mode,
		  (((spr[1].y+1)*32) / 20)+(spr[1].x / 20));
/* 		  map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20) ]); */
	}
      if (mode == MODE_SCREEN_TILES)
	{
	  //((x-1) - (x / 12))*50, (x / 12) * 50,
	  sprintf(msg,
		  "Map # %d, (C)opy or (S)tamp tile. ESC to exit to map picker. ENTER to edit hardness. TAB for sprite edit mode. 1-10 for tilescreens. (hold alt, crtl or shift for more) SPACE to show hardness of screen. (I)nfo on sprites."
		  " V to change vision, B to change maps base .C file.",
		  mode);
/* 		  cur_map, */
/* 		  cur_tile, */
/* 		  pam.t[crap].num); */
	}
      if (mode == MODE_TILE_PICKER)
	sprintf(msg,
		"Map # %d - Current tile # %d - ENTER to choose, SPACE to edit hardness.",
		mode,
		cur_map);
/* 		(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)); */
      if (mode == MODE_TILE_HARDNESS)
	{
	  sprintf(msg, "X:%d Y:%d: Density index %d  Z to harden, X to soften, A for low-hard, S for ???.\n"
		  "Alt+X/A/S to fill hardness. Shift+direction for larger brush. ENTER or ESC to exit.",
		  (spr[1].x / 9) -9,(spr[1].y / 9) +1, hard_tile);
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

	  char crap7[80];
	  if (sp_screenmatch) strcpy(crap7, "ScreenMatch is ON."); else strcpy(crap7, "ScreenMatch is OFF");

	  sprintf(msg, "Press ENTER to pickup/putdown sprite. Space to show hardness.  E to edit/pick new sprite. SHIFT to move fast. (S)tamp sprite. ] &"
		  "[ to scale (now at %d). DEL to erase sprite.  Press 1 through 9 to change sprite attributes. (hold alt or shift for more)  Last sprite touched: %d  %s (M to toggle)"
		  "Hold Z or X + arrow keys to trim a sprite. V to change Vision mode. X: %d Y: %d",
		  spr[1].size,last_sprite_added,crap7, spr[1].x, spr[1].y);

	  //lets draw the depth dot




	}


      if (mode == MODE_SPRITE_HARDNESS)
	{


	  if (sp_mode == 0)
	    {
	      sprintf(msg, "Editing depth dot for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini.",
		      sp_seq,sp_frame);

	    }

	  if ( (sp_mode == 1) )

	    {
	      sprintf(msg, "Editing hardbox up left cordinate for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini. X: %d Y: %d",
		      sp_seq,sp_frame, k[seq[sp_seq].frame[sp_frame]].hardbox.left,
		      k[seq[sp_seq].frame[sp_frame]].hardbox.top);

	    }

	  if ( (sp_mode == 2) )

	    {
	      sprintf(msg, "Editing hardbox down right cordinate for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini. X: %d Y: %d",
		      sp_seq,sp_frame, k[seq[sp_seq].frame[sp_frame]].hardbox.right,
		      k[seq[sp_seq].frame[sp_frame]].hardbox.bottom);

	    }






	}


      if (mode == MODE_SCREEN_HARDNESS)
	{
	  int screentile = xy2screentile(spr[1].x, spr[1].y);
	  int sourcetile = pam.t[screentile].square_full_idx0;
	  int defaulthardness = hmap.btile_default[sourcetile];
	  int curhardness = realhard(screentile);
	  char str[100];
	  if (defaulthardness == curhardness)
	    sprintf(str, "Current %03d (default %03d tile %03d)", curhardness, defaulthardness, sourcetile);
	  else
	    sprintf(str, "Current %03d (default %03d tile %03d) (alternate)", curhardness, defaulthardness, sourcetile);
	    
	  sprintf(msg, "Alternative Tile Hardness Selector: Press S to stamp this tiles hardness."
		  "  DEL to remove alternate hardness."
		  "  C to copy from current block."
		  "  [ & ] to cycle."
		  "  ESCAPE to exit.\n"
		  "%s", str);
	}

      {
	int x;
	for (x = 0; x < 256; x++)
	  {
	    if (getkeystate(x))
	      {
		sprintf(msg + strlen(msg), " (Key %i)", x);
	      }
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
	  print_text_wrap(msg, &rcRect, 0, 0, FONT_SYSTEM);
	}

/*       lpDDSBack->ReleaseDC(hdc); */

/*     } /\* GetDC(&hdc) *\/ */


  if ((mode == MODE_MINIMAP))
    {
      if (sjoy.charjustpressed['z'])
	{
	  if (show_display) show_display = /*false*/0;
	  else show_display = /*true*/1;
	}
    }


  if ( (mode == MODE_SCREEN_SPRITES) | (mode == MODE_SCREEN_TILES) )
    {
      /* Show sprites info */
      if (getcharstate('i'))
	{
	  int j;
	  for (j = 1; j < 100; j++)
	    {
	      if (pam.sprite[j].active == /*true*/1)
		{

/* 		  ddbltfx.dwSize = sizeof(ddbltfx); */
/* 		  ddbltfx.dwFillColor = 230; */

		  //info on the sprites  sprite info
/* 		  int temp = s_index[pam.sprite[j].seq].s + pam.sprite[j].frame; */


		  int sprite2 = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y,0,
						pam.sprite[j].seq,pam.sprite[j].frame,
						pam.sprite[j].size);
		  rect_copy(&spr[sprite2].alt , &pam.sprite[j].alt);

		  get_box(sprite2, &box_crap, &box_real);

		  spr[sprite2].active = /*false*/0;

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
	    SDL_FillRect(GFX_lpDDSBack, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 41, 0, 49));
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
	    SDL_FillRect(GFX_lpDDSBack, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 41, 0, 49));
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
	    SDL_FillRect(GFX_lpDDSBack, &dst, SDL_MapRGB(GFX_lpDDSTwo->format, 41, 0, 49));
	  }
	}
    }

  
  if (in_enabled)
    {
      //text window is open, lets act accordingly
      //check_joystick();
      if (getkeystate(SDLK_RETURN) || getkeystate(SDLK_KP_ENTER))
	{
	  //exit text mode
	  
	  if (in_command == 2)
	    {
	      if (in_string != NULL)
		strcpy(in_string, in_temp);
	      else
		log_error("Error, in_char pointer not set, can't issue a value.");
	    }

	  if (in_command == 1)
	    {
	      if (in_int != NULL)
		{
		  char *stop;
		  int in_crap = strtol(in_temp, &stop,10);
		  
		  in_crap2 = in_crap;
		  
		  if ((old_command == INPUT_SCREEN_MIDI) || (old_command == INPUT_SCREEN_TYPE))
		    load_info();
		  
		  *in_int = in_crap2;

		  if ((old_command == INPUT_SCREEN_MIDI) || (old_command == INPUT_SCREEN_TYPE))
		    save_info();
		}
	      else
		{
		  log_error("in_int pointer not set, can't issue a value.");
		}
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
	  
	  in_enabled = 0;
	}
      else if (sjoy.keyjustpressed[SDLK_ESCAPE])
	{
	  in_enabled = 0;
	}
      else if (sjoy.keyjustpressed[SDLK_BACKSPACE])
	//	if (getkey(8)) //this is a much faster backspace than the above
	{
	  if (strlen(in_temp) > 0)
	    in_temp[strlen(in_temp)-1] = 0;

	}
      else if (strlen(in_temp) < in_max)
	{
	  if (isprint(sjoy.last_unicode))
	    sprintf(in_temp + strlen(in_temp), "%c", sjoy.last_unicode);
	}

      if (in_enabled)
	{
	  Say(in_temp,260,200);
	}
      else
	{
	  /* Redraw last mode */
	  if (mode == MODE_MINIMAP)
	    {
	      if (old_command == INPUT_MINIMAP_LOAD)
		draw_minimap_buff();
	      else
		draw_minimap();
	    }
	  else
	    {
	      draw_map();
	    }
	}
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
      in_onflag = /*false*/0;
      in_enabled = /*true*/1;



    }



  //MAIN PAGE FLIP DONE HERE


  if (getkeystate(SDLK_SPACE) && (mode != 1))
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
      flip_it();
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


int load_editor_sounds()
{
  int i;

  if (InitSound() < 0)
    return -1;

  /*
   * Load all sounds
   */
  for(i = 0; i < NUM_SOUND_EFFECTS; i++)
    {
      char *filename = szSoundEffects[i];
      log_info("Loading sound %s [%d]", filename, i);
      
      /* Try from resources */
      SDL_RWops* rwops;
      rwops = find_resource_as_rwops(filename);
      if (rwops != NULL)
	CreateBufferFromWaveFile_RW(rwops, 1, i);
      else
	log_error("Cannot load sound effect %s, from resources or from %s",
		  filename, paths_getpkgdatadir());
    }
  return 1;
}


/*
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
static int doInit(int argc, char *argv[])
{
  dinkedit = 1;

  /* New initialization */
  if (init(argc, argv, "tiles/esplash.bmp") < 0)
    return -1;

  /* Difference with the game: attempt to get a Unicode key state
     (to handle '[' and ']' in a layout-independant way, namely) */
  SDL_EventState(SDL_KEYUP, SDL_ENABLE);
  

  /** SETUP **/
  /* Manually setup basic sequences */
  {
    int i = 1;
    for (; i <= 4; i++)
      {
	spr[i].active = 0;
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
  }

  spr[1].active = /*TRUE*/1;
  spr[1].x = 0;
  spr[1].y = 0;
  spr[1].speed = 20;
  spr[1].brain = 1;
  rect_set(&spr[1].alt,0,0,0,0);
  spr[1].pseq = 10;
  spr[1].pframe = 3;
  spr[1].seq = 0;
  spr[1].seq = 2;
  
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
  
  if (sound_on)
    load_editor_sounds();

  mode = MODE_DIALOG;
  cur_tile = 1;
  load_info();
  
  playl = 20;
  playx = 620;
  playy = 480;
  sp_seq = 0;

  return 0;
}


int main(int argc, char *argv[])
{
  /* Initialize/setup */
  int init_ret = doInit(argc, argv);

  if (init_ret == 0)
    {
      /* Main loop */
      while(1)
	{
	  SDL_Event event;
	  SDL_PumpEvents();
	  
	  /* Check if we need to quit */
	  if (SDL_PeepEvents(&event, 1, SDL_GETEVENT,
			     SDL_EVENTMASK(SDL_QUIT)) > 0)
	    break;
	  
	  /* Fullscreen <-> window */
	  if ((SDL_GetModState()&KMOD_ALT) && GetKeyboard(SDLK_RETURN))
	    {
	      /* Note: as of 2008-08-07, only works under X11 */
	      int success = SDL_WM_ToggleFullScreen(GFX_lpDDSBack);
	      if (!success)
		/* Try using a more portable way: */
		gfx_toggle_fullscreen();
	    }
	  
	  updateFrame();

	  /* Clean-up finished sounds: normally this is done by
	     SDL_mixer but since we're using effects tricks to
	     stream&resample sounds, we need to do this manually. */
	  sfx_cleanup_finished_channels();
	}
    }

  /* Uninitialize/clean-up */
  finiObjects();

  if (init_ret < 0)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}
