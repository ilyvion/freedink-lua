/**
 * Link game engine and DinkC script engine

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2005, 2007, 2008  Sylvain Beucler

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

#include "dinkc_bindings.h"

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <alloca.h>

#include <ffi.h>

/* Gnulib */
#include "hash.h"

#include "game_engine.h"
#include "dinkvar.h"
#include "dinkc.h"
#include "freedink.h"
#include "gfx.h"
#include "gfx_fonts.h"
#include "gfx_sprites.h"
#include "gfx_tiles.h"
#include "bgm.h"
#include "sfx.h"
#include "str_util.h"
#include "paths.h"
#include "log.h"
#include "dinkc_console.h"
#include "i18n.h"

/* store current procedure arguments expanded values of type 'int' (see get_parms) */
static long nlist[10];
/* store current procedure arguments of type 'string' (idem) */
static char slist[10][200];
static char* cur_funcname;


/***************/
/*  DinkC API  */
/*             */
/***************/

/**
 * Short-hand to check for invalid sprites and avoid segfaults.
 * Also warn the D-Mod author about it.
 */
#define STOP_IF_BAD_SPRITE(sprite)                                             \
  if (sprite <= 0 || sprite >= MAX_SPRITES_AT_ONCE)                            \
    {                                                                          \
      Msg("%s:%s: DinkC error: invalid sprite %d (offset %d)",                 \
          rinfo[script]->name, cur_funcname, sprite, rinfo[script]->current);  \
      return;                                                                  \
    }

/**
 * sp_* functions used to call 'change_sprite' on spr[sprite] without
 * checking if 'sprite' was in [1; MAX_SPRITES_AT_ONCE-1]. Since
 * 'change_sprite' returns -1 when 'sprite' is inactive, that's also
 * what we return when the sprite is out of range.
 */
#define RETURN_NEG_IF_BAD_SPRITE(sprite)                                       \
  if (sprite <= 0 || sprite >= MAX_SPRITES_AT_ONCE)                            \
    {                                                                          \
      Msg("%s:%s: DinkC error: invalid sprite %d (offset %d)",                 \
          rinfo[script]->name, cur_funcname, sprite, rinfo[script]->current);  \
      *preturnint = -1;                                                        \
      return;                                                                  \
    }


void dc_sp_active(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].active);
}

void dc_sp_attack_hit_sound(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].attack_hit_sound);
}

void dc_sp_attack_hit_sound_speed(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].attack_hit_sound_speed);
}

void dc_sp_attack_wait(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg+thisTickCount, &spr[sprite].attack_wait);
}

void dc_sp_base_attack(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].base_attack);
}

void dc_sp_base_die(int script, int* yield, int* preturnint, int sprite, int base_sequence)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, base_sequence, &spr[sprite].base_die);
}

void dc_sp_base_hit(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].base_hit);
}

void dc_sp_base_idle(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].base_idle);
}

void dc_sp_base_walk(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].base_walk);
}

void dc_sp_brain(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].brain);
}

void dc_sp_brain_parm(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].brain_parm);
}

void dc_sp_brain_parm2(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].brain_parm2);
}

void dc_sp_defense(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].defense);
}

void dc_sp_dir(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].dir);
  if (sparg != -1)
    changedir(spr[sprite].dir, sprite, spr[sprite].base_walk);
}

void dc_sp_disabled(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].disabled);
}

void dc_sp_distance(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].distance);
}

void dc_sp_exp(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].exp);
}

void dc_sp_flying(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].flying);
}

void dc_sp_follow(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].follow);
}

void dc_sp_frame(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].frame);
}

void dc_sp_frame_delay(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].frame_delay);
}

void dc_sp_gold(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].gold);
}

void dc_sp_hard(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].hard);
  if (spr[sprite].sp_index != 0 && sparg != -1)
    pam.sprite[spr[sprite].sp_index].hard = *preturnint;
}

void dc_sp_hitpoints(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].hitpoints);
}

void dc_sp_move_nohard(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].move_nohard);
}

void dc_sp_mx(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].mx);
}

void dc_sp_my(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].my);
}

void dc_sp_noclip(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].noclip);
}

void dc_sp_nocontrol(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].nocontrol);
}

void dc_sp_nodraw(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].nodraw);
}

void dc_sp_nohit(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].nohit);
}

void dc_sp_notouch(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].notouch);
}

void dc_sp_pframe(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].pframe);
}

void dc_sp_picfreeze(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].picfreeze);
}

void dc_sp_pseq(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].pseq);
}

void dc_sp_que(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].que);
}

void dc_sp_range(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].range);
}

void dc_sp_reverse(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].reverse);
}

void dc_sp_seq(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].seq);
}

void dc_sp_size(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].size);
}

void dc_sp_sound(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].sound);
  if (sparg > 0)
    SoundPlayEffect(spr[sprite].sound,22050, 0, sprite, 1);
}

void dc_sp_speed(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].speed);
  if (sparg != -1)
    changedir(spr[sprite].dir, sprite, spr[sprite].base_walk);
}

void dc_sp_strength(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].strength);
}

void dc_sp_target(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].target);
}

void dc_sp_timing(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].timer);
}

void dc_sp_touch_damage(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].touch_damage);
}

void dc_sp_x(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].x);
}

void dc_sp_y(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].y);
}



void dc_sp_kill(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  STOP_IF_BAD_SPRITE(sprite);
  spr[sprite].kill = sparg;
}

void dc_sp_editor_num(int script, int* yield, int* preturnint, int sprite)
{
  *preturnint = 0;
  if (sprite > 0 && sprite < MAX_SPRITES_AT_ONCE)
    *preturnint = spr[sprite].sp_index;
  else
    Msg("Error: sp_editor_num: invalid sprite %d", sprite);
}


void dc_sp_kill_wait(int script, int* yield, int* preturnint, int sprite)
{
  if (sprite > 0 && sprite < MAX_SPRITES_AT_ONCE)
    spr[sprite].wait = 0;
  else
    Msg("Error: sp_kill_wait: invalid sprite %d", sprite);
}

void dc_sp_script(int script, int* yield, int* preturnint, int sprite, char* dcscript)
{
  // (sprite, direction, until, nohard);
  if (sprite <= 0 || (sprite >= MAX_SPRITES_AT_ONCE && sprite != 1000))
    {
      Msg("Error: sp_script cannot process sprite %d??", sprite);
      return;
    }
  kill_scripts_owned_by(sprite);
  if (load_script(dcscript, sprite, /*true*/1) == 0)
    {
      *preturnint = 0;
      return;
    }

  int tempreturn = 0;
  if (sprite != 1000)
    {
      if (no_running_main == /*true*/1)
	Msg("Not running %s until later..", rinfo[spr[sprite].script]->name);
      if (no_running_main == /*false*/0 && sprite != 1000)
	locate(spr[sprite].script, "MAIN");
    
      tempreturn = spr[sprite].script;
    
      if (no_running_main == /*false*/0)
	run_script(spr[sprite].script);
    }
    
  *preturnint = tempreturn;
}


void dc_unfreeze(int script, int* yield, int* preturnint, int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);

  if (spr[sprite].active)
    spr[sprite].freeze = 0;
  else
    Msg("Couldn't unfreeze sprite %d in script %d, it doesn't exist.", sprite, script);
}

void dc_freeze(int script, int* yield, int* preturnint, int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);

  if (spr[sprite].active)
    spr[sprite].freeze = script;
  else
    Msg("Couldn't freeze sprite %d in script %d, it doesn't exist.", sprite, script);
}

void dc_set_callback_random(int script, int* yield, int* preturnint, char* procedure, int base, int range)
{
  int retval = add_callback(procedure, base, range, script);
  if (dversion >= 108)
    *preturnint = retval;
}

void dc_set_dink_speed(int script, int* yield, int* preturnint, int speed)
{
  if (dversion >= 108 && speed == 0)
    ; // do nothing
  else
    dinkspeed = speed;
}

void dc_reset_timer(int script, int* yield, int* preturnint)
{
  time(&time_start);
  play.minutes = 0;
}

void dc_set_keep_mouse(int script, int* yield, int* preturnint, int keep_mouse_p)
{
  keep_mouse = keep_mouse_p;
}

void dc_add_item(int script, int* yield, int* preturnint, char* dcscript, int sequence, int frame)
{
  add_item(dcscript, sequence, frame, ITEM_REGULAR);
}

void dc_add_magic(int script, int* yield, int* preturnint, char* dcscript, int sequence, int frame)
{
  add_item(dcscript, sequence, frame, ITEM_MAGIC);
}

void dc_add_exp(int script, int* yield, int* preturnint, int amount, int active_sprite)
{
  STOP_IF_BAD_SPRITE(active_sprite);

  if (dversion >= 108)
    // fix - made work with all sprites when
    // using add_exp DinkC command
    add_exp_force(amount, active_sprite);
  else
    add_exp(amount, active_sprite);
}

void dc_kill_this_item(int script, int* yield, int* preturnint, char* dcscript)
{
  kill_cur_item_script(dcscript);
}

void dc_kill_this_magic(int script, int* yield, int* preturnint, char* dcscript)
{
  kill_cur_magic_script(dcscript);
}

void dc_show_bmp(int script, int* yield, int* preturnint, char* bmp_file, int show_map_dot, int unused)
{
  Msg("showing BMP");
  wait4b.active = /*false*/0;
  show_bmp(bmp_file, show_map_dot, script);
  *yield = 1;
}

void dc_copy_bmp_to_screen(int script, int* yield, int* preturnint, char* bmp_file)
{
  Msg("copying BMP");
  copy_bmp(bmp_file);
}

void dc_wait_for_button(int script, int* yield, int* preturnint)
{
  Msg("waiting for button with script %d", script);
  wait4b.script = script;
  wait4b.active = /*true*/1;
  wait4b.button = 0;
  *yield = 1;
}

void dc_stop_wait_for_button(int script, int* yield, int* preturnint)
{
  wait4b.active = /*false*/0;
}

void dc_load_screen(int script, int* yield, int* preturnint)
{
  /* STOP_IF_BAD_SPRITE(active_sprite); */

  //Msg("Loading map %d..",*pmap);
  update_screen_time();
  load_map(map.loc[*pmap]);

  // update indicator on mini-map
  if (map.indoor[*pmap] == 0)
    play.last_map = *pmap;
    
  return;
}

void dc_say(int script, int* yield, int* preturnint, char* text, int active_sprite)
{
  /* 1000 is a valid value, and bad values don't trigger segfaults
     in this particular function; so don't validate active_sprite */
  /* STOP_IF_BAD_SPRITE(active_sprite); */

  if (active_sprite == 0)
    {
      Msg("Say_stop error:  Sprite 0 can talk? Yeah, didn't think so.");
      return;
    }

  if (active_sprite != 1000)
    kill_text_owned_by(active_sprite);

  /* Translate text (before variable substitution) */
  if (strlen(text) >= 2 && text[0] == '`')
    i18n_translate(text+2, 200-2);
  else
    i18n_translate(text, 200);

  decipher_string(text, script);
  *preturnint = say_text(text, active_sprite, script);
}

