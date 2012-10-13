/**
 * Screen sprites and hardness

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009  Sylvain Beucler

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

#include <string.h>

#include "dinkvar.h"
#include "screen.h"
#include "gfx.h"
#include "sfx.h"
#include "log.h"

/**
 * Fills a int[MAX_SPRITES_EDITOR] with the index of the current
 * screen's sprites, sorted by ascending height/queue.
 */
void screen_rank_map_sprites(int* rank)
{
  memset(rank, 0, MAX_SPRITES_EDITOR * sizeof(int));

  int r1 = 0;
  int already_checked[MAX_SPRITES_EDITOR+1];
  memset(already_checked, 0, sizeof(already_checked));
  for (r1 = 0; r1 < MAX_SPRITES_EDITOR; r1++)
    {
      int highest_sprite = 22000; //more than it could ever be
      rank[r1] = 0;
      
      int h1;
      for (h1 = 1; h1 <= MAX_SPRITES_EDITOR; h1++)
	{
	  if (already_checked[h1] == 0 && pam.sprite[h1].active)
	    {
	      int height;
	      if (pam.sprite[h1].que != 0)
		height = pam.sprite[h1].que;
	      else
		height = pam.sprite[h1].y;
	      
	      if (height < highest_sprite)
		{
		  highest_sprite = height;
		  rank[r1] = h1;
		}
	    }
	}
      if (rank[r1] != 0)
	already_checked[rank[r1]] = 1;
    }
}

/**
 * Fills a int[MAX_SPRITES_AT_ONCE] with the index of the current
 * screen's sprites, sorted by ascending height/queue.
 */
void screen_rank_game_sprites(int* rank)
{
  memset(rank, 0, MAX_SPRITES_AT_ONCE * sizeof(int));

  int r1 = 0;
  int already_checked[MAX_SPRITES_AT_ONCE+1];
  memset(already_checked, 0, sizeof(already_checked));
  for (r1 = 0; r1 < last_sprite_created; r1++)
    {
      int highest_sprite = 22000; //more than it could ever be
      rank[r1] = 0;

      int h1;
      for (h1 = 1; h1 <= last_sprite_created; h1++)
	{
	  if (already_checked[h1] == 0 && spr[h1].active)
	    {
	      int height;
	      if (spr[h1].que != 0)
		height = spr[h1].que;
	      else
		height = spr[h1].y;
	      
	      if (height < highest_sprite)
		{
		  highest_sprite = height;
		  rank[r1] = h1;
		}
	    }
	}
      if (rank[r1] != 0)
	already_checked[rank[r1]] = 1;
    }
}

/**
 * Load screen sprites: draw sprites on background buffer, ordered by queue
 * and configure the others (sounds, scripts, etc.).
 *
 * Also cf. cf. place_sprites_game_background(...) and editor's
 * place_sprites(...).
 */
void place_sprites_game()
{
  update_play_changes();
  
  int rank[MAX_SPRITES_EDITOR];
  screen_rank_map_sprites(rank);
  
  int r1 = 0;  
  for (; r1 < MAX_SPRITES_EDITOR && rank[r1] > 0; r1++)
    {
      //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
      int j = rank[r1];
      
      if (pam.sprite[j].active == 1
	  && (pam.sprite[j].vision == 0 || pam.sprite[j].vision == *pvision))
	{
	  check_seq_status(pam.sprite[j].seq);
	  
	  //we have instructions to make a sprite
	  if (pam.sprite[j].type == 0 || pam.sprite[j].type == 2)
	    {
	      //make it part of the background (much faster)
	      int sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y, 0,
					   pam.sprite[j].seq,pam.sprite[j].frame,
					   pam.sprite[j].size);

	      spr[sprite].hard = pam.sprite[j].hard;
	      spr[sprite].sp_index = j;
	      rect_copy(&spr[sprite].alt , &pam.sprite[j].alt);
	      
	      check_sprite_status_full(sprite);

	      if (pam.sprite[j].type == 0)
		draw_sprite_game(GFX_lpDDSTwo, sprite);
	      
	      if (spr[sprite].hard == 0)
		{
		  /*if (pam.sprite[j].is_warp == 0)
		    add_hardness(sprite, 1); else */
		  add_hardness(sprite, 100 + j);
		}
	      spr[sprite].active = 0;
	    }

	  if (pam.sprite[j].type == 1)
	    {
	      //make it a living sprite
	      int sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y, 0,
					   pam.sprite[j].seq,pam.sprite[j].frame,
					   pam.sprite[j].size);
	      
	      spr[sprite].hard = pam.sprite[j].hard;
	      
	      //assign addition parms to the new sprite
	      spr[sprite].sp_index = j;
	      
	      spr[sprite].brain = pam.sprite[j].brain;
	      spr[sprite].speed = pam.sprite[j].speed;
	      spr[sprite].base_walk = pam.sprite[j].base_walk;
	      spr[sprite].base_idle = pam.sprite[j].base_idle;
	      spr[sprite].base_attack = pam.sprite[j].base_attack;
	      spr[sprite].base_hit = pam.sprite[j].base_hit;
	      spr[sprite].hard = pam.sprite[j].hard;
	      spr[sprite].timer = pam.sprite[j].timer;
	      spr[sprite].que = pam.sprite[j].que;
	      
	      
	      spr[sprite].sp_index = j;
	      
	      rect_copy(&spr[sprite].alt , &pam.sprite[j].alt);
	      
	      spr[sprite].base_die = pam.sprite[j].base_die;
	      spr[sprite].strength = pam.sprite[j].strength;
	      spr[sprite].defense = pam.sprite[j].defense;
	      spr[sprite].gold = pam.sprite[j].gold;
	      spr[sprite].exp = pam.sprite[j].exp;
	      spr[sprite].nohit = pam.sprite[j].nohit;
	      spr[sprite].touch_damage = pam.sprite[j].touch_damage;
	      spr[sprite].hitpoints = pam.sprite[j].hitpoints;
	      spr[sprite].sound = pam.sprite[j].sound;
	      check_sprite_status_full(sprite);
	      if (pam.sprite[j].is_warp == 0 && spr[sprite].sound != 0)
		{
		  //make looping sound
		  log_debug("making sound with sprite %d..", sprite);
		  SoundPlayEffect( spr[sprite].sound,22050, 0,sprite, 1);
		}
	      if (spr[sprite].brain == 3)
		{
		  // Duck
		  check_seq_status(21);
		  check_seq_status(23);
		  check_seq_status(24);
		  check_seq_status(26);
		  check_seq_status(27);
		  check_seq_status(29);
		  // Headless duck
		  check_seq_status(111);
		  check_seq_status(113);
		  check_seq_status(117);
		  check_seq_status(119);
		  // Duck head
		  check_seq_status(121);
		  check_seq_status(123);
		  check_seq_status(127);
		  check_seq_status(129);
		}
	      
	      if (spr[sprite].hard == 0)
		{
		  /*  if (pam.sprite[j].is_warp == 0)
			add_hardness(sprite, 1);
		      else */
		  add_hardness(sprite, 100+j);
		}
	      
	      //does it need a script loaded?
	      if (strlen(pam.sprite[j].script) > 1)
		{
		  spr[sprite].script = load_script(pam.sprite[j].script, sprite, /*true*/1);
		}
	    }
	  //Msg("I just made sprite %d because rank[%d] told me to..",sprite,j);
					       }
    }
}

