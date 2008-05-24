/**
 * Header for code common to FreeDink and FreeDinkedit

 * Copyright (C) 2005  Sylvain Beucler

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

#ifndef _DINKVAR_H
#define _DINKVAR_H

/* for RECT ?? */
/* #include <windows.h> */

/* #include <ddraw.h> */
/* #include <dinput.h> */
/* #include <mmsystem.h> */

#include <limits.h>

#include "SDL.h"
#include "SDL_framerate.h"
#include "rect.h"
#include "dinkc.h"

struct attackinfo_struct
{
	int time;
	/*bool*/int active;
	int script;
	/*bool*/int hitme;
	int last_power;
	int wait;
	int pull_wait;
};


struct tile
{
	int num, property, althard, more2;
	unsigned char more3,more4;
	
	int buff[15];
};

struct sprite_placement
{
  int x,y,seq,frame, type,size;
  BOOL_1BYTE active;
  int rotation, special,brain;
  
  char script[13];
  char hit[13];
  char die[13];
  char talk[13];
  int speed, base_walk,base_idle,base_attack,base_hit,timer,que;
  int hard;
  rect alt;
  int prop;
  int warp_map;
  int warp_x;
  int warp_y;
  int parm_seq;
  
  int base_die, gold, hitpoints, strength, defense,exp, sound, vision, nohit, touch_damage;
  int buff[5];
};

/* one screen from map.dat */
struct small_map
{
  char name[20];
  struct tile t[97];
  int v[40];
  char s[80];
  struct sprite_placement sprite[101];
  
  char script[13];
  char random[13];
  char load[13];
  char buffer[1000];
};



struct talk_struct
{
  char line[21][101];
  int line_return[21];
  char buffer[3000];
  int cur;
  int last;
  /*bool*/int active;
  int cur_view;
  int cur_view_end;
  int page;
  int script;
  int offset;
  int newy;
  int color;
  int curf;
  int timer;
};

//sub struct for hardness map
struct mega_y
{
	unsigned char y[401];
};

//struct for hardness map
struct hit_map
{
	struct mega_y x[601];
};


//sub struct for tile hardness

struct block_y
{
	unsigned char y[51];
};

struct ts_block
{
	struct block_y x[51];
	BOOL_1BYTE used;
	int hold;
};

//struct for hardness info, INDEX controls which hardness each block has.  800 max
//types available.
struct hardness
{
	struct ts_block tile[800];
	int index[8000];
};

extern int GetKeyboard(int key);
extern int add_sprite(int x1, int y, int brain,int pseq, int pframe );
extern void check_seq_status(int h);
/* extern void dderror(HRESULT hErr); */
//extern void draw_sprite_game(LPDIRECTDRAWSURFACE lpdest,int h);
extern void draw_sprite_game(SDL_Surface *GFX_lpdest, int h);
extern void draw_status_all(void);
extern void drawallhard( void);
extern void duck_brain(int h);
extern /*BOOL*/int init_mouse();
extern void load_map(const int num);
extern int load_script(char filename[15], int sprite, /*bool*/int set_sprite);
extern /*bool*/int locate(int script, char proc[20]);
extern void process_callbacks(void);
extern void run_script (int script);
extern void update_sound(void);
extern void update_status_all(void);

/* string_utils */
extern /*bool*/int compare(char *orig, char *comp);
/*bool*/int separate_string (char str[255], int num, char liney, char *return1);

extern /*bool*/int abort_this_flip;
extern int base_timing;
extern struct attackinfo_struct bow;
extern int dinkspeed;
extern int flife;
extern int flub_mode;
extern int fps_final;
/* extern HANDLE g_hevtMouse; */
/* extern LPDIRECTINPUTDEVICE g_pMouse; */
/* extern LPDIRECTINPUT g_pdi; */
extern /*bool*/int item_screen;
extern int stop_entire_game;
extern int getpic(int h);
/* extern HWND hWndMain; */
/* extern HFONT hfont_small; */


/* show_bmp() currently ran */
struct show_bmp
{
	/*bool*/int active;
	/*bool*/int showdot;
	int reserved;
	int script;
	int stime;
	int picframe;
};
extern struct show_bmp showb;


extern int keep_mouse;
extern int last_sprite_created;
extern int mbase_count;
extern int mbase_timing;
extern int mcc;
extern int mode;
extern unsigned long mold;
extern /*bool*/int process_downcycle;
extern /*bool*/int process_upcycle;
extern int process_warp;
extern int *pupdate_status;
extern /*bool*/int move_gonna;
extern /*bool*/int no_cheat;
extern /*bool*/int no_transition;
extern struct small_map pam;
extern int plane_process;
extern int playl;
extern int playx;
extern int playy;
extern int *pmap;
extern int screenlock;
extern int show_dot;
extern struct talk_struct talk;
extern Uint32 thisTickCount;
extern Uint32 lastTickCount;
extern FPSmanager framerate_manager;
extern /*bool*/int total_trigger;
extern /*bool*/int trig_man;
extern /*bool*/int turn_on_plane;
extern int x;
extern int y;

