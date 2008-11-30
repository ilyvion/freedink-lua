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
#define STOP_IF_BAD_SPRITE(sprite)                   \
  if (sprite <= 0 || sprite >= MAX_SPRITES_AT_ONCE)  \
    {                                                \
      Msg("DinkC error: %s: invalid sprite %d",      \
          cur_funcname, sprite);                     \
      return;                                        \
    }

/**
 * sp_* functions used to call 'change_sprite' on spr[sprite] without
 * checking if 'sprite' was in [1; MAX_SPRITES_AT_ONCE-1]. Since
 * 'change_sprite' returns -1 when 'sprite' is inactive, that's also
 * what we return when the sprite is out of range.
 */
#define RETURN_NEG_IF_BAD_SPRITE(sprite)             \
  if (sprite <= 0 || sprite >= MAX_SPRITES_AT_ONCE)  \
    {                                                \
      Msg("DinkC error: %s: invalid sprite %d",      \
          cur_funcname, sprite);                     \
      *preturnint = -1;                              \
      return;                                        \
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
  decipher_string(text, script);
  *preturnint = say_text(text, active_sprite, script);
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
  decipher_string(text, script);
  int sprite = say_text(text, active_sprite, script);
  *preturnint = sprite;
  spr[sprite].callback = script;
    
  *yield = 1;
}

void dc_say_stop_xy(int script, int* yield, int* preturnint, char* text, int x, int y)
{
  Msg("Say_stop_xy: Adding %s", text);
  kill_returning_stuff(script);
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

void dc_free_magic(int script, int* yeild, int* preturnint)
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


        if (compare(line, "\n")) goto redo;
        if (compare(line, "\\\\")) goto redo;


        strip_beginning_spaces(line);
        //Msg("Comparing to %s.", line);
        if (compare(line, "\n")) goto redo;
        if (compare(line, "\\\\")) goto redo;

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

                        line[strlen(line)] = 0;
                        //Msg("LINE IS: %s: Like it?",line);

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

                p = &line[strlen(check)+1];

                strcpy(check, p);


                strcpy(line, check);

                //Msg("new line is %s, happy?", line);
                goto morestuff;
        }



        separate_string(line, 2, '\"', check);
        strip_beginning_spaces(check);
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
 * Return: 0 if parse error, 1 if success
 */
int get_parms(char proc_name[20], int script, char *str_params, int spec[10])
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
	      Msg("Missing ')' in %s, offset %d.", rinfo[script]->name, rinfo[script]->current);
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
	  Msg("Procedure %s does not take %d parms in %s, offset %d. (%s?)",
	      proc_name, i+1, rinfo[script]->name, rinfo[script]->current, str_params);
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
  int i;
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


  for (i = 1; i <= 14; i++)
    {
      if (separate_string(h, i, ' ', ev[i]) == /*false*/0)
	goto pass;
    }
  