/**
 * Draw background sprites and background (not looking at
 * non-background sprites), ordered by queue.
 * 
 * Also cf. place_sprites_game(...) and editor's place_sprites(...).
 */
void place_sprites_game_background()
{
  int rank[MAX_SPRITES_EDITOR];
  screen_rank_map_sprites(rank);

  int r1 = 0;
  for (; r1 < MAX_SPRITES_EDITOR && rank[r1] > 0; r1++)
    {
      //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
      int j = rank[r1];
      
      if (pam.sprite[j].active == 1
	  && (pam.sprite[j].vision == 0 || pam.sprite[j].vision == *pvision))
	{
	  if (pam.sprite[j].type == 0)
	    {
	      //we have instructions to make a sprite
	      check_seq_status(pam.sprite[j].seq);
	      
	      //make it part of the background (much faster)
	      int sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y, 0,
					   pam.sprite[j].seq,pam.sprite[j].frame,
					   pam.sprite[j].size);

	      check_sprite_status_full(sprite);
	      draw_sprite_game(GFX_lpDDSTwo, sprite);
	      spr[sprite].active = 0;
	    }
	}
    }
}


void fill_back_sprites()
{
  int rank[MAX_SPRITES_EDITOR];
  screen_rank_map_sprites(rank);

  int r1 = 0;
  for (; r1 < MAX_SPRITES_EDITOR && rank[r1] > 0; r1++)
    {
      //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
      int j = rank[r1];

      if (pam.sprite[j].active == 1
	  && (pam.sprite[j].vision == 0 || pam.sprite[j].vision == *pvision))
	{



	  if (pam.sprite[j].type != 1 && pam.sprite[j].hard == 0)
	    {
	      //make it part of the background (much faster)
	      int sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y, 0,
					   pam.sprite[j].seq,pam.sprite[j].frame,
					   pam.sprite[j].size);

	      spr[sprite].hard = pam.sprite[j].hard;
	      spr[sprite].sp_index = j;
	      rect_copy(&spr[sprite].alt , &pam.sprite[j].alt);

	      check_sprite_status_full(sprite);




	      if (spr[sprite].hard == 0)
		{
		  /*if (pam.sprite[j].is_warp == 0)
		    add_hardness(sprite, 1); else */
		  add_hardness(sprite,100+j);
		}
	      spr[sprite].active = 0;
	    }
	}
    }
}


void fill_hard_sprites()
{
  int rank[MAX_SPRITES_AT_ONCE];
  screen_rank_game_sprites(rank);

  int r1 = 0;
  for (; r1 < last_sprite_created && rank[r1] > 0; r1++)
    {
      int h = rank[r1];
      if (spr[h].active)
	{
	  // Msg("proccesing sprite %d", h);
	  if (spr[h].sp_index != 0)
	    {
	      //Msg("has spindex of %d is_warp is %d",spr[h].sp_index,pam.sprite[spr[h].sp_index].is_warp);
	      if (pam.sprite[spr[h].sp_index].hard == 0)
		{
		  add_hardness(h,100+spr[h].sp_index);
		  //Msg("added warp hardness for %d", spr[h].sp_index);
		}
	    }
	  else
	    {
	      if (spr[h].hard == 0)
		{
		  //Msg("adding a new sprite hardness %d (not from editor)", h);
		  add_hardness(h, 1);
		}
	    }
	}
    }
}