void dc_say_stop(int script, int* yield, int* preturnint, char* text, int active_sprite)
{
  /* STOP_IF_BAD_SPRITE(active_sprite); */

  if (active_sprite == 0)
    {
      Msg("Say_stop error:  Sprite 0 can talk? Yeah, didn't think so.");
      return;
    }
    
  kill_text_owned_by(active_sprite);
  kill_text_owned_by(1);
  kill_returning_stuff(script);

  /* Translate text (before variable substitution) */
  if (strlen(text) >= 2 && text[0] == '`')
    i18n_translate(text+2, 200-2);
  else
    i18n_translate(text, 200);

  decipher_string(text, script);
  int sprite = say_text(text, active_sprite, script);
  *preturnint = sprite;
  spr[sprite].callback = script;
  play.last_talk = script;
  //Msg("Sprite %d marked callback true.", sprite);
    
  *yield = 1;
}

void dc_say_stop_npc(int script, int* yield, int* preturnint, char* text, int active_sprite)
{
  /* STOP_IF_BAD_SPRITE(active_sprite); */

  /* no-op if already talking */
  if (text_owned_by(active_sprite))
    {
      *preturnint = 0;
      return;
    }
    
  kill_returning_stuff(script);

  /* Translate text (before variable substitution) */
  if (strlen(text) >= 2 && text[0] == '`')
    i18n_translate(text+2, 200-2);
  else
    i18n_translate(text, 200);

  decipher_string(text, script);
  int sprite = say_text(text, active_sprite, script);
  *preturnint = sprite;
  spr[sprite].callback = script;
    
  *yield = 1;
}

void dc_say_stop_xy(int script, int* yield, int* preturnint, char* text, int x, int y)
{
  kill_returning_stuff(script);

  /* Translate text (before variable substitution) */
  if (strlen(text) >= 2 && text[0] == '`')
    i18n_translate(text+2, 200-2);
  else
    if (strlen(text) >= 2 && text[0] == '`')
    i18n_translate(text+2, 200-2);
  else
    i18n_translate(text, 200);

  Msg("Say_stop_xy: Adding %s", text);
  decipher_string(text, script);
  int sprite = say_text_xy(text, x, y, script);
  spr[sprite].callback = script;
  spr[sprite].live = /*true*/1;
  play.last_talk = script;
  *yield = 1;
}

void dc_say_xy(int script, int* yield, int* preturnint, char* text, int x, int y)
{
  kill_returning_stuff(script);

  /* Translate text (before variable substitution) */
  if (strlen(text) >= 2 && text[0] == '`')
    i18n_translate(text+2, 200-2);
  else
    i18n_translate(text, 200);

  decipher_string(text, script);
  int sprite = say_text_xy(text, x, y, script);
  *preturnint = sprite;
}

void dc_draw_screen(int script, int* yield, int* preturnint)
{
  draw_map_game();
  /* only refresh screen if not in a cut-scene */
  if (rinfo[script]->sprite != 1000)
    *yield = 1;
}

void dc_free_items(int script, int* yield, int* preturnint)
{
  int i;
  *preturnint = 0;
  for (i = 1; i < 17; i ++)
    {
      if (play.item[i].active == /*false*/0)
	{
	  *preturnint += 1;
	}
    }
  return;
}

void dc_free_magic(int script, int* yield, int* preturnint)
{
  int i;
  *preturnint = 0;

  for (i = 1; i < 9; i ++)
    {
      if (play.mitem[i].active == /*false*/0)
	{
	  *preturnint += 1;
	}
    }
  return;
}

void dc_kill_cur_item(int script, int* yield, int* preturnint)
{
  *preturnint = 0;
  kill_cur_item();
  *yield = 1;
}

void dc_kill_cur_magic(int script, int* yield, int* preturnint)
{
  *preturnint = 0;
  kill_cur_magic();
  *yield = 1;
}

void dc_draw_status(int script, int* yield, int* preturnint)
{
  draw_status_all();
  return;
}

void dc_arm_weapon(int script, int* yield, int* preturnint)
{
  if (weapon_script != 0 && locate(weapon_script, "DISARM"))
    run_script(weapon_script);

  weapon_script = load_script(play.item[*pcur_weapon].name, 1000, /*false*/0);
  if (locate(weapon_script, "ARM"))
    run_script(weapon_script);

  return;
}

void dc_arm_magic(int script, int* yield, int* preturnint)
{
  if (magic_script != 0 && locate(magic_script, "DISARM"))
    run_script(magic_script);
    
  magic_script = load_script(play.mitem[*pcur_magic].name, 1000, /*false*/0);
  if (locate(magic_script, "ARM"))
    run_script(magic_script);
    
  return;
}

void dc_restart_game(int script, int* yield, int* preturnint)
{
  int u;
  int mainscript;
  while (kill_last_sprite());
  kill_repeat_sounds_all();
  kill_all_scripts_for_real();
  mode = 0;
  screenlock = 0;
  kill_all_vars();
  memset(&hm, 0, sizeof(hm));
  for (u = 1; u <= 10; u++)
    play.button[u] = u;
  mainscript = load_script("main", 0, /*true*/1);
    
  locate(mainscript, "main");
  run_script(mainscript);
  //lets attach our vars to the scripts
  attach();
  *yield = 1;
}

void dc_wait(int script, int* yield, int* preturnint, int delayms)
{
  kill_returning_stuff(script);
  add_callback("", delayms, 0, script);
  *yield = 1;
}

void dc_preload_seq(int script, int* yield, int* preturnint, int sequence)
{
  check_seq_status(sequence);
}

void dc_script_attach(int script, int* yield, int* preturnint, int sprite)
{
  /* STOP_IF_BAD_SPRITE(sprite); */
  rinfo[script]->sprite = sprite;
}

void dc_draw_hard_sprite(int script, int* yield, int* preturnint, int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);

  update_play_changes();
  int l = sprite;
  rect mhard;
  rect_copy(&mhard, &k[seq[spr[l].pseq].frame[spr[l].pframe]].hardbox);
  rect_offset(&mhard, (spr[l].x- 20), spr[l].y);

  fill_hardxy(mhard);
  fill_back_sprites();
  fill_hard_sprites();
}


void dc_activate_bow(int script, int* yield, int* preturnint)
{
  spr[1].seq = 0;
  spr[1].pseq = 100+spr[1].dir;
  spr[1].pframe = 1;
  bow.active = /*true*/1;
  bow.script = script;
  bow.hitme = /*false*/0;
  bow.time = 0;
    
  /*      bowsound->Release();
    
  //lpDS->DuplicateSoundBuffer(ssound[42].sound,&bowsound);
  //bowsound->Play(0, 0, DSBPLAY_LOOPING);
  */
    
  *yield = 1;
}

void dc_disable_all_sprites(int script, int* yield, int* preturnint)
{
  int jj;
  for (jj = 1; jj < last_sprite_created; jj++)
    if (spr[jj].active) spr[jj].disabled = /*true*/1;
}

void dc_draw_background(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  draw_map_game_background();
}

void dc_draw_hard_map(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  Msg("Drawing hard map..");
  update_play_changes();
  fill_whole_hard();
  fill_hard_sprites();
  fill_back_sprites();
}

void dc_enable_all_sprites(int script, int* yield, int* preturnint)
{
  int jj;
  for (jj = 1; jj < last_sprite_created; jj++)
    if (spr[jj].active) spr[jj].disabled = /*false*/0;
}

void dc_fade_down(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  process_downcycle = /*true*/1;
  cycle_clock = thisTickCount+1000;
  cycle_script = script;
  *yield = 1;
}

void dc_fade_up(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  process_upcycle = /*true*/1;
  cycle_script = script;
  *yield = 1;
}

void dc_get_burn(int script, int* yield, int* preturnint)
{
  *preturnint = 1;
}

void dc_get_last_bow_power(int script, int* yield, int* preturnint)
{
  *preturnint = bow.last_power;
}

void dc_get_version(int script, int* yield, int* preturnint)
{
  *preturnint = dversion;
}

void dc_kill_all_sounds(int script, int* yield, int* preturnint)
{
  kill_repeat_sounds_all();
}

void dc_kill_game(int script, int* yield, int* preturnint)
{
  Msg("Was told to kill game, so doing it like a good boy.");
  /* Send QUIT event to the main game loop,
     which will cleanly exit */
  SDL_Event ev;
  ev.type = SDL_QUIT;
  SDL_PushEvent(&ev);
  *yield = 1;
}

void dc_kill_this_task(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  if (rinfo[script]->proc_return != 0)
    {
      run_script(rinfo[script]->proc_return);
    }
  kill_script(script);
  *yield = 1;
}

void dc_scripts_used(int script, int* yield, int* preturnint)
{
  int m = 0;
  int i;
  for (i = 1; i < MAX_SCRIPTS; i++)
    if (rinfo[i] != NULL) m++;
  *preturnint = m;
}

void dc_stopcd(int script, int* yield, int* preturnint)
{
  // mciSendCommand(CD_ID, MCI_CLOSE, 0, NULL);
  Msg("Stopped cd");
  killcd();
}

void dc_stopmidi(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  StopMidi();
}

void dc_turn_midi_off(int script, int* yield, int* preturnint)
{
  midi_active = /*false*/0;
}

void dc_turn_midi_on(int script, int* yield, int* preturnint)
{
  midi_active = /*true*/1;
}

void dc_count_item(int script, int* yield, int* preturnint, char* dcscript)
{
  int i;
  *preturnint = 0;
  for (i = 1; i < 17; i++)
    {
      if (play.item[i].active
	  && compare(play.item[i].name, dcscript))
	returnint++;
    }
}

void dc_count_magic(int script, int* yield, int* preturnint, char* dcscript)
{
  int i;
  *preturnint = 0;
  for (i = 1; i < 9; i++)
    {
      if (play.mitem[i].active
	  && compare(play.mitem[i].name, dcscript))
	returnint++;
    }
}

void dc_compare_sprite_script(int script, int* yield, int* preturnint, int sprite, char* dcscript)
{
  *preturnint = 0;
  STOP_IF_BAD_SPRITE(sprite);
 
  if (spr[sprite].active)
    {
      if (spr[sprite].script == 0)
	{
	  Msg("Compare sprite script says: Sprite %d has no script.", sprite);
	  return;
	}
      if (rinfo[spr[sprite].script] == NULL)
	{
	  Msg("Compare sprite script says: script %d for sprite %d was already killed!.",
	      sprite, spr[sprite].script);
	  return;
	}
      if (compare(dcscript, rinfo[spr[sprite].script]->name))
	{
	  *preturnint = 1;
	  return;
	}
    }
  else
    {
      Msg("Can't compare sprite script, sprite not active.");
    }
}



