/**
 * Header for code common to FreeDink and FreeDinkedit

 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2012  Sylvain Beucler

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
#include "game_engine.h"
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

struct sprite_placement
{
  int x, y;
  int seq, frame, type;  /* DinkC: editor_seq, editor_frame, editor_type */
  int size;
  BOOL_1BYTE active;
  int rotation, special, brain;
  
  char script[13+1]; /* attached DinkC script */
  int speed, base_walk, base_idle, base_attack, base_hit, timer, que;
  int hard;
  rect alt; /* trim left/top/right/bottom */
  int is_warp;
  int warp_map;
  int warp_x;
  int warp_y;
  int parm_seq;
  
  int base_die, gold, hitpoints, strength, defense, exp, sound, vision, nohit, touch_damage;
  int buff[5];
};

/* one screen from map.dat */
struct small_map
{
  struct tile t[12*8+1]; // 97 background tiles
  struct sprite_placement sprite[100+1];
  char script[20+1]; /* script to run when entering the script */
};



#define TALK_TITLE_BUFSIZ 3000
#define TALK_LINE_BUFSIZ 101
struct talk_struct
{
  char line[21][TALK_LINE_BUFSIZ];  /* dialog choices */
  int line_return[21]; /* return this number if chosen */
  char buffer[TALK_TITLE_BUFSIZ];   /* title */
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

//sub struct for tile hardness

struct block_y
{
  unsigned char y[51];
};

struct ts_block
{
  struct block_y x[51];
  BOOL_1BYTE used;
};

//struct for hardness info, INDEX controls which hardness each block has.  800 max
//types available.
#define HARDNESS_NB_TILES 800
struct hardness
{
  struct ts_block htile[HARDNESS_NB_TILES];
  /* default hardness for each background tile square, 12*8=96 tiles
     per screen but indexed % 128 in the code (so 128*(41-1)+12*8=5216
     used indexes instead of 12*8*41=3936). */
  short btile_default[GFX_TILES_NB_SQUARES];
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
extern int load_map_to(char* path, const int num, struct small_map* screen);
extern int load_map(const int num);
extern int load_script(char filename[15], int sprite, /*bool*/int set_sprite);
extern /*bool*/int locate(int script, char proc[20]);
extern void process_callbacks(void);
extern void run_script (int script);
extern void update_status_all(void);

extern /*bool*/int abort_this_flip;
extern int base_timing;
extern struct attackinfo_struct bow;
extern int dinkspeed;
extern int flife;
extern int flub_mode;
extern int fps_final;
extern int show_inventory;
extern int stop_entire_game;
extern int getpic(int h);

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
extern int mode;
extern unsigned long mold;

extern int *pupdate_status;
extern struct small_map pam;
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

/* Player */
extern /*bool*/int inside_box(int x1, int y1, rect box);
extern int walk_off_screen;

/* Sprites - state */
extern void add_exp_force(int num, int source_sprite);
extern void add_exp(int num, int killed_sprite);
extern void changedir( int dir1, int k,int base);
extern int get_pan(int h);
extern int get_vol(int h);

/* Sprites - action */
extern void kill_text_owned_by(int sprite);

/* Sprites - global */
extern void kill_sprite_all (int sprite);
extern int find_sprite(int editor_sprite);

/* Scripts */
extern int say_text_xy(char text[200], int mx, int my, int script);
extern void kill_all_scripts_for_real(void);
extern void kill_returning_stuff(int script);
extern int say_text(char text[200], int h, int script);

/* Map */
extern unsigned char get_hard(int x1, int y1);
extern unsigned char get_hard_play(int h, int x1, int y1);
extern void load_hard(void);
extern int load_info_to(char* path, struct map_info *mymap);
extern void load_info(void);

/* Dunno */
extern /*bool*/int mouse1;

/* Screen */
extern void update_screen_time(void);

/* OS */
extern int bActive; // is application active?
extern char *command_line; // command line params, used by doInit

//if true, will close app as soon as the message pump is empty
extern int g_b_kill_app;


/* Startup */
extern void pre_figure_out(char* line);
extern void figure_out(char* line);

/* Metadata */
extern int burn_revision;

/*
 * Game & editor
 */
/*bool*/int get_box (int h, rect * box_crap, rect * box_real);
extern /*bool*/int dinkedit;
extern int draw_map_tiny;
extern int cur_map;
extern struct hardness hmap;


/*
 * Editor
 */

extern void check_sprite_status(int h);
extern void add_hardness(int sprite, int num);
extern void fill_whole_hard(void);
extern int add_sprite_dumb(int x1, int y, int brain,int pseq, int pframe,int size);
extern /*bool*/int kill_last_sprite(void);
extern void check_frame_status(int h, int frame);
extern void flip_it_second(void);
extern void save_map(const int num);
extern void save_info(void);
extern int realhard(int tile);
extern void save_hard(void);

extern /*bool*/int no_running_main;

extern void fill_screen(int num);


/* Used by dinkc_bindings.c only */
enum item_type { ITEM_REGULAR, ITEM_MAGIC };
extern void clear_talk(void);
extern void add_item(char name[10], int mseq, int mframe, enum item_type type);
extern void kill_item_script(char* name);
extern void kill_mitem_script(char* name);
extern void show_bmp(char name[80], int showdot, int script);
extern void copy_bmp( char name[80]);
extern void kill_cur_item( void );
extern void kill_cur_magic( void );
extern /*bool*/int text_owned_by(int sprite);
extern void kill_all_vars(void);
extern void update_play_changes( void );
extern void fill_hardxy(rect box);
extern void save_game(int num);
extern /*bool*/int load_game(int num);
extern int does_sprite_have_text(int sprite);
extern int change_sprite(int h,  int val, int * change);
extern int change_sprite_noreturn(int h,  int val, int * change);
extern int change_edit_char(int h,  int val, unsigned char * change);
extern int change_edit(int h,  int val, unsigned short * change);
extern int hurt_thing(int h, int damage, int special);
extern void random_blood(int mx, int my, int h);
extern void check_sprite_status_full(int sprite_no);

#endif
