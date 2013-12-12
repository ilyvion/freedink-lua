/**
 * Lua DinkC integration

 * Copyright (C) 2013  Alexander Krivács Schrøder

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

#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "log.h"
#include "game_engine.h"
#include "dinkvar.h"

// TODO: Move these somewhere else, perhaps? Consider when making Dink scripting
// TODO: a more modular endeavor

extern void dc_sp_active(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_attack_hit_sound(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_attack_hit_sound_speed(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_attack_wait(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_base_attack(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_base_die(int script, int* yield, int* preturnint, int sprite, int base_sequence);
extern void dc_sp_base_hit(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_base_idle(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_base_walk(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_brain(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_brain_parm(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_brain_parm2(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_defense(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_dir(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_disabled(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_distance(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_exp(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_flying(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_follow(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_frame(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_frame_delay(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_gold(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_hard(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_hitpoints(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_move_nohard(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_mx(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_my(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_noclip(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_nocontrol(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_nodraw(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_nohit(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_notouch(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_pframe(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_picfreeze(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_pseq(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_que(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_range(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_reverse(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_seq(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_size(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_sound(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_speed(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_strength(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_target(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_timing(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_touch_damage(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_x(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_y(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_kill(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_editor_num(int script, int* yield, int* preturnint, int sprite);
extern void dc_sp_kill_wait(int script, int* yield, int* preturnint, int sprite);
extern void dc_sp_script(int script, int* yield, int* preturnint, int sprite, char* dcscript);
extern void dc_unfreeze(int script, int* yield, int* preturnint, int sprite);
extern void dc_freeze(int script, int* yield, int* preturnint, int sprite);
extern void dc_set_callback_random(int script, int* yield, int* preturnint, char* procedure, int base, int range);
extern void dc_set_dink_speed(int script, int* yield, int* preturnint, int speed);
extern void dc_reset_timer(int script, int* yield, int* preturnint);
extern void dc_set_keep_mouse(int script, int* yield, int* preturnint, int keep_mouse_p);
extern void dc_add_item(int script, int* yield, int* preturnint, char* dcscript, int sequence, int frame);
extern void dc_add_magic(int script, int* yield, int* preturnint, char* dcscript, int sequence, int frame);
extern void dc_add_exp(int script, int* yield, int* preturnint, int amount, int active_sprite);
extern void dc_kill_this_item(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_kill_this_magic(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_show_bmp(int script, int* yield, int* preturnint, char* bmp_file, int show_map_dot, int unused);
extern void dc_copy_bmp_to_screen(int script, int* yield, int* preturnint, char* bmp_file);
extern void dc_wait_for_button(int script, int* yield, int* preturnint);
extern void dc_stop_wait_for_button(int script, int* yield, int* preturnint);
extern void dc_load_screen(int script, int* yield, int* preturnint);
extern void dc_say(int script, int* yield, int* preturnint, char* text, int active_sprite);
extern void dc_say_stop(int script, int* yield, int* preturnint, char* text, int active_sprite);
extern void dc_say_stop_npc(int script, int* yield, int* preturnint, char* text, int active_sprite);
extern void dc_say_stop_xy(int script, int* yield, int* preturnint, char* text, int x, int y);
extern void dc_say_xy(int script, int* yield, int* preturnint, char* text, int x, int y);
extern void dc_draw_screen(int script, int* yield, int* preturnint);
extern void dc_free_items(int script, int* yield, int* preturnint);
extern void dc_free_magic(int script, int* yield, int* preturnint);
extern void dc_kill_cur_item(int script, int* yield, int* preturnint);
extern void dc_kill_cur_magic(int script, int* yield, int* preturnint);
extern void dc_draw_status(int script, int* yield, int* preturnint);
extern void dc_arm_weapon(int script, int* yield, int* preturnint);
extern void dc_arm_magic(int script, int* yield, int* preturnint);
extern void dc_restart_game(int script, int* yield, int* preturnint);
extern void dc_wait(int script, int* yield, int* preturnint, int delayms);
extern void dc_preload_seq(int script, int* yield, int* preturnint, int sequence);
extern void dc_script_attach(int script, int* yield, int* preturnint, int sprite);
extern void dc_draw_hard_sprite(int script, int* yield, int* preturnint, int sprite);
extern void dc_activate_bow(int script, int* yield, int* preturnint);
extern void dc_disable_all_sprites(int script, int* yield, int* preturnint);
extern void dc_draw_background(int script, int* yield, int* preturnint);
extern void dc_draw_hard_map(int script, int* yield, int* preturnint);
extern void dc_enable_all_sprites(int script, int* yield, int* preturnint);
extern void dc_fade_down(int script, int* yield, int* preturnint);
extern void dc_fade_up(int script, int* yield, int* preturnint);
extern void dc_get_burn(int script, int* yield, int* preturnint);
extern void dc_get_last_bow_power(int script, int* yield, int* preturnint);
extern void dc_get_version(int script, int* yield, int* preturnint);
extern void dc_kill_all_sounds(int script, int* yield, int* preturnint);
extern void dc_kill_game(int script, int* yield, int* preturnint);
extern void dc_kill_this_task(int script, int* yield, int* preturnint);
extern void dc_scripts_used(int script, int* yield, int* preturnint);
extern void dc_stopcd(int script, int* yield, int* preturnint);
extern void dc_stopmidi(int script, int* yield, int* preturnint);
extern void dc_turn_midi_off(int script, int* yield, int* preturnint);
extern void dc_turn_midi_on(int script, int* yield, int* preturnint);
extern void dc_count_item(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_count_magic(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_compare_sprite_script(int script, int* yield, int* preturnint, int sprite, char* dcscript);
extern void dc_compare_weapon(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_compare_magic(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_init(int script, int* yield, int* preturnint, char* dink_ini_line);
extern void dc_dink_can_walk_off_screen(int script, int* yield, int* preturnint, int can_walk_off_screen_p);
extern void dc_push_active(int script, int* yield, int* preturnint, int dink_can_push_p);
extern void dc_stop_entire_game(int script, int* yield, int* preturnint, int stop_p);
extern void dc_editor_type(int script, int* yield, int* preturnint, int editor_sprite, int type);
extern void dc_editor_seq(int script, int* yield, int* preturnint, int editor_sprite, int seq);
extern void dc_editor_frame(int script, int* yield, int* preturnint, int editor_sprite, int frame);
extern void dc_move(int script, int* yield, int* preturnint, int sprite, int direction, int destination_limit, int ignore_hardness_p);
extern void dc_spawn(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_run_script_by_number(int script, int* yield, int* preturnint, int script_index, char* funcname);
extern void dc_playmidi(int script, int* yield, int* preturnint, char* midi_file);
extern void dc_playsound(int script, int* yield, int* preturnint, int sound_number, int min_speed, int rand_speed_to_add, int sprite, int repeat_p);
extern void dc_sound_set_survive(int script, int* yield, int* preturnint, int sound_bank, int survive_p);
extern void dc_sound_set_vol(int script, int* yield, int* preturnint, int sound_bank, int vol);
extern void dc_sound_set_kill(int script, int* yield, int* preturnint, int sound_bank);
extern void dc_save_game(int script, int* yield, int* preturnint, int game_slot);
extern void dc_force_vision(int script, int* yield, int* preturnint, int vision);
extern void dc_fill_screen(int script, int* yield, int* preturnint, int palette_index);
extern void dc_load_game(int script, int* yield, int* preturnint, int game_slot);
extern void dc_game_exist(int script, int* yield, int* preturnint, int game_slot);
extern void dc_move_stop(int script, int* yield, int* preturnint, int sprite, int direction, int destination_limit, int ignore_hardness_p);
extern void dc_load_sound(int script, int* yield, int* preturnint, char* wav_file, int sound_index);
extern void dc_debug(int script, int* yield, int* preturnint, char* text);
extern void dc_busy(int script, int* yield, int* preturnint, int sprite);
extern void dc_make_global_int(int script, int* yield, int* preturnint, char* varname, int default_val);
extern void dc_inside_box(int script, int* yield, int* preturnint, int x, int y, int left, int right, int top, int bottom);
extern void dc_random(int script, int* yield, int* preturnint, int range, int base);
extern void dc_initfont(int script, int* yield, int* preturnint, char* fontname);
extern void dc_set_mode(int script, int* yield, int* preturnint, int newmode);
extern void dc_kill_shadow(int script, int* yield, int* preturnint, int sprite);
extern void dc_create_sprite(int script, int* yield, int* preturnint, int x, int y, int brain, int sequence, int frame);
extern void dc_sp(int script, int* yield, int* preturnint, int editor_sprite);
extern void dc_is_script_attached(int script, int* yield, int* preturnint, int sprite);
extern void dc_get_sprite_with_this_brain(int script, int* yield, int* preturnint, int brain, int sprite_ignore);
extern void dc_get_rand_sprite_with_this_brain(int script, int* yield, int* preturnint, int brain, int sprite_ignore);
extern void dc_set_button(int script, int* yield, int* preturnint, int button, int function);
extern void dc_hurt(int script, int* yield, int* preturnint, int sprite, int damage);
extern void dc_screenlock(int script, int* yield, int* preturnint, int param);
extern void dc_sp_blood_num(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_blood_seq(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_clip_bottom(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_clip_left(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_clip_right(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_clip_top(int script, int* yield, int* preturnint, int sprite, int sparg);
extern void dc_sp_custom(int script, int* yield, int* preturnint, char* key, int sprite, int val);
extern void dc_sp_move_x(int script, int* yield, int* preturnint, int sprite, int dx);
extern void dc_sp_move_y(int script, int* yield, int* preturnint, int sprite, int dy);
extern void dc_sp_freeze(int script, int* yield, int* preturnint, int sprite, int frozen_p);
extern void dc_clear_editor_info(int script, int* yield, int* preturnint);
extern void dc_get_date_day(int script, int* yield, int* preturnint);
extern void dc_get_date_month(int script, int* yield, int* preturnint);
extern void dc_get_date_year(int script, int* yield, int* preturnint);
extern void dc_get_time_game(int script, int* yield, int* preturnint);
extern void dc_get_time_real(int script, int* yield, int* preturnint);
extern void dc_get_truecolor(int script, int* yield, int* preturnint);
extern void dc_show_console(int script, int* yield, int* preturnint);
extern void dc_show_inventory(int script, int* yield, int* preturnint);
extern void dc_var_used(int script, int* yield, int* preturnint);
extern void dc_loopmidi(int script, int* yield, int* preturnint, int loop_midi);
extern void dc_math_abs(int script, int* yield, int* preturnint, int val);
extern void dc_math_sqrt(int script, int* yield, int* preturnint, int val);
extern void dc_math_mod(int script, int* yield, int* preturnint, int val, int div);
extern void dc_make_global_function(int script, int* yield, int* preturnint, char* dcscript, char* procname);
extern void dc_set_save_game_info(int script, int* yield, int* preturnint, char* info);
extern void dc_load_map(int script, int* yield, int* preturnint, char* mapdat_file, char* dinkdat_file);
extern void dc_load_tile(int script, int* yield, int* preturnint, char* tileset_file, int tileset_index);
extern void dc_map_tile(int script, int* yield, int* preturnint, int tile_position, int tile_index);
extern void dc_map_hard_tile(int script, int* yield, int* preturnint, int tile_position, int hard_tile_index);
extern void dc_load_palette(int script, int* yield, int* preturnint, char* bmp_file);
extern void dc_get_item(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_get_magic(int script, int* yield, int* preturnint, char* dcscript);
extern void dc_set_font_color(int script, int* yield, int* preturnint, int index, int r, int g, int b);
extern void dc_get_next_sprite_with_this_brain(int script, int* yield, int* preturnint, int brain, int sprite_ignore, int sprite_start_with);
extern void dc_set_smooth_follow(int script, int* yield, int* preturnint, int smooth_p);
extern void dc_set_dink_base_push(int script, int* yield, int* preturnint, int base_sequence);
extern void dc_callback_kill(int script, int* yield, int* preturnint, int callback_index);

extern int search_var_with_this_scope(char* variable, int scope);

extern lua_State* luaVM;
extern int current_lua_script;

#define LUAREG(name) {#name, lua_dinkc_ ## name}

#define LUA_REPORT_NUMBER_ERROR(name)                       \
if (!lua_isnumber(l, -1))                                   \
{                                                           \
  return luaL_error(l, #name " must be set to a number");   \
}

#define LUA_REPORT_BOOLEAN_ERROR(name)                      \
if (!lua_isboolean(l, -1))                                  \
{                                                           \
  return luaL_error(l, #name " must be set to a boolean");  \
}

#define LUA_REPORT_STRING_ERROR(name)                       \
if (!lua_isstring(l, -1))                                   \
{                                                           \
  return luaL_error(l, #name " must be set to a string");   \
}

int lua_dinkc_debug(lua_State *l)
{
  int yield, returnint;
  const char* debug_string = lua_tostring(l, -1);
  dc_debug(current_lua_script, &yield, &returnint, debug_string);
  return 0;
}

int lua_dinkc_load_sound(lua_State *l)
{
  int yield, returnint;
  const char* wav_file = lua_tostring(l, -2);
  int sound_index = lua_tointeger(l, -1);
  dc_load_sound(current_lua_script, &yield, &returnint,
           wav_file, sound_index);
  return 0;
}

int lua_dinkc_fill_screen(lua_State *l)
{
  int yield, returnint;
  int palette_index = lua_tointeger(l, -1);
  dc_fill_screen(current_lua_script, &yield, &returnint, palette_index);
  return 0;
}

int lua_dinkc_create_sprite_dc(lua_State *l)
{
  int yield, returnint;
  int x = lua_tointeger(l, -5);
  int y = lua_tointeger(l, -4);
  int brain = lua_tointeger(l, -3);
  int sequence = lua_tointeger(l, -2);
  int frame = lua_tointeger(l, -1);
  dc_create_sprite(current_lua_script, &yield, &returnint,
                   x, y, brain, sequence, frame);
  lua_pushinteger(l, returnint);
  return 1;
}

int lua_dinkc_set_global_dinkc_value(lua_State *l)
{
  const char* global_name = lua_tostring(l, -2);
  char variable_name[strlen(global_name) + 2];
  sprintf(variable_name, "&%s", global_name);
  
  int global_value = lua_tointeger(l, -1);

  int variable = search_var_with_this_scope(variable_name, 0);
  if (variable < 1)
    return luaL_error (l, "No global variable %s exists.", variable_name);
  
  play.var[variable].var = global_value;
  log_debug("lua set global var %s to value %d", variable_name, global_value);
  return 0;
}

int lua_dinkc_get_global_dinkc_value(lua_State *l)
{
  const char* global_name = lua_tostring(l, -1);
  char variable_name[strlen(global_name) + 2];
  sprintf(variable_name, "&%s", global_name);

  int variable = search_var_with_this_scope(variable_name, 0);
  if (variable < 1)
    return luaL_error (l, "No global variable %s exists.", variable_name);

  lua_pushinteger (l, play.var[variable].var);
  return 1;
}

int lua_dinkc_set_sprite_value(lua_State *l)
{
  int sprite_number = lua_tointeger(l, -3);
  const char* sprite_command = lua_tostring(l, -2);

  log_debug("lua set sprite value %s on sprite %d",
            sprite_command, sprite_number);

  int yield, returnint;
  if (strcmp(sprite_command, "seq") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(seq);
    
    int sequence_number = lua_tointeger(l, -1);
    dc_sp_seq(current_lua_script, &yield, &returnint,
              sprite_number, sequence_number);
  }
  else if (strcmp(sprite_command, "brain") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(brain);

    int brain_number = lua_tointeger(l, -1);
    dc_sp_brain(current_lua_script, &yield, &returnint,
                sprite_number, brain_number);
  }
  else if (strcmp(sprite_command, "pseq") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(pseq);
    
    int sequence_number = lua_tointeger(l, -1);
    dc_sp_pseq(current_lua_script, &yield, &returnint,
              sprite_number, sequence_number);
  }
  else if (strcmp(sprite_command, "pframe") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(pframe);
    
    int frame_number = lua_tointeger(l, -1);
    dc_sp_pframe(current_lua_script, &yield, &returnint,
              sprite_number, frame_number);
  }
  else if (strcmp(sprite_command, "que") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(que);
    
    int que = lua_tointeger(l, -1);
    dc_sp_que(current_lua_script, &yield, &returnint,
              sprite_number, que);
  }
  else if (strcmp(sprite_command, "noclip") == 0)
  {
    LUA_REPORT_BOOLEAN_ERROR(noclip);
    
    int noclip = lua_toboolean(l, -1);
    dc_sp_noclip(current_lua_script, &yield, &returnint,
              sprite_number, noclip);
  }
  else if (strcmp(sprite_command, "script") == 0)
  {
    LUA_REPORT_STRING_ERROR(script);
    
    const char* script = lua_tostring(l, -1);
    dc_sp_script(current_lua_script, &yield, &returnint, sprite_number, script);
  }
  else if (strcmp(sprite_command, "touch_damage") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(touch_damage);
    
    int touch_damage = lua_tointeger(l, -1);
    dc_sp_touch_damage(current_lua_script, &yield, &returnint,
              sprite_number, touch_damage);
  }
  else if (strcmp(sprite_command, "reverse") == 0)
  {
    LUA_REPORT_BOOLEAN_ERROR(reverse);
    
    int reverse = lua_toboolean(l, -1);
    dc_sp_reverse(current_lua_script, &yield, &returnint,
              sprite_number, reverse);
  }
  else if (strcmp(sprite_command, "x") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(x);
    
    int x = lua_tointeger(l, -1);
    dc_sp_x(current_lua_script, &yield, &returnint,
              sprite_number, x);
  }
  else if (strcmp(sprite_command, "y") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(y);
    
    int y = lua_tointeger(l, -1);
    dc_sp_y(current_lua_script, &yield, &returnint,
              sprite_number, y);
  }
  else if (strcmp(sprite_command, "base_walk") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(base_walk);
    
    int base_walk = lua_tointeger(l, -1);
    dc_sp_base_walk(current_lua_script, &yield, &returnint,
              sprite_number, base_walk);
  }
  else if (strcmp(sprite_command, "base_attack") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(base_attack);
    
    int base_attack = lua_tointeger(l, -1);
    dc_sp_base_attack(current_lua_script, &yield, &returnint,
              sprite_number, base_attack);
  }
  else if (strcmp(sprite_command, "dir") == 0)
  {
    LUA_REPORT_NUMBER_ERROR(dir);

    int dir = lua_tointeger(l, -1);
    dc_sp_dir(current_lua_script, &yield, &returnint,
                sprite_number, dir);
  }
  else
  {
    luaL_error (l, "Sprites do not have a '%s' property", sprite_command);
  }
  
  return 0;
}

int lua_dinkc_get_sprite_value(lua_State *l)
{
  lua_pop(luaVM, 2);
  return 0;
}

int lua_dinkc_get_version(lua_State *l)
{
  int yield, returnint;
  dc_get_version(current_lua_script, &yield, &returnint);
  lua_pushinteger(l, returnint);
  return 1;
}

int lua_dinkc_playmidi(lua_State *l)
{
  int yield, returnint;
  const char* midi_file = lua_tostring(l, -1);
  dc_playmidi(current_lua_script, &yield, &returnint,
         midi_file);
  return 0;
}

int lua_dinkc_kill_this_task(lua_State *l)
{
  int yield, returnint;
  dc_kill_this_task(current_lua_script, &yield, &returnint);
  return 0;
}

int lua_dinkc_say_xy_dc(lua_State *l)
{
  int yield, returnint;
  
  const char* text = lua_tostring(l, -3);
  int x = lua_tointeger(l, -2);
  int y = lua_tointeger(l, -1);
  dc_say_xy(current_lua_script, &yield, &returnint, text, x, y);
  lua_pushinteger(l, returnint);
  return 1;
}

int lua_dinkc_wait_dc(lua_State *l)
{
  int yield, returnint;
  
  int amount = lua_tointeger(l, -1);
  dc_wait(current_lua_script, &yield, &returnint, amount);
  return 0;
}

int lua_dinkc_kill_game_dc(lua_State *l)
{
  int yield, returnint;
  dc_kill_game(current_lua_script, &yield, &returnint);
  return 0;
}

int lua_dinkc_playsound_dc(lua_State *l)
{
  int yield, returnint;
  int sound_number = lua_tointeger(l, -5);
  int min_speed = lua_tointeger(l, -4);
  int rand_speed_to_add = lua_tointeger(l, -3);
  int sprite = lua_tointeger(l, -2);
  int repeat_p = lua_toboolean(l, -1);
  dc_playsound(current_lua_script, &yield, &returnint,
		  sound_number, min_speed, rand_speed_to_add, sprite, repeat_p);
  lua_pushinteger(l, returnint);
  return 1;
}

int lua_dinkc_set_mode(lua_State *l)
{
  int yield, returnint;
  
  int newmode = lua_tointeger(l, -1);
  dc_set_mode(current_lua_script, &yield, &returnint, newmode);
  lua_pushinteger(l, returnint);
  return 1;
}

int lua_dinkc_reset_timer(lua_State *l)
{
  int yield, returnint;
  
  dc_reset_timer(current_lua_script, &yield, &returnint);
  return 0;
}

int lua_dinkc_add_item(lua_State *l)
{
  int yield, returnint;
  
  const char* scriptname = lua_tostring(l, -3);
  int sequence = lua_tointeger(l, -2);
  int frame = lua_tointeger(l, -1);
  dc_add_item(current_lua_script, &yield, &returnint, scriptname, sequence, frame);
  return 0;
}

int lua_dinkc_arm_weapon(lua_State *l)
{
  int yield, returnint;
  
  dc_arm_weapon(current_lua_script, &yield, &returnint);
  return 0;
}

void lua_bind_dinkc()
{
  luaL_Reg dinkc_funcs[] =
  {
    LUAREG(debug),    
    LUAREG(load_sound),
    LUAREG(fill_screen),
    LUAREG(get_version),
    LUAREG(playmidi),
    LUAREG(kill_this_task),
    LUAREG(set_mode),
    LUAREG(reset_timer),
    LUAREG(add_item),
    LUAREG(arm_weapon),
    {NULL, NULL}
  };

  luaL_newlib(luaVM, dinkc_funcs);
  lua_setglobal(luaVM, "dinkc");
  
  luaL_Reg wrapper_funcs[] =
  {
    LUAREG(set_sprite_value),
    LUAREG(get_sprite_value),
    LUAREG(set_global_dinkc_value),
    LUAREG(get_global_dinkc_value),
    LUAREG(create_sprite_dc),
    LUAREG(say_xy_dc),
    LUAREG(wait_dc),
    LUAREG(kill_game_dc),
    LUAREG(playsound_dc),
  };

  luaL_newlib(luaVM, wrapper_funcs);
  lua_setglobal(luaVM, "wrapper");
}