void dc_compare_weapon(int script, int* yield, int* preturnint, char* dcscript)
{
  *preturnint = 0;
  if (*pcur_weapon == 0)
    return;

  if (compare(play.item[*pcur_weapon].name, dcscript))
    *preturnint = 1;
}

void dc_compare_magic(int script, int* yield, int* preturnint, char* dcscript)
{
  *preturnint = 0;
  if (*pcur_magic == 0)
    return;
 
  if (dversion >= 108)
    {
      if (compare(play.mitem[*pcur_magic].name, dcscript))
	*preturnint = 1;
    }
  else
    {
      /* reproduce v1.07 bug: compare with regular item rather than
	 magic item */
      if (compare(play.item[*pcur_magic].name, dcscript))
	*preturnint = 1;
    }
}

void dc_init(int script, int* yield, int* preturnint, char* dink_ini_line)
{
  figure_out(dink_ini_line);
}

void dc_dink_can_walk_off_screen(int script, int* yield, int* preturnint, int can_walk_off_screen_p)
{
  walk_off_screen = can_walk_off_screen_p;
}

void dc_push_active(int script, int* yield, int* preturnint, int dink_can_push_p)
{
  push_active = dink_can_push_p;
}

void dc_stop_entire_game(int script, int* yield, int* preturnint, int stop_p)
{
  stop_entire_game = stop_p;
  SDL_BlitSurface(GFX_lpDDSBack, NULL, GFX_lpDDSTwo, NULL);
}


void dc_editor_type(int script, int* yield, int* preturnint, int editor_sprite, int type)
{
  if (editor_sprite < 0 || editor_sprite >= 100)
    return;
  *preturnint = change_edit_char(editor_sprite, type,
				 &play.spmap[*pmap].type[editor_sprite]);
}
void dc_editor_seq(int script, int* yield, int* preturnint, int editor_sprite, int seq)
{
  if (editor_sprite < 0 || editor_sprite >= 100)
    return;
  *preturnint = change_edit(editor_sprite, seq,
			    &play.spmap[*pmap].seq[editor_sprite]);
}

void dc_editor_frame(int script, int* yield, int* preturnint, int editor_sprite, int frame)
{
  if (editor_sprite < 0 || editor_sprite >= 100)
    return;
  *preturnint = change_edit_char(editor_sprite, frame,
				 &play.spmap[*pmap].frame[editor_sprite]);
}



void dc_move(int script, int* yield, int* preturnint,
	     int sprite, int direction, int destination_limit, int ignore_hardness_p)
{
  STOP_IF_BAD_SPRITE(sprite);
  spr[sprite].move_active = /*true*/1;
  spr[sprite].move_dir = direction;
  spr[sprite].move_num = destination_limit;
  spr[sprite].move_nohard = ignore_hardness_p;
  spr[sprite].move_script = 0;
  if (debug_mode)
    Msg("Moving: Sprite %d, dir %d, num %d", sprite, direction, destination_limit);
}

void dc_spawn(int script, int* yield, int* preturnint,
	     char* dcscript)
{
  int mysc = load_script(dcscript, 1000, /*true*/1);
  if (mysc == 0)
    {
      *preturnint = 0;
      return;
    }
  locate(mysc, "MAIN");
  int tempreturn = mysc;
  run_script(mysc);
  *preturnint = tempreturn;
}

void dc_run_script_by_number(int script, int* yield, int* preturnint,
			     int script_index, char* funcname)
{
  if (locate(script_index, funcname))
    run_script(script_index);
}

void dc_playmidi(int script, int* yield, int* preturnint,
		 char* midi_file)
{
  //StopMidi();
  int regm = atol(midi_file);
  Msg("Processing playmidi command.");
  if (regm > 1000)
    //cd directive
    {
      int cd_track = regm - 1000;
      Msg("playmidi - cd play command detected.");
      
      if (cd_inserted)
	{
	  if (cd_track == last_cd_track
	      && cdplaying())
	    {
	      *yield = 1;
	      return;
	    }
	  
	  Msg("Playing CD track %d.", cd_track);
	  if (PlayCD(cd_track) >= 0)
	    return;
	}
    }
  Msg("Playing midi %s.", midi_file);
  PlayMidi(midi_file);
}

void dc_playsound(int script, int* yield, int* preturnint,
		  int sound_number, int min_speed, int rand_speed_to_add, int sprite, int repeat_p)
{
  if (sprite < 0 || sprite >= MAX_SPRITES_AT_ONCE)
    sprite = 0; // no "3d" volume effect... and no segfault :p

  if (sound_on)
    *preturnint = playsound(sound_number, min_speed, rand_speed_to_add, sprite, repeat_p);
  else
    *preturnint = 0;
}

void dc_sound_set_survive(int script, int* yield, int* preturnint,
			  int sound_bank, int survive_p)
{
  //let's set one sound to survive
  if (sound_on && sound_bank > 0)
    sound_set_survive(sound_bank, survive_p);
}

void dc_sound_set_vol(int script, int* yield, int* preturnint,
		      int sound_bank, int vol)
{
  if (sound_on && sound_bank > 0)
    sound_set_vol(sound_bank, vol);
}

void dc_sound_set_kill(int script, int* yield, int* preturnint,
		       int sound_bank)
{
  if (sound_on && sound_bank > 0)
    sound_set_kill(sound_bank);
}


void dc_save_game(int script, int* yield, int* preturnint, int game_slot)
{
  save_game(game_slot);
}

void dc_force_vision(int script, int* yield, int* preturnint, int vision)
{
  *pvision = vision;
  rinfo[script]->sprite = 1000;
  fill_whole_hard();
  draw_map_game();
}

void dc_fill_screen(int script, int* yield, int* preturnint, int palette_index)
{
  fill_screen(palette_index);
}

void dc_load_game(int script, int* yield, int* preturnint, int game_slot)
{
  kill_all_scripts_for_real();
  *preturnint = load_game(game_slot);
  Msg("load completed. ");
  if (rinfo[script] == NULL)
    Msg("Script %d is suddenly null!", script);
  *pupdate_status = 1;
  draw_status_all();
  *yield = 1;
}

void dc_game_exist(int script, int* yield, int* preturnint, int game_slot)
{
  FILE *fp;
  if ((fp = paths_savegame_fopen(game_slot, "rb")) != NULL)
    {
      fclose(fp);
      *preturnint = 1;
    }
  else
    {
      *preturnint = 0;
    }
}

void dc_move_stop(int script, int* yield, int* preturnint,
		  int sprite, int direction, int destination_limit, int ignore_hardness_p)
{
  STOP_IF_BAD_SPRITE(sprite);
  spr[sprite].move_active = /*true*/1;
  spr[sprite].move_dir = direction;
  spr[sprite].move_num = destination_limit;
  spr[sprite].move_nohard = ignore_hardness_p;
  spr[sprite].move_script = script;
  if (debug_mode)
    Msg("Move_stop: Sprite %d, dir %d, num %d", sprite, direction, destination_limit);
  *yield = 1;
}

void dc_load_sound(int script, int* yield, int* preturnint,
		   char* wav_file, int sound_index)
{
  if (sound_on)
    {
      Msg("getting %s..", wav_file);
      CreateBufferFromWaveFile(wav_file, sound_index);
    }
}

void dc_debug(int script, int* yield, int* preturnint,
	      char* text)
{
  char buf[350]; /* cf. 'Msg' */
  /* Convert from Latin-1 (.c) to UTF-8 (SDL) since the message is
     shown on the screen in debug mode */
  latin1_to_utf8(text, buf, 350);
  decipher_string(buf, script);
  Msg(buf);
}

void dc_busy(int script, int* yield, int* preturnint,
	     int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);
  *preturnint = does_sprite_have_text(nlist[0]);
  Msg("Busy: Return int is %d and %d.  Nlist got %d.",
      *preturnint, does_sprite_have_text(sprite), sprite);
}


void dc_make_global_int(int script, int* yield, int* preturnint,
			char* varname, int default_val)
{
  make_int(varname, default_val, 0, script);
}

void dc_inside_box(int script, int* yield, int* preturnint,
		   int x, int y, int left, int right, int top, int bottom)
{
  rect myrect;
  rect_set(&myrect, left, right, top, bottom);
  *preturnint = inside_box(x, y, myrect);
  if (debug_mode)
    Msg("Inbox is int is %d and %d.  Nlist got %d.", *preturnint, x, y);
}

void dc_random(int script, int* yield, int* preturnint,
	       int range, int base)
{
  *preturnint = (rand() % range) + base;
}

void dc_initfont(int script, int* yield, int* preturnint,
		 char* fontname)
{
  initfont(fontname);
  Msg("Initted font %s", fontname);
}

void dc_set_mode(int script, int* yield, int* preturnint,
		 int newmode)
{
  mode = newmode;
  *preturnint = mode;
}

void dc_kill_shadow(int script, int* yield, int* preturnint,
		    int sprite)
{
  /* STOP_IF_BAD_SPRITE(sprite); */
  int jj;
  for (jj = 1; jj <= last_sprite_created; jj++)
    {
      if (spr[jj].brain == 15 && spr[jj].brain_parm == sprite)
	{
	  spr[jj].active = 0;
	}
    }
}

void dc_create_sprite(int script, int* yield, int* preturnint,
		      int x, int y, int brain, int sequence, int frame)
{
  *preturnint = add_sprite_dumb(x, y, brain, sequence, frame, 100/*size*/);
}

void dc_sp(int script, int* yield, int* preturnint,
	   int editor_sprite)
{
  int ii;
  for (ii = 1; ii <= last_sprite_created; ii++)
    {
      if (spr[ii].sp_index == editor_sprite)
	{
	  if (debug_mode)
	    Msg("Sp returned %d.", ii);
	  *preturnint = ii;
	  return;
	}
      
    }
  if (last_sprite_created == 1)
    Msg("warning - you can't call SP() from a screen-ref,"
	" no sprites have been created yet.");
}

void dc_is_script_attached(int script, int* yield, int* preturnint,
			   int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);
  *preturnint = spr[sprite].script;
}

void dc_get_sprite_with_this_brain(int script, int* yield, int* preturnint,
				   int brain, int sprite_ignore)
{
  int i;
  for (i = 1; i <= last_sprite_created; i++)
    {
      if (spr[i].brain == brain && i != sprite_ignore && spr[i].active == 1)
	{
	  Msg("Ok, sprite with brain %d is %d", brain, i);
	  *preturnint = i;
	  return;
	}
    }
}