pass:
  //Msg("first line is %s (second is %s)", ev[1], ev[2]);
                if (compare(ev[1], "VOID"))
                {

                        if (rinfo[script]->proc_return != 0)
                        {
                                run_script(rinfo[script]->proc_return);
                                kill_script(script);
                        }

                        //Msg("returning..");
                        return(DCPS_YIELD);
                }
                //replace("\n","",ev[1]);
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
                if (ev[1][0] == '(')
                {
                        //this procedure has been passed a conditional statement finder
                        //what kind of conditional statement is it?
                        p = h;
                        separate_string(h, 2,')',temp);
                        //Msg("We found %s, woah!", temp);
                        separate_string(h, 1,')',ev[1]);

                        // Msg("Ok, turned h %s to  ev1 %s.",h,ev[1]);
                        p = &p[strlen(ev[1])+1];

                        strip_beginning_spaces(p);
                        //      Msg("does %s have a ( in front?", p);
                        //Msg("We found %s, woah!", temp);


                        if (strchr(temp, '=') != NULL)
                        {
                                h = &h[1];
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
                                h = &h[1];
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
                                h = &h[1];
                                strip_beginning_spaces(h);
                                process_line(script, h, /*false*/0);
                                replace("==", "", temp);
                                sprintf(line, "%d < %s", returnint, temp);
                                returnint = var_figure(line, script);
                                strcpy(h, "\n");
                                return(0);
                        }

                        /* Beuc: This should be converted to a set of
                         * "if ... else if... else if ..." and
                         * multi-character constants should be
                         * removed. However, this may cause the
                         * interpreter to behave differently, so be
                         * careful. */
                        /* For now, I'll rewrite the code in an
                         * equivalent warning-free inelegant way:
                         * strchr(str, 'ab') <=> strchr(str, 'b') */
                        /* if (strchr (temp, '<=') != NULL) */
                        if (strchr(temp, '=') != NULL)
                        {
                                h = &h[1];
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
                                h = &h[1];
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
                                h = &h[1];
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
                                h = &h[1];
                                p = &p[1];
                                strcpy(line, p);
                                process_line(script, h, /*false*/0);

                                //8
                                Msg("Returned %d for the returnint", returnint);
                                h = s;
                                strcpy(s, line);

                                //      Msg("Returing %s..", s);
                                return(0);
                        } else
                        {
                                h = &h[1];

                                separate_string(h, 1,')',line);
                                h = &h[strlen(line)+1];
                                returnint = var_figure(line, script);

                                strcpy_nooverlap(s, h);

                                return(0);
                        }

                        strip_beginning_spaces(h);
                        strip_beginning_spaces(ev[1]);

                        s = h;


        }


        if (strchr(ev[1], '(') != NULL)
        {
                //Msg("Has a (, lets change it");
                separate_string(h, 1,'(',ev[1]);
                //Msg("Ok, first is now %s",ev[1]);


        }

	char first = ev[1][0];

        if (first == '{')
        {


                rinfo[script]->level++;
                //Msg("Went up level, now at %d.", rinfo[script]->level);
                h = &h[1];
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
                h = &h[1];

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

	/* TODO: that's the 2nd time we compare with "VOID" - cf. above */
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

        { //used to be an if..


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

                //if (debug_mode) Msg("%s",s);


                if (compare(ev[1], "void"))
                {
                        Msg("ERROR: Missing } in %s, offset %d.", rinfo[script]->name,rinfo[script]->current);
                        strcpy_nooverlap(s, h);
                        return(DCPS_YIELD);
                }

                if (compare(ev[1], "else"))
                {
                        //Msg("Found else!");
                        h = &h[strlen(ev[1])];


                        if (doelse)
                        {
                                //Msg("Yes to else...");



                        } else
                        {
                                //they shouldn't run the next thing
                                rinfo[script]->skipnext = /*true*/1;
                                //Msg("No to else...");

                        }
                        strcpy_nooverlap(s, h);
                        return(1);

                }


  /******************/
  /*  New bindings  */
  /*                */
  /******************/

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
	      /* Invalid parameters in the DinkC script */
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


                if (compare(ev[1], "choice_start"))
                {

                        kill_text_owned_by(1);
                        if (talk_get(script))
                        {

                                //              Msg("Question gathered successfully.");
                                return(DCPS_YIELD);
                        }

                        return(0);
                }

                if (compare(ev[1], "goto"))
                {

                        locate_goto(ev[2], script);
                        return(0);
                }

                if (compare(ev[1], "int"))
                {

                        int_prepare(h, script);

                        //Msg(slist[0]);

                        h = &h[strlen(ev[1])];

                        //Msg("Int is studying %s..", h);
                        if (strchr(h, '=') != NULL)
                        {
                                strip_beginning_spaces(h);
                                //Msg("Found =...continuing equation");
                                strcpy_nooverlap(s, h);
                                return(DCPS_CONTINUE);
                        }

                        return(DCPS_GOTO_NEXTLINE);

                }

                                                                if (compare(ev[1], "return;"))
                                                                {

                                                                        if (debug_mode) Msg("Found return; statement");

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
    /* added so we can have return values and crap. */
    /* see also "return;" above */
    if (compare (ev[1], "return"))

      {
	if (debug_mode)
	  Msg ("Found return; statement");
	h = &h[strlen (ev[1])];
	strip_beginning_spaces (h);
	process_line (script, h, 0);
	if (rinfo[script]->proc_return != 0)

	  {
	    bKeepReturnInt = 1;
	    run_script (rinfo[script]->proc_return);
	    kill_script (script);
	  }
	return (2);
      }
  }



                                                                if (compare(ev[1], "if"))
                                                                {

                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        //Msg("running if with string of %s", h);

                                                                        process_line(script, h, /*false*/0);
                                                                        //Msg("Result is %d", returnint);

                                                                        if (returnint != 0)
                                                                        {
                                                                                if (debug_mode) Msg("If returned true");


                                                                        } else
                                                                        {
                                                                                //don't do it!
                                                                                rinfo[script]->skipnext = /*true*/1;
                                                                                if (debug_mode) Msg("If returned false, skipping next thing");
                                                                        }

                                                                        //DO STUFF HERE!
                                                                        strcpy_nooverlap(s, h);
                                                                        //g("continuing to run line %s..", h);


                                                                        //return(5);
									return(DCPS_DOELSE_ONCE);
									/* state 5 should actually be state DCPS_CONTINUE, but keeping it
									   that way (e.g. with doelse=1 for the next line) for compatibility, just in case somebody
									   abused it */

                                                                }




  /******************/
  /*  Old bindings  */
  /*                */
  /******************/

                if (compare(ev[1], "move"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,1,1,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                spr[nlist[0]].move_active = /*true*/1;
                                spr[nlist[0]].move_dir = nlist[1];
                                spr[nlist[0]].move_num = nlist[2];
                                spr[nlist[0]].move_nohard = nlist[3];
                                spr[nlist[0]].move_script = 0;
                                if (debug_mode) Msg("Moving: Sprite %d, dir %d, num %d", nlist[0],nlist[1], nlist[2]);


                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "spawn"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                int mysc = load_script(slist[0], 1000, /*true*/1);
                                if (mysc == 0)
                                {
                                        returnint = 0;
                                        return(0);
                                }
                                locate(mysc, "MAIN");
                                int tempreturn = mysc;
                                run_script(mysc);
                                returnint = tempreturn;
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "run_script_by_number"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,2,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  if (locate (nlist[0], slist[1]))
			    run_script (nlist[0]);
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "playmidi"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                          {
                            //StopMidi();
                            int regm = atol(slist[0]);
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
                                      return(DCPS_YIELD);

                                    Msg("Playing CD track %d.", cd_track);
                                    if (PlayCD(cd_track) >= 0)
                                      {
                                        strcpy_nooverlap(s, h);
                                        return(0);
                                      }
                                  }
                              }
                            Msg("Playing midi %s.", slist[0]);
                            PlayMidi(slist[0]);
                          }
                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "Playsound"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,1,1,1,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                if (sound_on)
                                        returnint = playsound(nlist[0], nlist[1], nlist[2], nlist[3],nlist[4]);
                                else returnint = 0;

                        } else
                                returnint = 0;

                        strcpy_nooverlap(s, h);
                        return(0);
                }


                if (compare(ev[1], "sound_set_survive"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                if (sound_on)
                                {
                                        //let's set one sound to survive
                                        if (nlist[0] > 0)
					  sound_set_survive(nlist[0], nlist[1]);
                                }
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }


                if (compare(ev[1], "sound_set_vol"))
                {
                  // (sprite, direction, until, nohard);
                  h = &h[strlen(ev[1])];
                  int p[20] = {1,1,0,0,0,0,0,0,0,0};
                  if (get_parms(ev[1], script, h, p))
                    {
                      if (sound_on)
                        {
                          //let's set one sound to survive
                          if (nlist[0] > 0)
                            {
                              sound_set_vol(nlist[0], nlist[1]);
                            }
                        }
                    }

                  strcpy_nooverlap(s, h);
                  return(0);
                }


                if (compare(ev[1], "sound_set_kill"))
                {
                  // (sprite, direction, until, nohard);
                  h = &h[strlen(ev[1])];
                  int p[20] = {1,0,0,0,0,0,0,0,0,0};
                  if (get_parms(ev[1], script, h, p))
                    {
                      if (sound_on)
                        {
                          //let's set one sound to survive
                          if (nlist[0] > 0)
                            sound_set_kill(nlist[0] - 1);
                        }
                    }

                  strcpy_nooverlap(s, h);
                  return(0);
                }



                if (compare(ev[1], "save_game"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                save_game(nlist[0]);
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }


                if (compare(ev[1], "force_vision"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                *pvision = nlist[0];
                                rinfo[script]->sprite = 1000;
                                fill_whole_hard();

                                draw_map_game();

                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "fill_screen"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                fill_screen(nlist[0]);

                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }


                if (compare(ev[1], "load_game"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                kill_all_scripts_for_real();
                                returnint = load_game(nlist[0]);
                                Msg("load completed. ");
                                if (rinfo[script] == NULL) Msg("Script %d is suddenly null!", script);


                                *pupdate_status = 1;
                                draw_status_all();
                                return(DCPS_YIELD);
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "game_exist"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  FILE *fp;
			  if ((fp = paths_savegame_fopen(nlist[0], "rb")) != NULL)
			    {
			      fclose(fp);
			      returnint = 1;
			    }
			  else
			    {
			      returnint = 0;
			    }
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }


                if (compare(ev[1], "move_stop"))
                {
                        // (sprite, direction, until, nohard);

                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,1,1,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                //Msg("Move stop running %d to %d..", nlist[0], nlist[0]);
                                spr[nlist[0]].move_active = /*true*/1;
                                spr[nlist[0]].move_dir = nlist[1];
                                spr[nlist[0]].move_num = nlist[2];
                                spr[nlist[0]].move_nohard = nlist[3];
                                spr[nlist[0]].move_script = script;
                                strcpy_nooverlap(s, h);
                                if (debug_mode) Msg("Move_stop: Sprite %d, dir %d, num %d", nlist[0],nlist[1], nlist[2]);
                                return(DCPS_YIELD);

                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }




                if (compare(ev[1], "load_sound"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                if (sound_on)
                                {
                                        Msg("getting %s..",slist[0]);
                                        CreateBufferFromWaveFile(slist[0],nlist[1]);
                                }
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }




                if (compare(ev[1], "debug"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                decipher_string(slist[0], script);
                                Msg(slist[0]);
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }



                if (compare(ev[1], "make_global_int"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                make_int(slist[0], nlist[1], 0, script);
                                //Msg(slist[0]);
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }



                if (compare(ev[1], "busy"))
                {

                        h = &h[strlen(ev[1])];
                        // Msg("Running busy, h is %s", h);
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                if (nlist[0] == 0) Msg("ERROR:  Busy cannot get info on sprite 0 in %s.",rinfo[script]->name);
                                else
                                {

                                        returnint = does_sprite_have_text(nlist[0]);

                                        Msg("Busy: Return int is %d and %d.  Nlist got %d.", returnint,does_sprite_have_text(nlist[0]), nlist[0]);

                                }

                        }  else Msg("Failed getting parms for Busy()");

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "inside_box"))
                {

                        h = &h[strlen(ev[1])];
                        Msg("Running pigs with h", h);
                        int p[20] = {1,1,1,1,1,1,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                rect myrect;
                rect_set(&myrect, nlist[2], nlist[3], nlist[4], nlist[5]);
                                returnint = inside_box(nlist[0], nlist[1], myrect);

                                if (debug_mode)
                                        Msg("Inbox is int is %d and %d.  Nlist got %d.", returnint, nlist[0], nlist[1]);



                        }  else Msg("Failed getting parms for inside_box");

                        strcpy_nooverlap(s, h);
                        return(0);
                }


                if (compare(ev[1], "random"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = (rand() % nlist[0])+nlist[1];
                        }  else Msg("Failed getting parms for Random()");

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "initfont"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                initfont(slist[0]);
                                Msg("Initted font %s",slist[0]);
                        }  else Msg("Failed getting parms for Initfont()");

                        strcpy_nooverlap(s, h);
                        return(0);
                }




                if (compare(ev[1], "set_mode"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                mode = nlist[0];
                                returnint = mode;
                        }  else Msg("Failed to set mode");

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "kill_shadow"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  int jj;
                                for (jj = 1; jj <= last_sprite_created; jj++)
                                {
                                        if (spr[jj].brain == 15) if (spr[jj].brain_parm == nlist[0])
                                        {

                                                spr[jj].active = 0;
                                        }


                                }
                        }

                        strcpy_nooverlap(s, h);
                        return(0);
                }



                if (compare(ev[1], "create_sprite"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,1,1,1,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                returnint = add_sprite_dumb(nlist[0],nlist[1],nlist[2],
                                        nlist[3],nlist[4],
                                        100);

                                return(0);
                        }
                        returnint =  0;
                        return(0);
                }



                if (compare(ev[1], "sp"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  int ii;
                                for (ii = 1; ii <= last_sprite_created; ii++)
                                {

                                        if (spr[ii].sp_index == nlist[0])
                                        {

                                                if (debug_mode) Msg("Sp returned %d.", ii);
                                                returnint = ii;
                                                return(0);
                                        }

                                }
                                if (last_sprite_created == 1)
                                {
                                        Msg("warning - you can't call SP() from a screen-ref, no sprites have been created yet.");
                                }

                        }
                        returnint =  0;
                        return(0);
                }



                if (compare(ev[1], "is_script_attached"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {


                                returnint =  spr[nlist[0]].script;

                        }
                        return(0);
                }


                if (compare(ev[1], "get_sprite_with_this_brain"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  int i;
                                for (i = 1; i <= last_sprite_created; i++)
                                {
                                        if (   (spr[i].brain == nlist[0]) && (i != nlist[1]) ) if
                                                (spr[i].active == 1)
                                        {
                                                Msg("Ok, sprite with brain %d is %d", nlist[0], i);
                                                returnint = i;
                                                return(0);
                                        }

                                }
                        }
                                 Msg("Ok, sprite with brain %d is 0", nlist[0], i);

                                         returnint =  0;
                                         return(0);
                }


                if (compare(ev[1], "get_rand_sprite_with_this_brain"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  int i;
                                int cter = 0;
                                for (i = 1; i <= last_sprite_created; i++)
                                {
                                        if (   (spr[i].brain == nlist[0]) && (i != nlist[1]) ) if
                                                (spr[i].active == 1)
                                        {
                                                cter++;

                                        }

                                }

                                if (cter == 0)
                                {
                                        Msg("Get rand brain can't find any brains with %d.",nlist[0]);
                                        returnint =  0;
                                        return(0);
                                }

				{
				  int mypick = (rand() % cter)+1;
				  int ii;
				  cter = 0;
				  for (ii = 1; ii <= last_sprite_created; ii++)
				    {
				      if (spr[ii].brain == nlist[0] && ii != nlist[1] && spr[ii].active == 1)
					{
					  cter++;
					  if (cter == mypick)
					    {
					      returnint = ii;
					      return(0);
					    }
                                        }
				    }
                                }


                        }


                        returnint =  0;
                        return(0);
                }


                if (compare(ev[1], "editor_type"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                //Msg("Setting editor_type..");
                                returnint = change_edit_char(nlist[0], nlist[1], &play.spmap[*pmap].type[nlist[0]]);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }
                if (compare(ev[1], "editor_seq"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_edit(nlist[0], nlist[1], &play.spmap[*pmap].seq[nlist[0]]);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "editor_frame"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_edit_char(nlist[0], nlist[1], &play.spmap[*pmap].frame[nlist[0]]);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "set_button"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                play.button[nlist[0]] = nlist[1];

                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "hurt"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  if (dversion >= 108)
			    {
			        // With v1.07 hurt(&sthing, -1) would
			        // run hit(), with v1.08 it doesn't
			        // (after redink1 tried to fix a game
			        // freeze bug that I can't reproduce)
                                if (nlist[1] < 0)
                                  return (0);
			    }

                                if (hurt_thing(nlist[0], nlist[1], 0) > 0)
                                        random_blood(spr[nlist[0]].x, spr[nlist[0]].y-40, nlist[0]);
                                if (spr[nlist[0]].nohit != 1)
                                        if (spr[nlist[0]].script != 0)

                                                if (locate(spr[nlist[0]].script, "HIT"))
                                                {

                                                        if (rinfo[script]->sprite != 1000)
							  {
                                                                *penemy_sprite = rinfo[script]->sprite;
								//redink1 addition of missle_source stuff
								if (dversion >= 108)
								  *pmissle_source = rinfo[script]->sprite;

							  }

                                                        kill_returning_stuff(spr[nlist[0]].script);
                                                        run_script(spr[nlist[0]].script);
                                                }

                                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "screenlock"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
			if (dversion >= 108)
			  {
			    // returns the screenlock value to DinkC
			    if (get_parms (ev[1], script, h, p))
			      {
				if (nlist[0] == 0 || nlist[0] == 1)
				  screenlock = nlist[0];
			      }
			    returnint = screenlock;
			    return (0);
			  }
			else
			  {
			    if (get_parms(ev[1], script, h, p))
			      {
                                screenlock = nlist[0];
                                return(0);
			      }
			    returnint = -1;
			  }
			return(0);
                }

                if (compare(ev[1], "stop_entire_game"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                stop_entire_game = nlist[0];




/*                                 while( 1 ) */
/*                                 { */

/*                                         RECT rcRect; */

/*                                         SetRect(&rcRect, 0,0,640,480); */
/*                                         ddrval = lpDDSTwo->BltFast( 0, 0, lpDDSBack, */
/*                                                 &rcRect, DDBLTFAST_NOCOLORKEY); */
					// GFX
					SDL_BlitSurface(GFX_lpDDSBack, NULL, GFX_lpDDSTwo, NULL);

/*                                         if( ddrval == DD_OK ) */
/*                                         { */
/*                                                 break; */
/*                                         } */
/*                                         if( ddrval != DDERR_WASSTILLDRAWING ) */
/*                                         { */
/*                                                 dderror(ddrval); */
/*                                                 return(0); */
/*                                         } */
/*                                 } */










                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "dink_can_walk_off_screen"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                walk_off_screen = nlist[0];
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "push_active"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                push_active = nlist[0];
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }



                if (compare(ev[1], "count_item"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  int i;
                                returnint = 0;
                                for (i = 1; i < 17; i++)
                                {
                                        if (play.item[i].active)
                                        {
                                                if (compare(play.item[i].name, slist[0])) returnint++;
                                        }

                                }

                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "count_magic"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  int i;
                                returnint = 0;
                                for (i = 1; i < 9; i++)
                                {
                                        if (play.mitem[i].active)
                                        {
                                                if (compare(play.mitem[i].name, slist[0])) returnint++;
                                        }

                                }

                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                                                                if (compare(ev[1], "init"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {

                                                                                figure_out(slist[0]);
                                                                                return(0);
                                                                        }
                                                                        returnint =  -1;
                                                                        return(0);
                                                                }

                                                                if (compare(ev[1], "compare_weapon"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = 0;
                                                                                if (*pcur_weapon == 0)
                                                                                {
                                                                                        return(0);
                                                                                }

                                                                                if (compare(play.item[*pcur_weapon].name, slist[0]))
                                                                                {
                                                                                        returnint = 1;

                                                                                }
                                                                                return(0);
                                                                        }


                                                                        return(0);
                                                                }


if (compare(ev[1], "compare_magic"))
{
  h = &h[strlen(ev[1])];
  int p[20] = {2,0,0,0,0,0,0,0,0,0};
  if (get_parms(ev[1], script, h, p))
    {
      returnint = 0;
      if (*pcur_magic == 0)
	{
	  return(0);
	}
 
      if (dversion >= 108)
	{
	  if (compare(play.mitem[*pcur_magic].name, slist[0]))
	    returnint = 1;
	}
      else
	{
	  if (compare(play.item[*pcur_magic].name, slist[0]))
	    returnint = 1;
	}
      return(0);
    }
  return(0);
}


                                                                if (compare(ev[1], "compare_sprite_script"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,2,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = 0;

                                                                                if (nlist[0] <= 0 || nlist[0] >= MAX_SPRITES_AT_ONCE)
                                                                                {
										  Msg("Error: Can't compare sprite script for sprite %d!?", nlist[0]);
                                                                                        return(0);
                                                                                }
                                                                                if (spr[nlist[0]].active)
                                                                                {

                                                                                        if (spr[nlist[0]].script == 0)
                                                                                        {
                                                                                                Msg("Compare sprite script says: Sprite %d has no script.",nlist[0]);
                                                                                                return(0);
                                                                                        }
                                                                                        if (rinfo[spr[nlist[0]].script] == NULL)
                                                                                        {
											        Msg("Compare sprite script says: script %d for sprite %d was already killed!.", nlist[0], spr[nlist[0]].script);
                                                                                                return(0);
                                                                                        }
                                                                                        if (compare(slist[1], rinfo[spr[nlist[0]].script]->name))
                                                                                        {
                                                                                                returnint = 1;
                                                                                                return(0);
                                                                                        }

                                                                                } else
                                                                                {
                                                                                        Msg("Can't compare sprite script, sprite not active.");
                                                                                }



                                                                                return(0);
                                                                        }


                                                                        return(0);
                                                                }



  /*********************************/
  /** New DinkC commands in v1.08 **/
  /**                             **/
  /*********************************/

  if (dversion >= 108)
    {
    //redink1 added this function
    if (compare (ev[1], "math_abs"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    returnint = abs (nlist[0]);
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "math_sqrt"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    returnint = sqrt ((double) abs (nlist[0]));
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "math_mod"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    returnint = (nlist[0] % nlist[1]);
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added for global functions
    if (compare (ev[1], "make_global_function"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    make_function (slist[0], slist[1]);

	    //Msg(slist[0]);
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function to change the save game 'info'
    if (compare (ev[1], "set_save_game_info"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    strcpy (save_game_info, slist[0]);
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function to load a new map/dink.dat
    if (compare (ev[1], "load_map"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = { 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms(ev[1], script, h, p))
	  {
	    strcpy(current_map, slist[0]);
	    strcpy(current_dat, slist[1]);
	    load_info();
	  }
	strcpy(s, h);
	return(0);
      }

    //redink1 added this function to load new tiles, because he is a l33t guy
    if (compare (ev[1], "load_tile"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 2, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    if (nlist[1] >= 1 && nlist[1] <= NB_TILE_SCREENS)
	      {
		//Load in the new tiles...
		tiles_load_slot(slist[0], nlist[1]);

		//Store in save game
		strncpy(play.tile[nlist[1]].file, slist[0], 50);
	      }
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added so developers can change or see what tile is at any given position
    if (compare (ev[1], "map_tile"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    //Yeah... they can only modify valid tiles
	    if (nlist[0] > 0 && nlist[0] <= 96)
	      {
		//Only change the value if it is greater than 0...
		if (nlist[1] > 0)
		  {
		    pam.t[nlist[0] - 1].num = nlist[1];
		  }
		returnint = pam.t[nlist[0] - 1].num;
		return (0);
	      }
	  }
	returnint = -1;
	return (0);
      }

    //redink1 added so a developer can retrieve/modify a hard tile
    if (compare (ev[1], "map_hard_tile"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    //Yeah... they can only modify valid tiles
	    if (nlist[0] > 0 && nlist[0] <= 96)
	      {
		//Only change the value if it is greater than 0...
		if (nlist[1] > 0)
		  {
		    pam.t[nlist[0] - 1].althard = nlist[1];
		  }
		returnint = pam.t[nlist[0] - 1].althard;
		return (0);
	      }
	  }
	returnint = -1;
	return (0);
      }

    //redink1 added this function to load a pallete from any bmp
    if (compare (ev[1], "load_palette"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    char *name = slist[0];
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
	strcpy(s, h);
	return(0);
      }

    //redink1 added to get index of specified item
    if (compare (ev[1], "get_item"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms(ev[1], script, h, p))
	  {
	    returnint = 0;
	    for (int i = 1; i < 17; i++)
	      {
		if (play.item[i].active)
		  {
		    if (compare(play.item[i].name, slist[0]))
		      {
			returnint = i;
			break;
		      }
		  }
	      }
	    return(0);
	  }
	returnint = -1;
	return(0);
      }

    //redink1 added to get index of specified magic spell
    if (compare (ev[1], "get_magic"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms(ev[1], script, h, p))
	  {
	    returnint = 0;
	    for (int i = 1; i < 9; i++)
	      {
		if (play.mitem[i].active)
		  {
		    if (compare(play.mitem[i].name, slist[0]))
		      {
			returnint = i;
			break;
		      }
		  }
	      }
	    return(0);
	  }
	returnint = -1;
	return(0);
      }

    //redink1 - sets font color
    if (compare (ev[1], "set_font_color"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };
	if (get_parms(ev[1], script, h, p))
	  {
	    set_font_color(nlist[0],
			   nlist[1], nlist[2], nlist[3]);
	  }
	strcpy(s, h);
	return(0);
      }

    //redink1 added this to make Paul Pliska's life more fulfilling
    if (compare (ev[1], "get_next_sprite_with_this_brain"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    for (int i = nlist[2]; i <= last_sprite_created; i++)
	      {
		if ((spr[i].brain == nlist[0]) && (i != nlist[1]))
		  if (spr[i].active == 1)
		    {
		      Msg ("Ok, sprite with brain %d is %d", nlist[0], i);
		      returnint = i;
		      return (0);
		    }
	      }
	  }
	Msg ("Ok, sprite with brain %d is 0", nlist[0], i);
	returnint = 0;
	return (0);
      }

    if (compare (ev[1], "set_smooth_follow"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    if (nlist[0] == 0)
	      {
		smooth_follow = 0;
	      }
	    else if (nlist[0] == 1)
	      {
		smooth_follow = 1;
	      }
	    return (0);
	  }
	returnint = -1;
	return (0);
      }
    
    //redink1
    if (compare(ev[1], "set_dink_base_push"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = {1,0,0,0,0,0,0,0,0,0};  
	if (get_parms(ev[1], script, h, p))
	  dink_base_push = nlist[0];
	strcpy(s, h);  
	return(0);
      }

    // redink1 added
    if (compare (ev[1], "callback_kill"))
      {
	Msg ("setting callback random");
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  kill_callback(nlist[0]);
	strcpy (s, h);
	return (0);
      }
    }

  /***************/
  /** Operators **/
  /**           **/
  /***************/

                                                                if (compare(ev[2], "="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[1];
                                                                        strip_beginning_spaces(h);
                                                                        var_equals(ev[1], ev[3], '=', script, h);
                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "+="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);
                                                                        var_equals(ev[1], ev[3], '+', script, h);
                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "*="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);
                                                                        var_equals(ev[1], ev[3], '*', script, h);
                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }



                                                                if (compare(ev[2], "-="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);

                                                                        var_equals(ev[1], ev[3], '-', script, h);

                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }


                                                                if (compare(ev[2], "/")
								    || (dversion >= 108 && compare(ev[2], "/=")))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[1];
                                                                        strip_beginning_spaces(h);

                                                                        var_equals(ev[1], ev[3], '/', script, h);

                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "*"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[1];
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
	h = &h[strlen (ev[1])];
	int
	p[20] = { 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 };
	memset (slist, 0, 10 * 200);
	get_parms (ev[1], script, h, p);
	if (strlen (slist[0]) > 0 && strlen (slist[1]) > 0)
	  {
	    int
	      myscript1 =
	      load_script (slist[0], rinfo[script]->sprite, 0);
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
		return (2);
	      }
	    else
	      {
		Msg ("Error:  Couldn't find procedure %s in %s.", slist[1],
		     slist[0]);
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
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	get_parms (ev[1], script, h, p);
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
	    Msg
	      ("ERROR:  Procedure void %s( void ); not found in script %s. (word 2 was %s) ",
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

                                                                        h = &h[strlen(ev[1])];
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
                                                                                } else
                                                                                {
                                                                                        Msg("Error:  Couldn't find procedure %s in %s.", slist[1], slist[0]);
                                                                                        kill_script(myscript1);
                                                                                }
                                                                        }
                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }

                                                                if (strchr(h, '(') != NULL)
                                                                {

                                                                        //lets attempt to run a procedure

                                                                        separate_string(h, 1,'(',line);


                                                                        int myscript = load_script(rinfo[script]->name, rinfo[script]->sprite, /*false*/0);

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
                                                                        }


                                                                        return(0);

                                                                }

                                                                Msg("MERROR: \"%s\" unknown in %s, offset %d.",ev[1], rinfo[script]->name,rinfo[script]->current);


                                                                //in a thingie, ready to go
    }
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