/* extern HRESULT ddrval; */
/* extern LPDIRECTDRAWPALETTE lpDDPal; /\* The primary surface palette *\/ */
/* extern PALETTEENTRY    pe[256]; */


/* Game state */
extern /*bool*/int add_time_to_saved_game(int num);
extern void attach(void);
extern /*bool*/int windowed;
extern int fcur_weapon, fcur_magic;
extern int push_active;
extern int move_screen;
extern int move_counter;
extern int weapon_script;
extern int magic_script;
extern unsigned long cycle_clock;
extern int cycle_script;
extern int process_count;
extern int item_timer;
extern int item_pic;

/* Player */
extern /*bool*/int inside_box(int x1, int y1, rect box);
extern int walk_off_screen;

/* Sprites - state */
extern void add_exp(int num, int h);
extern void changedir( int dir1, int k,int base);
extern int get_pan(int h);
extern int get_vol(int h);

/* Sprites - action */
extern void kill_text_owned_by(int sprite);

/* Sprites - global */
extern void kill_sprite_all (int sprite);

/* Scripts */
extern int say_text_xy(char text[200], int mx, int my, int script);
extern void kill_all_scripts_for_real(void);
extern void kill_returning_stuff(int script);
extern int say_text(char text[200], int h, int script);

/* Map */
extern unsigned char get_hard(int h,int x1, int y1);
extern unsigned char get_hard_play(int h,int x1, int y1);
extern void load_hard(void);
extern void load_info(void);

extern struct hit_map hm;

/* Dunno */
extern void program_idata(void);
extern /*bool*/int mouse1;

/* Screen */
extern void update_screen_time(void);

/* OS */
extern int bActive; // is application active?
extern char *command_line; // command line params, used by doInit

//if true, will close app as soon as the message pump is empty
extern int g_b_kill_app;


/* Startup */
extern void pre_figure_out(char line[255], int load_seq);

/* Metadata */
extern int burn_revision;

/*
 * Editor
 */
#define MAX_SEQUENCES 1000 /* Max # of sprite animations */

/*bool*/int get_box (int h, rect * box_crap, rect * box_real);
extern void check_sprite_status(int h);
extern void add_hardness(int sprite, int num);
extern void fill_whole_hard(void);
extern int add_sprite_dumb(int x1, int y, int brain,int pseq, int pframe,int size);
extern /*bool*/int kill_last_sprite(void);
extern void check_frame_status(int h, int frame);
extern void Say(char thing[500], int px, int py);
extern void flip_it_second(void);
extern void save_map(const int num);
extern void save_info(void);
extern int realhard(int tile);
extern void save_hard(void);

extern int draw_map_tiny;
extern int cur_map, cur_tile;
extern int map_vision;
extern struct hardness hmap;
extern int hard_tile;
extern int m1x,m1y;
extern int m2x, m2y;
extern int m3x, m3y;
extern int last_sprite_added;
extern int sp_brain;
extern int sp_speed;
extern /*bool*/int sp_screenmatch;
extern int in_huh;
extern int in_master;
extern int in_command;
extern int in_max;
extern int *in_int;
extern char in_default[200];
extern int in_onflag;
extern char *in_string;
extern char in_temp[200];
extern int in_x, in_y;
extern /*bool*/int in_enabled;
extern int sp_mode;
extern /*bool*/int dinkedit;

extern /*bool*/int no_running_main;

extern void fill_screen(int num);


/* Used by gfx_tiles.c only */
extern void place_sprites_game(void);
extern void place_sprites_game_background(void);
extern void kill_all_scripts(void);
extern void init_scripts(void);


/* Used by dinkc_bindings.c only */
extern void clear_talk(void);
extern int var_figure(char h[200], int script);
extern void add_item(char name[10], int mseq, int mframe, /*bool*/int magic);
extern void kill_cur_item_script( char name[20]);
extern void kill_cur_magic_script( char name[20]);
extern void show_bmp(char name[80], int showdot, int reserved, int script);
extern void copy_bmp( char name[80]);
extern void kill_cur_item( void );
extern void kill_cur_magic( void );
extern /*bool*/int text_owned_by(int sprite);
extern void kill_all_vars(void);
extern void update_play_changes( void );
extern void fill_hardxy(rect box);
extern void fill_back_sprites(void );
extern void fill_hard_sprites(void );
extern void save_game(int num);
extern /*bool*/int load_game(int num);
extern int does_sprite_have_text(int sprite);
extern int change_sprite(int h,  int val, int * change);
extern int change_sprite_noreturn(int h,  int val, int * change);
extern int change_edit_char(int h,  int val, unsigned char * change);
extern int change_edit(int h,  int val, unsigned short * change);
extern int hurt_thing(int h, int damage, int special);
extern void random_blood(int mx, int my, int h);
extern void figure_out(char line[255], int load_seq);

#endif