void dc_get_rand_sprite_with_this_brain(int script, int* yield, int* preturnint,
					int brain, int sprite_ignore)
{
  int i;
  int nb_matches = 0;
  for (i = 1; i <= last_sprite_created; i++)
    {
      if (spr[i].brain == brain && i != sprite_ignore && spr[i].active == 1)
	nb_matches++;
    }
  if (nb_matches == 0)
    {
      Msg("Get rand brain can't find any brains with %d.", brain);
	    *preturnint = 0;
	    return;
    }
  
  int mypick = (rand() % nb_matches) + 1;
  int ii;
  int cur_match = 0;
  for (ii = 1; ii <= last_sprite_created; ii++)
    {
      if (spr[ii].brain == brain && ii != sprite_ignore && spr[ii].active == 1)
	{
	  cur_match++;
	  if (cur_match == mypick)
	    {
	      *preturnint = ii;
	      return;
	    }
	}
    }
}

void dc_set_button(int script, int* yield, int* preturnint,
		   int button, int function)
{
  play.button[button] = function;
}

void dc_hurt(int script, int* yield, int* preturnint,
	     int sprite, int damage)
{
  STOP_IF_BAD_SPRITE(sprite);

  if (dversion >= 108)
    {
      // With v1.07 hurt(&sthing, -1) would run hit(), with v1.08 it
      // doesn't (after redink1 tried to fix a game freeze bug that I
      // can't reproduce)
      if (damage < 0)
	return;
    }

  if (hurt_thing(sprite, damage, 0) > 0)
    random_blood(spr[sprite].x, spr[sprite].y-40, sprite);

  if (spr[sprite].nohit != 1
      && spr[sprite].script != 0
      && locate(spr[sprite].script, "HIT"))
    {
      if (rinfo[script]->sprite != 1000)
	{
	  *penemy_sprite = rinfo[script]->sprite;
	  //redink1 addition of missle_source stuff
	  if (dversion >= 108)
	    *pmissle_source = rinfo[script]->sprite;
	}
      kill_returning_stuff(spr[sprite].script);
      run_script(spr[sprite].script);
    }
}

void dc_screenlock(int script, int* yield, int* preturnint,
		   int param)
{
  if (dversion >= 108)
    {
      // returns the screenlock value to DinkC
      if (param == 0 || param == 1)
	screenlock = param;
      *preturnint = screenlock;
      /* Note: redink1's v1.08 always set returnint, even if too many
	 parameters were passed. Since this breaks the logic of DinkC
	 interpreter clarification (return a variable value when bad
	 parameters), we won't reproduce this particular bug
	 here. AFAICS no D-Mod abused 'screenlock' this way. */
    }
  else
    {
      screenlock = param;
    }
}


/****************/
/*  v1.08-only  */
/*              */
/****************/

void dc_sp_blood_num(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].bloodnum);
  *preturnint = spr[sprite].bloodseq;
}

void dc_sp_blood_seq(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].bloodseq);
  *preturnint = spr[sprite].bloodseq;
}

void dc_sp_clip_bottom(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].alt.bottom);
  *preturnint = spr[sprite].alt.bottom;
}

void dc_sp_clip_left(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].alt.left);
  *preturnint = spr[sprite].alt.left;
}

void dc_sp_clip_right(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].alt.right);
  *preturnint = spr[sprite].alt.right;
}

void dc_sp_clip_top(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].alt.top);
  *preturnint = spr[sprite].alt.top;
}

void dc_sp_custom(int script, int* yield, int* preturnint, char* key, int sprite, int val)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  if (spr[sprite].active == 0)
    {
      *preturnint = -1;
    }
  else
    {
      // Set the value
      if (val != -1)
	dinkc_sp_custom_set(spr[sprite].custom, key, val);
      *preturnint = dinkc_sp_custom_get(spr[sprite].custom, key);
    }
}


/**
 * Like sp_mx but use change_sprite_noreturn, so allow setting the
 * value to -1.
 */
void dc_sp_move_x(int script, int* yield, int* preturnint, int sprite, int dx)
  {
    STOP_IF_BAD_SPRITE(sprite);
    change_sprite_noreturn (sprite, dx, &spr[sprite].mx);
  }

/**
 * Like sp_my but use change_sprite_noreturn, so allow setting the
 * value to -1.
 */
void dc_sp_move_y(int script, int* yield, int* preturnint, int sprite, int dy)
  {
    STOP_IF_BAD_SPRITE(sprite);
    change_sprite_noreturn (sprite, dy, &spr[sprite].my);
  }

void dc_sp_freeze(int script, int* yield, int* preturnint, int sprite, int frozen_p)
{
  STOP_IF_BAD_SPRITE(sprite);
  // Set the value
  if (frozen_p == 0)
    spr[sprite].freeze = 0;
  else if (frozen_p == 1)
    spr[sprite].freeze = script;
  /* else -> invalid value */

  // Return the (normalized) value
  *preturnint = (spr[sprite].freeze > 0);
}


void dc_clear_editor_info(int script, int* yield, int* preturnint)
{
    int i;
    for (i = 0; i < 769; i++)
      {
	int j;
	for (j = 0; j < 100; j++)
	  {
	    play.spmap[i].seq[j] = 0;
	    play.spmap[i].frame[j] = 0;
	    play.spmap[i].type[j] = 0;
	    play.spmap[i].last_time = 0;
	  }
      }
    *preturnint = 1;
}

void dc_get_date_day(int script, int* yield, int* preturnint)
{
    char mytime[5];
    time_t ct;
    struct tm* time_now;
    time (&ct);
    time_now = localtime (&ct);
    strftime (mytime, 5, "%d", time_now);
    *preturnint = atoi (mytime);
}

void dc_get_date_month(int script, int* yield, int* preturnint)
{
    char mytime[5];
    time_t ct;
    struct tm* time_now;
    time (&ct);
    time_now = localtime (&ct);
    strftime (mytime, 5, "%m", time_now);
    *preturnint = atoi (mytime);
}

void dc_get_date_year(int script, int* yield, int* preturnint)
{
    char mytime[5];
    time_t ct;
    struct tm* time_now;
    time (&ct);
    time_now = localtime (&ct);
    strftime (mytime, 5, "%Y", time_now);
    *preturnint = atoi (mytime);
}

void dc_get_time_game(int script, int* yield, int* preturnint)
{
    time_t ct;
    time (&ct);
    *preturnint = play.minutes + (difftime (ct, time_start) / 60);
}

void dc_get_time_real(int script, int* yield, int* preturnint)
{
    char mytime[5];
    time_t ct;
    struct tm* time_now;
    time (&ct);
    time_now = localtime (&ct);
    strftime (mytime, 5, "%M", time_now);
    *preturnint = atoi (mytime);
    strftime (mytime, 5, "%H", time_now);
    *preturnint += 60 * atoi (mytime);
}

void dc_get_truecolor(int script, int* yield, int* preturnint)
{
    *preturnint = truecolor;
}

void dc_show_console(int script, int* yield, int* preturnint)
{
    console_active = 1;
}

void dc_show_inventory(int script, int* yield, int* preturnint)
{
    item_screen = 1;
}

void dc_var_used(int script, int* yield, int* preturnint)
{
    int m = 0;
    int i;
    for (i = 1; i < MAX_VARS; i++)
      if (play.var[i].active == 1)
	m++;
    *preturnint = m;
}


void dc_loopmidi(int script, int* yield, int* preturnint, int loop_midi)
{
  loopmidi(loop_midi);
}


void dc_math_abs(int script, int* yield, int* preturnint, int val)
{
  *preturnint = abs(val);
}

void dc_math_sqrt(int script, int* yield, int* preturnint, int val)
{
  *preturnint = sqrt(abs(val));
}

void dc_math_mod(int script, int* yield, int* preturnint, int val, int div)
{
  *preturnint = (val % div);
}

void dc_make_global_function(int script, int* yield, int* preturnint, char* dcscript, char* procname)
{
  make_function(dcscript, procname);
}

void dc_set_save_game_info(int script, int* yield, int* preturnint, char* info)
{
  strcpy (save_game_info, info);
}

void dc_load_map(int script, int* yield, int* preturnint, char* mapdat_file, char* dinkdat_file)
{
  // load a new map/dink.dat
  strcpy(current_map, mapdat_file);
  strcpy(current_dat, dinkdat_file);
  load_info();
}

void dc_load_tile(int script, int* yield, int* preturnint, char* tile_file, int tile_index)
{
  // load new tiles
  if (tile_index >= 1 && tile_index <= NB_TILE_SCREENS)
    {
      //Load in the new tiles...
      tiles_load_slot(tile_file, tile_index);
      
      //Store in save game
      strncpy(play.tile[tile_index].file, tile_file, 50);
    }
}

void dc_map_tile(int script, int* yield, int* preturnint, int tile_position, int tile_index)
{
  // developers can change or see what tile is at any given position
  // Yeah... they can only modify valid tiles
  if (tile_position > 0 && tile_position <= 96)
    {
      //Only change the value if it is greater than 0...
      if (tile_index > 0)
	pam.t[tile_position - 1].num = tile_index;
      *preturnint = pam.t[tile_position - 1].num;
    }
}

void dc_map_hard_tile(int script, int* yield, int* preturnint, int tile_position, int hard_tile_index)
{
  // developers can retrieve/modify a hard tile
  // Yeah... they can only modify valid tiles
  if (tile_position > 0 && tile_position <= 96)
    {
      //Only change the value if it is greater than 0...
      if (hard_tile_index > 0)
	{
	  pam.t[tile_position - 1].althard = hard_tile_index;
	}
      *preturnint = pam.t[tile_position - 1].althard;
    }
}


void dc_load_palette(int script, int* yield, int* preturnint, char* bmp_file)
{
  // load a pallete from any bmp
  char *name = bmp_file;
  SDL_Surface* image = NULL;
  FILE *in = paths_dmodfile_fopen(name, "rb");
  if (in == NULL)
    fprintf(stderr, "Error: Can't open palette '%s'.", name);
  else
    {
      /* Set palette */
      image = load_bmp_setpal(in);
      memcpy(GFX_real_pal, cur_screen_palette, 256);
    }
  
  if (image == NULL)
    fprintf(stderr, "Couldn't load palette from '%s'.\n", name);
  else
    {
      //Store in save game
      strncpy(play.palette, slist[0], 50);
      SDL_FreeSurface(image);
    }
}

void dc_get_item(int script, int* yield, int* preturnint, char* dcscript)
{
  // get index of specified item
  *preturnint = 0;
  for (int i = 1; i < 17; i++)
    {
      if (play.item[i].active
	  && compare(play.item[i].name, dcscript))
	{
	  *preturnint = i;
	  break;
	}
    }
}

