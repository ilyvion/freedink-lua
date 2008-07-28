/**
 * Header for code common to FreeDink and FreeDinkedit

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2008  Sylvain Beucler

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

#include "game_engine.h"
#include "dinkvar.h" /* hmap, pam */

struct sp spr[MAX_SPRITES_AT_ONCE]; //max sprite control systems at once
int last_sprite_created;

/* Engine variables directly mapped with DinkC variables */
int *pvision, *plife, *presult, *pspeed, *ptiming, *plifemax,
  *pexper, *pmap, *pstrength, *pcur_weapon,*pcur_magic, *pdefense,
  *pgold, *pmagic, *plevel, *plast_text, *pmagic_level;
int *pupdate_status, *pmissile_target, *penemy_sprite,
  *pmagic_cost, *pmissle_source;


int flife, fexp, fstrength, fdefense, fgold, fmagic, fmagic_level, flifemax, fraise, last_magic_draw;

int fcur_weapon, fcur_magic;

/* Sound - BGM */
int sound_on = 1;
int cd_inserted;
int midi_active = 1;



struct map_info map;



//JOYINFOEX jinfo; //joystick info
SDL_Joystick* jinfo;
int joystick = /*true*/1;

struct wait_for_button wait4b;



int dversion = 108;
char* dversion_string;

int last_saved_game = 0;
char save_game_info[200] = "Level &level";
char current_map[50] = "map.dat";
char current_dat[50] = "dink.dat";

time_t time_start;


int smooth_follow = 0;



/** Fadedown / fadeup **/
/* Tell the engine that we're warping */
/* (quick fadedown + black + fadeup) */
int process_warp = 0;
/* Tell the engine that we're fading down */
/* or fading up */
int process_downcycle = 0;
int process_upcycle = 0;
/* When the fadedown must finish: */
unsigned long cycle_clock = 0;
/* What script to resume after fade: */
int cycle_script = 0;
/* True color fade in [0,256]; 0 is completely dark, 256 is unaltered */
double truecolor_fade_brightness = 256;
/* Time elapsed since last fade computation; -1 is disabled */
Uint32 truecolor_fade_lasttick = -1;

/* Base for Dink's push sequences */
unsigned int dink_base_push = 310;

void game_init()
{
  /* Clean the game state structure - done by C++ but not
     automatically done by C, and this causes errors. TODO: fix the
     errors properly instead of using this dirty trick. */
  memset(&play, 0, sizeof(play));

  /* Precaution */
  memset(&hmap, 0, sizeof(hmap));
  memset(&pam, 0, sizeof(pam));
  memset(&spr, 0, sizeof(spr));

  if (dversion >= 108)
    dversion_string = "v1.08";
  else
    dversion_string = "v1.07";
}

void game_quit()
{
  int i = 0;
  for (i = 1; i < MAX_SPRITES_AT_ONCE; i++)
    if (spr[i].custom != NULL)
      dinkc_sp_custom_free(spr[i].custom);
}

/* Sound - SFX */
        int get_pan(int h)
        {

                int pan = 0;

                int x1 = 320;


                //uncomment to allow math to be done from Dink's current location
                //x1 = spr[1].x;


                if (spr[h].active)
                {
                        if (spr[h].x > x1) pan += (spr[h].x - x1) * 6;
                        if (x1 > spr[h].x) pan -= (x1 - spr[h].x) * 6;

                }


                if (pan > 10000) pan = 10000;
                if (pan < -10000) pan = -10000;


                return(pan);


        }

        int get_vol(int h)
        {

                int pan = 0;
                int pan2 = 0;

                if (spr[h].active)
                {
                        if (spr[h].x > spr[1].x) pan -= (spr[h].x - spr[1].x) * 4;


                        if (spr[1].x > spr[h].x) pan -= (spr[1].x - spr[h].x) * 4;


                        if (spr[h].y > spr[1].y) pan2 -= (spr[h].y - spr[1].y) * 4;

                        if (spr[1].y > spr[h].y) pan2 -= (spr[1].y - spr[h].y) * 4;


                        //Msg("pan %d, pan2 %d", pan, pan2);

                        if (pan2 < pan) pan = pan2;

                }


                if (pan > -100) pan = 0;

                if (pan < -10000) pan = -10000;


                return(pan);


        }