void dc_get_magic(int script, int* yield, int* preturnint, char* dcscript)
{
  // get index of specified magic spell
  *preturnint = 0;
  for (int i = 1; i < 9; i++)
    {
      if (play.mitem[i].active
	  && compare(play.mitem[i].name, dcscript))
	{
	  *preturnint = i;
	  break;
	}
    }
}

void dc_set_font_color(int script, int* yield, int* preturnint, int index, int r, int g, int b)
{
  // sets font color
  set_font_color(index, r, g, b);
}

void dc_get_next_sprite_with_this_brain(int script, int* yield, int* preturnint,
					int brain, int sprite_ignore, int sprite_start_with)
{
  // make Paul Pliska's life more fulfilling
  for (int i = sprite_start_with; i <= last_sprite_created; i++)
    {
      if ((spr[i].brain == brain) && (i != sprite_ignore))
	if (spr[i].active == 1)
	  {
	    Msg ("Ok, sprite with brain %d is %d", brain, i);
	    *preturnint = i;
	    return;
	  }
    }
  Msg ("Ok, sprite with brain %d is 0", brain);
}

void dc_set_smooth_follow(int script, int* yield, int* preturnint, int smooth_p)
{
  if (smooth_p == 0)
    smooth_follow = 0;
  else if (smooth_p == 1)
    smooth_follow = 1;
}
void dc_set_dink_base_push(int script, int* yield, int* preturnint, int base_sequence)
{
  dink_base_push = base_sequence;
}

void dc_callback_kill(int script, int* yield, int* preturnint, int callback_index)
{
  Msg ("setting callback random");
  kill_callback(callback_index);
}


/****************/
/*  Hash table  */
/*              */
/****************/

/* Map DinkC function with C function */
#define NB_COMMON_ARGS 3
struct binding 
{
  char* funcname; /* name of the function, as string */
  void* func;     /* pointer to the C function */
  int params[10]; /* DinkC specification of params e.g. {2,1,1,0,0,0,0,0,0,0} */
  enum dinkc_parser_state badparams_dcps; /* if the DinkC script has bad arguments, skip line or yield? */
  int badparams_returnint_p; /* overwrite returnint if bad arguments? */
  int badparams_returnint;   /* value for returnint if badparams_returnint_p is 1 */
  ffi_cif cif;                           /* libffi function struct */
  ffi_type* cif_args[NB_COMMON_ARGS+10]; /* libffi argument types, referenced by 'cif' */
};

/* Hash table of bindings, build dynamically (depending on 'dversion',
   not statically) */
Hash_table* bindings = NULL;

/* Auxiliary functions for hash */
static size_t dinkc_bindings_hasher(const void *x, size_t tablesize)
{
  return hash_string(((struct binding*)x)->funcname, tablesize);
  // We could also call 'hash_pjw' from module 'hash-pjw'
}

static bool dinkc_bindings_comparator(const void* a, const void* b)
{
  return !strcmp(((struct binding*)a)->funcname,
		 ((struct binding*)b)->funcname);
}

/**
 * Search a binding by function name
 */
struct binding* dinkc_bindings_lookup(dinkc_sp_custom hash, char* funcname)
{
  struct binding search;
  char* lcfuncname = strdup(funcname);
  char* pc;
  for (pc = lcfuncname; *pc != '\0'; pc++)
    *pc = tolower(*pc);
  search.funcname = lcfuncname;
  return hash_lookup(hash, &search);
}

/**
 * Initialize the libffi structures of binding pointer 'pbd' and add
 * it to hash table 'hash'
 */
static void dinkc_bindings_add(Hash_table* hash, struct binding* pbd)
{
  void* slot = dinkc_bindings_lookup(hash, pbd->funcname);
  if (slot != NULL)
    {
      fprintf(stderr, "Internal error: attempting to redeclare binding %s\n", pbd->funcname);
      exit(1);
    }

  /* Copy uninitialized binding in hash table */
  struct binding* newslot = malloc(sizeof(struct binding));
  *newslot = *pbd;
  hash_insert(hash, newslot);
  /* Only use 'newslot' now, otherwise 'args' may refer to the wrong
     place */
  pbd = NULL;

  /* Initialize the argument info vectors */
  ffi_type** args = newslot->cif_args;
  /* Common arguments */
  args[0] = &ffi_type_sint;
  args[1] = &ffi_type_pointer;
  args[2] = &ffi_type_pointer;
  
  /* prepare call interface */
  int nb_dc_args = 0;
  int* p = newslot->params;
  if (p[0] != -1)
    {
      int i = 0;
      while (p[i] != 0 && i < 10)
	{
	  switch(p[i])
	    {
	    case 1: /* int */
	      args[NB_COMMON_ARGS+i] = &ffi_type_sint;
	      break;
	    case 2:
	      args[NB_COMMON_ARGS+i] = &ffi_type_pointer;
	    }
	  i++;
	}
      nb_dc_args = i;
    }
      
  /* Initialize the cif */
  ffi_type* rt = &ffi_type_void;
  if (ffi_prep_cif(&newslot->cif, FFI_DEFAULT_ABI, NB_COMMON_ARGS+nb_dc_args,
		   rt, args) != FFI_OK)
    {
      printf("Internal error: couldn't initialize binding %s\n", newslot->funcname);
      exit(EXIT_FAILURE);
    }
}


/**
 * Add a DinkC binding
 * 
 * Simple macro to allow using struct initializer e.g. {2,1,1,0....}
 * when declaring a DinkC function.
 */
#define DCBD_ADD(name, ...)                                 \
{                                                           \
  struct binding bd = { #name, dc_ ## name, __VA_ARGS__ };  \
  dinkc_bindings_add(bindings, &bd);                        \
}
/**
 * Map DinkC functions to C functions, with their arguments
 */
void dinkc_bindings_init()
{
  Hash_tuning* default_tuner = NULL;
  int start_size = 400; /* ~nbfuncs*2 to try and avoid collisions */
  bindings = hash_initialize(start_size, default_tuner,
			     dinkc_bindings_hasher, dinkc_bindings_comparator,
			     free);

  DCBD_ADD(sp_active,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_attack_hit_sound,       {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_attack_hit_sound_speed, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_attack_wait,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_attack,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_die,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_hit,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_idle,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_walk,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_brain,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_brain_parm,             {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_brain_parm2,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_defense,                {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_dir,                    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_disabled,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_distance,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_exp,                    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_flying,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_follow,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_frame,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_frame_delay,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_gold,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_hard,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_hitpoints,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_move_nohard,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_mx,                     {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_my,                     {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_noclip,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_nocontrol,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_nodraw,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_nohit,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_notouch,                {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_pframe,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_picfreeze,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_pseq,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_que,                    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_range,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_reverse,                {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_seq,                    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_size,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_sound,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_speed,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_strength,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_target,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_timing,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_touch_damage,           {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_x,                      {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_y,                      {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

  DCBD_ADD(sp_kill,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_editor_num,             {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_kill_wait,              {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_script,                 {1,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  /* sp_base_death is an alias for sp_base_die */
  struct binding bd_sp_base_death = *dinkc_bindings_lookup(bindings, "sp_base_die");
  bd_sp_base_death.funcname = "sp_base_death";
  dinkc_bindings_add(bindings, &bd_sp_base_death);

  DCBD_ADD(unfreeze,              {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(freeze,                {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(set_callback_random,   {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(set_dink_speed,        {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(reset_timer,          {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(set_keep_mouse,        {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(add_item,              {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(add_magic,             {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(add_exp,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_this_item,        {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_this_magic,       {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(show_bmp,              {2,1,1,0,0,0,0,0,0,0}, DCPS_YIELD        , 0, 0);
  DCBD_ADD(copy_bmp_to_screen,    {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(wait_for_button,      {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(stop_wait_for_button, {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(draw_screen,          {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(free_items,           {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(free_magic,           {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(kill_cur_item,        {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(kill_cur_magic,       {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(draw_status,          {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(arm_weapon,           {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(arm_magic,            {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(load_screen,          {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(say,                   {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(say_stop,              {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(say_stop_npc,          {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(say_stop_xy,           {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(say_xy,                {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(restart_game,         {-1,0,0,0,0,0,0,0,0,0}, -1                , 0, 0);
  DCBD_ADD(wait,                  {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(preload_seq,           {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(script_attach,         {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(draw_hard_sprite,      {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

  DCBD_ADD(activate_bow,        {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(disable_all_sprites, {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(draw_background,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(draw_hard_map,       {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(enable_all_sprites,  {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(fade_down,           {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(fade_up,             {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(get_burn,            {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(get_last_bow_power,  {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(get_version,         {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_all_sounds,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_game,           {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_this_task,      {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(scripts_used,        {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(stopcd,              {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(stopmidi,            {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(turn_midi_off,       {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(turn_midi_on,        {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

  DCBD_ADD(count_item,               {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(count_magic,              {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(compare_sprite_script,    {1,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(compare_weapon,           {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(compare_magic,            {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(init,                     {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(dink_can_walk_off_screen, {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(push_active,              {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(stop_entire_game,         {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

  DCBD_ADD(editor_type,  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(editor_seq,   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(editor_frame, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

  DCBD_ADD(move,                 {1,1,1,1,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(spawn,                {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(run_script_by_number, {1,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(playmidi,             {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(playsound,            {1,1,1,1,1,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(sound_set_survive,    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(sound_set_vol,        {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(sound_set_kill,       {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

  DCBD_ADD(save_game,                 {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(force_vision,              {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(fill_screen,               {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(load_game,                 {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(game_exist,                {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(move_stop,                 {1,1,1,1,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(load_sound,                {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(debug,                     {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(busy,                      {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

  DCBD_ADD(make_global_int,            {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(inside_box,                 {1,1,1,1,1,1,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(random,                     {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(initfont,                   {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(set_mode,                   {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_shadow,                {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(create_sprite,              {1,1,1,1,1,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(sp,                         {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(is_script_attached,         {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(get_sprite_with_this_brain,      {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(get_rand_sprite_with_this_brain, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(set_button,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(hurt,                       {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(screenlock,                 {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

  if (dversion >= 108)
    {
      DCBD_ADD(sp_blood_num,   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_blood_seq,   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_clip_bottom, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_clip_left,   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_clip_right,  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_clip_top,    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

      DCBD_ADD(sp_custom,      {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

      DCBD_ADD(sp_move_x, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(sp_move_y, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(sp_freeze, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);


      DCBD_ADD(clear_editor_info, {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_date_day,      {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_date_month,    {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_date_year,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_time_game,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_time_real,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_truecolor,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(show_console,      {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(show_inventory,    {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(var_used,          {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

      DCBD_ADD(loopmidi,           {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

      DCBD_ADD(math_abs,                 {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(math_sqrt,                {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(math_mod,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(make_global_function,     {2,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(set_save_game_info,       {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(load_map,                 {2,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(load_tile,                {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(map_tile,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(map_hard_tile,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(load_palette,             {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_item,                 {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(get_magic,                {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(set_font_color,           {1,1,1,1,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(set_smooth_follow,        {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(set_dink_base_push,       {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(callback_kill,            {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_next_sprite_with_this_brain,  {1,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
    }
}

void dinkc_bindings_quit()
{
  hash_free(bindings);
}


/******************/
/*  DinkC parser  */
/*                */
/******************/

void attach(void)
{
  /* Make sure the "system" variable exists - otherwise we might use a
     NULL pointer below */
  char* var_names[22] = { "&life", "&vision", "&result", "&speed",
		     "&timing", "&lifemax", "&exp", "&strength",
		     "&defense", "&gold", "&magic", "&level",
		     "&player_map", "&cur_weapon", "&cur_magic",
		     "&last_text", "&magic_level", "&update_status",
		     "&missile_target", "&enemy_sprite", "&magic_cost",
		     "&missle_source" };
  int n, i;
  for (n = 0; n < 22; n++)
    {
      if (!var_exists(var_names[n], 0)) /* 0 = global scope */
	make_int(var_names[n], 0, 0, -1);
      /* TODO: setting script to -1 is asking for troubles... */
    }

  for (i = 1; i < MAX_VARS; i++)
    {
      if (compare("&life", play.var[i].name)) plife = &play.var[i].var;
      if (compare("&vision", play.var[i].name)) pvision = &play.var[i].var;
      if (compare("&result", play.var[i].name)) presult = &play.var[i].var;
      if (compare("&speed", play.var[i].name)) pspeed = &play.var[i].var;
      if (compare("&timing", play.var[i].name))	ptiming = &play.var[i].var;
      if (compare("&lifemax", play.var[i].name)) plifemax = &play.var[i].var;
      if (compare("&exp", play.var[i].name)) pexper = &play.var[i].var;
      if (compare("&strength", play.var[i].name))  pstrength = &play.var[i].var;
      if (compare("&defense", play.var[i].name))  pdefense = &play.var[i].var;
      if (compare("&gold", play.var[i].name))  pgold = &play.var[i].var;
      if (compare("&magic", play.var[i].name))  pmagic = &play.var[i].var;
      if (compare("&level", play.var[i].name))  plevel = &play.var[i].var;
      if (compare("&player_map", play.var[i].name)) pmap = &play.var[i].var;
      if (compare("&cur_weapon", play.var[i].name)) pcur_weapon = &play.var[i].var;
      if (compare("&cur_magic", play.var[i].name)) pcur_magic = &play.var[i].var;
      if (compare("&last_text", play.var[i].name)) plast_text = &play.var[i].var;
      if (compare("&magic_level", play.var[i].name)) pmagic_level = &play.var[i].var;
      if (compare("&update_status", play.var[i].name)) pupdate_status = &play.var[i].var;
      if (compare("&missile_target", play.var[i].name)) pmissile_target = &play.var[i].var;
      if (compare("&enemy_sprite", play.var[i].name)) penemy_sprite = &play.var[i].var;
      if (compare("&magic_cost", play.var[i].name)) pmagic_cost = &play.var[i].var;
      if (compare("&missle_source", play.var[i].name)) pmissle_source = &play.var[i].var;
    }
}


/**
 * Process DinkC dialog choice stanza
 */
/*bool*/int talk_get(int script)
{
  char line[200], check[200], checker[200];
  int cur = 1;
  char *p;
  int retnum = 0;
  clear_talk();
  talk.newy = -5000;
  while(1)
    {
    redo:
      read_next_line(script, line);
      
      strip_beginning_spaces(line);
      //Msg("Comparing to %s.", line);
      
      get_word(line, 1, checker);
      
      if (compare(checker, "set_y"))
        {
	  get_word(line, 2, checker);
	  talk.newy = atol(checker);
	  goto redo;
        }
      
      if (compare(checker, "set_title_color"))
        {
	  get_word(line, 2, checker);
	  talk.color = atol(checker);
	  goto redo;
        }
      
      strip_beginning_spaces(line);
      if (compare(line, "\n")) goto redo;

morestuff:
      separate_string(line, 1, '(', check);
      strip_beginning_spaces(check);
      
      if (compare(check, "title_start"))
        {
	  while(read_next_line(script, line))
	    {
	      strcpy(check, line);
	      strip_beginning_spaces(line);
	      get_word(line, 1, checker);
	      separate_string(line, 1, '(', check);
	      strip_beginning_spaces(check);
	      
	      if (compare(check, "title_end"))
		{
		  replace("\n\n\n\n","\n \n", talk.buffer);
		  replace("\n\n","\n", talk.buffer);
		  goto redo;
		}
	      
	      /* drop '\n', this messes translations */
	      line[strlen(line)-1] = '\0';
	      /* Translate text (before variable substitution) */
	      i18n_translate(line, 200-1);
	      /* put '\n' back, just in case */
	      strcat(line, "\n");

	      decipher_string(line, script);
	      strcat(talk.buffer, line);
	      //talk.buffer[strlen(talk.buffer)-1] = 0;
	    }
	  
	  goto redo;
        }
      
      if (compare(check, "choice_end"))
        {
	  if (cur-1 == 0)
	    {
	      Msg("Error: choice() has 0 options in script %s, offset %d.",
		  rinfo[script]->name, rinfo[script]->current);
	      return(/*false*/0);
	    }
	  //all done, lets jam
	  //Msg("found choice_end, leaving!");
	  talk.last = cur-1;
	  talk.cur = 1;
	  talk.active = /*true*/1;
	  talk.page = 1;
	  talk.cur_view = 1;
	  talk.script = script;
	  return(/*true*/1);
	}
      
      separate_string(line, 1, '\"', check);
      strip_beginning_spaces(check);
      
      //Msg("Check is %s.",check);
      
      if (strlen(check) > 2)
        {
	  //found conditional statement
	  if (strchr(check, '(') == NULL)
	    {
	      Msg("Error with choice() statement in script %s, offset %d. (%s?)",
		  rinfo[script]->name, rinfo[script]->current, check);
	      return(/*false*/0);
	    }
	  
	  separate_string(check, 2, '(', checker);
	  separate_string(checker, 1, ')', check);
	  
	  //Msg("Running %s through var figure..", check);
	  if (var_figure(check, script) == 0)
	    {
	      Msg("Answer is no.");
	      retnum++;
	      goto redo;
	      //said NO to statement
	    }
	  //Msg("Answer is yes.");
	  separate_string(line, 1, ')', check);
	  
	  p = line + strlen(check) + 1;
	  
	  strcpy(check, p);
	  strcpy(line, check);
	  
	  /* Resume processing stripping the first condition (there
	     may be several conditions on a single dialog ligne, which
	     are AND'ed) */
	  goto morestuff;
        }
      
      separate_string(line, 2, '\"', check);
      strip_beginning_spaces(check);

      /* Translate text (before variable substitution) */
      i18n_translate(check, 101);

      // Msg("Line %d is %s.",cur,check);
      retnum++;
      decipher_savegame = retnum;
      decipher_string(check, script);
      decipher_savegame = 0;
      strcpy(talk.line[cur], check);
      talk.line_return[cur] = retnum;
      cur++;
    }
}


/**
 * Utility function for 'process_line', to separate and store the current procedure arguments.
 *
 * proc_name: named of the called function
 * script: script id
 * str_params: string to parse (what was after the function name)
 * spec: describe the function's parameters:
 *    1=int
 *    2=string
 *    0=no more args (10 args max)
 *
 * Known bug: passing no argument to a function expecting 1 int
 * argument is considered valid..
 *
 * Return: 0 if parse error, 1 if success
 */
int get_parms(char proc_name[20], int script, char *str_params, int* spec)
{
  char crap[1024];

  /* Clean-up parameters */
  memset(nlist, 0, 10 * sizeof (int));
  memset(slist, '\0', 10 * 200);

  strip_beginning_spaces(str_params);
  if (str_params[0] == '(')
    {
      //Msg("Found first (.");
      str_params++;
    }
  else
    {
      Msg("Missing ( in %s, offset %d.", rinfo[script]->name, rinfo[script]->current);
      return 0;
    }

  int i;
  for (i = 0; i < 10; i++)
    {
      strip_beginning_spaces(str_params);
      
      if (spec[i] == 1) // type=int
	{
	  // Get next parameter (until ',' or ')' is reached)
	  if (strchr(str_params, ',') != NULL)
	    separate_string(str_params, 1, ',', crap);
	  else if (strchr(str_params, ')') != NULL)
	    separate_string(str_params, 1, ')', crap);

	  // move to next param
	  str_params += strlen(crap);
	  
	  if (crap[0] == '&')
	    {
	      replace(" ", "", crap);
	      //      Msg("Found %s, 1st is %c",crap, crap[0]);
	      decipher(crap, script);
	    }
	  // store parameter of type 'int'
	  nlist[i] = atol(crap);
	}
      else if (spec[i] == 2) // type=string
	{
	  // Msg("Checking for string..");
	  separate_string(str_params, 2, '"', crap);
	  // move to next param
	  str_params += strlen(crap)+2;

	  // store parameter of type 'string'
	  strcpy(slist[i], crap);
	}

      if ((i+1) == 10 || spec[i+1] == 0) // this was the last arg
	{
	  //finish
	  strip_beginning_spaces(str_params);
	  
	  if (str_params[0] == ')')
	    {
	      str_params++;
	    }
	  else
	    {
	      return 0;
	    }
	  strip_beginning_spaces(str_params);
	  return 1;
	}

      //got a parm, but there is more to get, lets make sure there is a comma there
      strip_beginning_spaces(str_params);

      if (str_params[0] == ',')
	{
	  str_params++;
	}
      else
	{
	  return 0;
	}
    }
  return 1;
}


/**
 * Process one line of DinkC and returns directive to the DinkC
 * interpreter.
 * 
 * Cf. doc/HACKING_dinkc.txt for understanding in progress ;)
 **/
enum dinkc_parser_state process_line(int script, char *s, /*bool*/int doelse)
{
  char *h, *p;
  char line[200];
  char ev[15][100];
  char temp[100];
  int kk;
  
  if (rinfo[script]->level < 1)
    rinfo[script]->level = 1;


  for (kk = 1; kk < 15; kk++)
    ev[kk][0] = 0;
  h = s;
  if (h[0] == '\0')
    return 0;
  
  if ((h[0] == '/') && (h[1] == '/'))
    {
      //Msg("It was a comment!");
      goto bad;
    }

  {
    int i;
    for (i = 1; i <= 14; i++)
      if (separate_string(h, i, ' ', ev[i]) == /*false*/0)
	break;
  }


  if (compare(ev[1], "VOID"))
    {
      if (rinfo[script]->proc_return != 0)
	{
	  run_script(rinfo[script]->proc_return);
	  kill_script(script);
	}
      return(DCPS_YIELD);
    }

  /* goto label? */
  if (ev[1][strlen(ev[1]) -1] == ':' && strlen(ev[2]) < 2)
    {
      if (dversion >= 108)
	{
	  /* Attempt to avoid considering:
			   say("bonus: 5 points", 1); // would not display any text at all!
			   as a label */
	  if (strncmp (ev[1], "say", 3) != 0)
	    return(0); //its a label
	}
      else
	{
	  return(0); //its a label
	}
    }

  /** Expression between parenthesis **/
  if (ev[1][0] == '(')
    {
      //this procedure has been passed a conditional statement finder
      //what kind of conditional statement is it?
      p = h;
      separate_string(h, 2, ')', temp);
      separate_string(h, 1, ')', ev[1]);

      // Msg("Ok, turned h %s to  ev1 %s.",h,ev[1]);
      p += strlen(ev[1]) + 1;

      strip_beginning_spaces(p);

      if (strchr(temp, '=') != NULL)
	{
	  h++;
	  strip_beginning_spaces(h);
	  process_line(script, h, /*false*/0);
	  replace("==", "", temp);
	  sprintf(line, "%d == %s", returnint, temp);
	  returnint = var_figure(line, script);
	  strcpy(h, "\n");
	  return(0);
	}
      
      if (strchr(temp, '>') != NULL)
	{
	  h++;
	  strip_beginning_spaces(h);
	  process_line(script, h, /*false*/0);
	  replace("==", "", temp);
	  sprintf(line, "%d > %s", returnint, temp);
	  returnint = var_figure(line, script);
	  strcpy(h, "\n");
	  return(0);
	}

      if (strchr(temp, '<') != NULL)
	{
	  h++;
	  strip_beginning_spaces(h);
	  process_line(script, h, /*false*/0);
	  replace("==", "", temp);
	  sprintf(line, "%d < %s", returnint, temp);
	  returnint = var_figure(line, script);
	  strcpy(h, "\n");
	  return(0);
	}
      
      /* Beuc: This should be converted to a set of "if ... else
       * if... else if ..." and multi-character constants should be
       * removed. However, this may cause the interpreter to behave
       * differently, so be careful. */
      /* For now, I'll rewrite the code in an equivalent warning-free
       * inelegant way: strchr(str, 'ab') <=> strchr(str, 'b') */
      /* if (strchr (temp, '<=') != NULL) */
      if (strchr(temp, '=') != NULL)
	{
	  h++;
	  strip_beginning_spaces(h);
	  process_line(script, h, /*false*/0);
	  replace("==", "", temp);
	  sprintf(line, "%d <= %s", returnint, temp);
	  returnint = var_figure(line, script);
	  strcpy(h, "\n");
	  return(0);
	}
      /* if (strchr (temp, '>=') != NULL) */
      if (strchr (temp, '=') != NULL)
	{
	  h++;
	  strip_beginning_spaces(h);
	  process_line(script, h, /*false*/0);
	  replace("==", "", temp);
	  sprintf(line, "%d >= %s", returnint, temp);
	  returnint = var_figure(line, script);
	  strcpy(h, "\n");
	  return(0);
	}
      /* if (strchr (temp, '!=') != NULL) */
      if (strchr (temp, '=') != NULL)
	{
	  h++;
	  strip_beginning_spaces(h);
	  process_line(script, h, /*false*/0);
	  replace("==", "", temp);
	  sprintf(line, "%d != %s", returnint, temp);
	  returnint = var_figure(line, script);
	  strcpy(h, "\n");
	  return(0);
	}
      

      if (p[0] == ')')
	{
	  //its a procedure in the if statement!!!
	  h++;
	  p++;
	  strcpy(line, p);
	  process_line(script, h, /*false*/0);
	  
	  Msg("Returned %d for the returnint", returnint);
	  h = s;
	  strcpy(s, line);
	  
	  return(0);
	}
      else
	{
	  h++;
	  
	  separate_string(h, 1,')',line);
	  h += strlen(line) + 1;
	  returnint = var_figure(line, script);
	  
	  strcpy_nooverlap(s, h);
	  
	  return(0);
	}
      
      strip_beginning_spaces(h);
      strip_beginning_spaces(ev[1]);

      s = h;
    } /* END expression between parenthesis */


  if (strchr(ev[1], '(') != NULL)
    {
      //Msg("Has a (, lets change it");
      separate_string(h, 1,'(',ev[1]);
      //Msg("Ok, first is now %s",ev[1]);
    }

  /** { Bloc } **/
  char first = ev[1][0];
  if (first == '{')
    {
      rinfo[script]->level++;
      //Msg("Went up level, now at %d.", rinfo[script]->level);
      h++;
      if (rinfo[script]->skipnext)
	{
	  /* Skip the whole { section } */
	  rinfo[script]->skipnext = /*false*/0;
	  rinfo[script]->onlevel = ( rinfo[script]->level - 1);
	}
      goto good;
    }
  
  if (first == '}')
    {
      rinfo[script]->level--;
      //Msg("Went down a level, now at %d.", rinfo[script]->level);
      h++;
      
      if (rinfo[script]->onlevel > 0 && rinfo[script]->level == rinfo[script]->onlevel)
	{
	  /* Finished skipping the { section }, preparing to run 'else' */
	  strip_beginning_spaces(h);
	  strcpy_nooverlap(s, h);
	  return(DCPS_DOELSE_ONCE);
	}
      goto good;
    }

  /* Fix if there are too many closing '}' */
  if (rinfo[script]->level < 0)
    {
      rinfo[script]->level = 0;
    }


  /* Note: that's the 2nd time we compare with "VOID" -
     cf. above. However ev[1] was modified in between, so this
     section may still be called if the first comparison didn't
     match. */
  if (compare(ev[1], "void"))
    {
      //     Msg("Next procedure starting, lets quit");
      strcpy_nooverlap(s, h);
      if (rinfo[script]->proc_return != 0)
	{
	  run_script(rinfo[script]->proc_return);
	  kill_script(script);
	}
      
      return(DCPS_YIELD);
    }

  
  /* Stop processing if we're skipping the current { section } */
  if (rinfo[script]->onlevel > 0 && rinfo[script]->level > rinfo[script]->onlevel)
    return(0);
    
  rinfo[script]->onlevel = 0;
    
  /* Skip the current line if the previous 'if' or 'else' said so */
  if (rinfo[script]->skipnext)
    {
      //sorry, can't do it, you were told to skip the next thing
      rinfo[script]->skipnext = /*false*/0;
      strcpy(s, "\n"); /* jump to next line */
      //return(3);
      return(DCPS_DOELSE_ONCE);
    }
    


  if (compare(ev[1], "void"))
    {
      Msg("ERROR: Missing } in %s, offset %d.", rinfo[script]->name,rinfo[script]->current);
      strcpy_nooverlap(s, h);
      return(DCPS_YIELD);
    }
    
  /** if **/
  if (compare(ev[1], "if"))
    {
      h += strlen(ev[1]);
      strip_beginning_spaces(h);
	
      process_line(script, h, /*false*/0);
      // Result is 'returnint'
	
      if (returnint != 0)
	{
	  if (debug_mode)
	    Msg("If returned true");
	}
      else
	{
	  //don't do it!
	  rinfo[script]->skipnext = /*true*/1;
	  if (debug_mode)
	    Msg("If returned false, skipping next thing");
	}
	
      strcpy_nooverlap(s, h);
      //g("continuing to run line %s..", h);

      //return(5);
      return(DCPS_DOELSE_ONCE);
      /* state 5 should actually be state DCPS_CONTINUE, but keeping
	 it that way (e.g. with doelse=1 for the next line) for
	 compatibility, just in case somebody abused it */
    }

  if (compare(ev[1], "else"))
    {
      //Msg("Found else!");
      h += strlen(ev[1]);
		
      if (doelse)
	{
	  // Yes to else
	}
      else
	{
	  // No to else...
	  // they shouldn't run the next thing
	  rinfo[script]->skipnext = /*true*/1;
	}
      strcpy_nooverlap(s, h);
      return(1);
    }
    
  /** Dialog **/
  if (compare(ev[1], "choice_start"))
    {
      kill_text_owned_by(1);
      if (talk_get(script))
	{
	  // Question(s) gathered successfully
	  return(DCPS_YIELD);
	}
      return(0);
    }

  /** Jump **/
  if (compare(ev[1], "goto"))
    {
      locate_goto(ev[2], script);
      return(0);
    }

  /** Definition **/
  if (compare(ev[1], "int"))
    {
      int_prepare(h, script);
      h += strlen(ev[1]);

      if (strchr(h, '=') != NULL)
	{
	  strip_beginning_spaces(h);
	  //Msg("Found =...continuing equation");
	  strcpy_nooverlap(s, h);
	  return(DCPS_CONTINUE);
	}
      else
	{
	  return(DCPS_GOTO_NEXTLINE);
	}
    }

  /** "return;" and "return something;" **/
  if (compare(ev[1], "return;"))
    {
      if (debug_mode)
	Msg("Found return; statement");
	
      if (rinfo[script]->proc_return != 0)
	{
	  bKeepReturnInt = 1; /* v1.08 */
	  run_script(rinfo[script]->proc_return);
	  kill_script(script);
	}
	
      return(DCPS_YIELD);
    }

  if (dversion >= 108)
    {
      /* "return", without trailing ';' */
      /* added so we can have return values and crap. */
      /* see also "return;" above */
      if (compare (ev[1], "return"))
	{
	  if (debug_mode)
	    Msg ("Found return; statement");
	  h += strlen(ev[1]);
	  strip_beginning_spaces (h);
	  process_line (script, h, 0);
	  if (rinfo[script]->proc_return != 0)
	    {
	      bKeepReturnInt = 1;
	      run_script (rinfo[script]->proc_return);
	      kill_script (script);
	    }
	  return(DCPS_YIELD);
	}
    }

  /********************/
  /*  DinkC bindings  */
  /*                  */
  /********************/
    
  /** Lookup bindings **/
  char* funcname = ev[1];
  char* str_args = h + strlen(ev[1]);
  struct binding* pbd = NULL;
  pbd = dinkc_bindings_lookup(bindings, funcname);
    
  if (pbd != NULL)
    {
      /** Call binding **/
      void *values[NB_COMMON_ARGS+10];
	
      /* Common arguments */
      values[0] = &script;
      int* yield = alloca(sizeof(int)*1);
      yield[0] = 0; /* don't yield by default) */
      values[1] = &yield;
      int* preturnint = &returnint;
      values[2] = &preturnint;
	
      /* Specific arguments */
      int* params = pbd->params;
      if (params[0] != -1) /* no args == no checks*/
	{
	  if (get_parms(funcname, script, str_args, params))
	    {
	      int i = 0;
	      while (params[i] != 0 && i < 10)
		{
		  switch(params[i])
		    {
		    case 1: /* int */
		      {
			values[NB_COMMON_ARGS+i] = &nlist[i];
			break;
		      }
		    case 2: /* string */
		      {
			char** pointer = alloca(sizeof(char*)*1);
			*pointer = slist[i];
			values[NB_COMMON_ARGS+i] = pointer;
			break;
		      }
		    }
		  i++;
		}
	    }
	  else
	    {
	      /* Invalid parameters in the DinkC script - output an
		 error message */
	      int i = 0;
	      while (params[i] != 0 && i < 10)
		i++;
	      Msg("%s: DinkC error: procedure '%s' takes %d parameters (offset %d)",
		  rinfo[script]->name, funcname, i, rinfo[script]->current);

	      /* Set 'returnint' if necessary */
	      if (pbd->badparams_returnint_p == 1)
		returnint = pbd->badparams_returnint;
	      /* Fallback parser state */
	      return pbd->badparams_dcps;
	    }
	}
	
      /* Call C function */
      int rc;
      cur_funcname = pbd->funcname; /* for error messages */
      ffi_call(&pbd->cif, pbd->func, &rc, values);
      cur_funcname = "";
      /* the function can manipulation returnint through argument #3 */
	
      if (*yield == 0)
	return DCPS_GOTO_NEXTLINE;
      else if (*yield == 1)
	return DCPS_YIELD;
      else
	{
	  fprintf(stderr, "Internal error: DinkC function %s requested invalid state %d",
		  pbd->funcname, *yield);
	  exit(EXIT_FAILURE);
	}
    }
    
  
  /***************/
  /** Operators **/
  /**           **/
  /***************/

  /* Beware: this works on ev[2], not ev[1]; position in the code is
     critical! */

  if (compare(ev[2], "="))
    {
      h += strlen(ev[1]);
      strip_beginning_spaces(h);
      h++;
      strip_beginning_spaces(h);
      var_equals(ev[1], ev[3], '=', script, h);
      strcpy_nooverlap(s, h);
      return(0);
    }
    
  if (compare(ev[2], "+="))
    {
      h += strlen(ev[1]);
      strip_beginning_spaces(h);
      h += 2;
      strip_beginning_spaces(h);
      var_equals(ev[1], ev[3], '+', script, h);
      strcpy_nooverlap(s, h);
      return(0);
    }
    
  if (compare(ev[2], "*="))
    {
      h += strlen(ev[1]);
      strip_beginning_spaces(h);
      h += 2;
      strip_beginning_spaces(h);
      var_equals(ev[1], ev[3], '*', script, h);
      strcpy_nooverlap(s, h);
      return(0);
    }
    
  if (compare(ev[2], "-="))
    {
      h += strlen(ev[1]);
      strip_beginning_spaces(h);
      h += 2;
      strip_beginning_spaces(h);
	
      var_equals(ev[1], ev[3], '-', script, h);
	
      strcpy_nooverlap(s, h);
      return(0);
    }
    
  if (compare(ev[2], "/")
      || (dversion >= 108 && compare(ev[2], "/=")))
    {
      h += strlen(ev[1]);
      strip_beginning_spaces(h);
      h++;
      strip_beginning_spaces(h);
	
      var_equals(ev[1], ev[3], '/', script, h);
	
      strcpy_nooverlap(s, h);
      return(0);
    }
    
  if (compare(ev[2], "*"))
    {
      h += strlen(ev[1]);
      strip_beginning_spaces(h);
      h++;
      strip_beginning_spaces(h);
	
      var_equals(ev[1], ev[3], '*', script, h);
	
      strcpy_nooverlap(s, h);
      return(0);
    }
    
    
  /***************************************/
  /** New DinkC user-defined procedures **/
  /**                                   **/
  /***************************************/
  if (dversion >= 108)
    {
      if (compare (ev[1], "external"))
	{
	  h += strlen(ev[1]);
	  int p[20] = { 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 };
	  memset (slist, 0, 10 * 200);
	  get_parms(ev[1], script, h, p);
	  if (strlen(slist[0]) > 0 && strlen(slist[1]) > 0)
	    {
	      int myscript1 = load_script(slist[0], rinfo[script]->sprite, 0);
	      if (myscript1 == 0)
		{
		  Msg ("Error:  Couldn't find %s.c (for procedure %s)",
		       slist[0], slist[1]);
		  return (0);
		}
	      rinfo[myscript1]->arg1 = nlist[2];
	      rinfo[myscript1]->arg2 = nlist[3];
	      rinfo[myscript1]->arg3 = nlist[4];
	      rinfo[myscript1]->arg4 = nlist[5];
	      rinfo[myscript1]->arg5 = nlist[6];
	      rinfo[myscript1]->arg6 = nlist[7];
	      rinfo[myscript1]->arg7 = nlist[8];
	      rinfo[myscript1]->arg8 = nlist[9];
	      if (locate (myscript1, slist[1]))
		{
		  rinfo[myscript1]->proc_return = script;
		  run_script (myscript1);
		  return(DCPS_YIELD);
		}
	      else
		{
		  Msg ("Error:  Couldn't find procedure %s in %s.",
		       slist[1], slist[0]);
		  kill_script (myscript1);
		}
	    }
	  strcpy (s, h);
	  return (0);
	}

      if (strchr (h, '(') != NULL)
	{
	  //lets attempt to run a procedure
	  int myscript = load_script (rinfo[script]->name, rinfo[script]->sprite, 0);
	  h += strlen(ev[1]);
	  int p[20] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	  get_parms(ev[1], script, h, p);
	  if (locate(myscript, ev[1]))
	    {
	      /* Custom procedure in the current script */
	      rinfo[myscript]->arg1 = nlist[0];
	      rinfo[myscript]->arg2 = nlist[1];
	      rinfo[myscript]->arg3 = nlist[2];
	      rinfo[myscript]->arg4 = nlist[3];
	      rinfo[myscript]->arg5 = nlist[4];
	      rinfo[myscript]->arg6 = nlist[5];
	      rinfo[myscript]->arg7 = nlist[6];
	      rinfo[myscript]->arg8 = nlist[7];
	      rinfo[myscript]->arg9 = nlist[8];
	      rinfo[myscript]->proc_return = script;
	      run_script(myscript);
	      return(DCPS_YIELD);
	    }
	  else
	    {
	      /* Try custom global procedure */
	      for (int i = 0; i < 100; i++)
		{
		  /* Skip empty slots */
		  if (strlen (play.func[i].func) == 0)
		    continue;
		    
		  if (compare(play.func[i].func, ev[1]))
		    {
		      myscript = load_script(play.func[i].file, rinfo[script]->sprite, 0);
		      rinfo[myscript]->arg1 = nlist[0];
		      rinfo[myscript]->arg2 = nlist[1];
		      rinfo[myscript]->arg3 = nlist[2];
		      rinfo[myscript]->arg4 = nlist[3];
		      rinfo[myscript]->arg5 = nlist[4];
		      rinfo[myscript]->arg6 = nlist[5];
		      rinfo[myscript]->arg7 = nlist[6];
		      rinfo[myscript]->arg8 = nlist[7];
		      rinfo[myscript]->arg9 = nlist[8];
		      if (locate(myscript, ev[1]))
			{
			  rinfo[myscript]->proc_return = script;
			  run_script (myscript);
			  return(DCPS_YIELD);
			}
		      break;
		    }
		}
	      Msg("ERROR:  Procedure void %s( void ); not found in script %s. (word 2 was %s)",
		  ev[1], ev[2], rinfo[myscript]->name);
	      kill_script (myscript);
	    }
	    
	  /*seperate_string(h, 1,'(',line);
	      
	    int myscript = load_script(rinfo[script]->name, rinfo[script]->sprite, false);
	      
	    if (locate( myscript, line))
	    {
	    rinfo[myscript]->proc_return = script;
	    run_script(myscript);    
	    return(DCPS_YIELD);
	    } else
	    {
	    Msg("ERROR:  Procedure void %s( void ); not found in script %s. (word 2 was %s) ", line,
	    ev[2], rinfo[myscript]->name); 
	    kill_script(myscript);          
	    } */
	  return (0);
	}
    }
  else
    {
      /* v1.07 function that are implemented differently than in v1.08 */
      if (compare(ev[1], "external"))
	{
	  h += strlen(ev[1]);
	  int p[20] = {2,2,0,0,0,0,0,0,0,0};
	  if (get_parms(ev[1], script, h, p))
	    {
	      int myscript1 = load_script(slist[0],rinfo[script]->sprite, /*false*/0);
	      if (myscript1 == 0)
		{
		  Msg("Error:  Couldn't find %s.c (for procedure %s)", slist[0], slist[1]);
		  return(0);
		}
	      if (locate( myscript1, slist[1]))
		{
		  rinfo[myscript1]->proc_return = script;
		  run_script(myscript1);
		  return(DCPS_YIELD);
		}
	      else
		{
		  Msg("Error:  Couldn't find procedure %s in %s.", slist[1], slist[0]);
		  kill_script(myscript1);
		}
	    }
	  else
	    {
	      Msg("%s: DinkC error: procedure 'external' takes 2 parameters (offset %d)",
		  rinfo[script]->name, rinfo[script]->current);
	    }
	  strcpy_nooverlap(s, h);
	  return(0);
	}

      if (strchr(h, '(') != NULL)
	{
	  //lets attempt to run a procedure
	  separate_string(h, 1, '(', line);
	  int myscript = load_script(rinfo[script]->name, rinfo[script]->sprite, /*false*/0);

	  if (locate(myscript, line))
	    {
	      rinfo[myscript]->proc_return = script;
	      run_script(myscript);
	      return(DCPS_YIELD);
	    }
	  else
	    {
	      Msg("ERROR:  Procedure void %s( void ); not found in script %s. (word 2 was %s) ", line,
		  ev[2], rinfo[myscript]->name);
	      kill_script(myscript);
	    }
	  return(0);
	}
	
      Msg("MERROR: \"%s\" unknown in %s, offset %d.",ev[1], rinfo[script]->name,rinfo[script]->current);
      //in a thingie, ready to go
    }

bad:
  strcpy(s, "\n"); /* jump to next line */
  //return(0);
  return(DCPS_CONTINUE);
  
 good:
  strcpy_nooverlap(s, h);
  //s = h
  //Msg("ok, continuing with running %s..",s);
  return(DCPS_CONTINUE);
}
