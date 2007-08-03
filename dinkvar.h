/**
 * Header for code common to FreeDink and FreeDinkedit

 * Copyright (C) 2005  Sylvain Beucler

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

 * I made this header because I though update_frame had to be compiled
 * separately - actually it was also included in dink.cpp. Might still
 * come in handy.
 */

#ifndef _DINKVAR_H
#define _DINKVAR_H

/* for RECT ?? */
#include <windows.h>
#include <ddraw.h>
#include <dinput.h>
#include <mmsystem.h>

#include "SDL.h"


const int max_vars = 250;
const int max_sprites_at_once = 300;
const int tile_screens = 41+1;

struct sprite_index
{
	int x,y;
	int s;
	int last;
	
};

struct attackinfo_struct
{
	int time;
	bool active;
	int script;
	bool hitme;
	int last_power;
	int wait;
	int pull_wait;
};

struct map_info
{
	char name[20];
	int loc[769];
	int music[769];
	int indoor[769];
	int v[40];
	char s[80];
	char buffer[2000];
	
};

struct tile
{
	int num, property, althard, more2;
	byte  more3,more4;
	
	int buff[15];
};

struct sprite_placement
{
  int x,y,seq,frame, type,size;
  bool active;
  int rotation, special,brain;
  
  char script[13];
  char hit[13];
  char die[13];
  char talk[13];
  int speed, base_walk,base_idle,base_attack,base_hit,timer,que;
  int hard;
  RECT alt;
  int prop;
  int warp_map;
  int warp_x;
  int warp_y;
  int parm_seq;
  
  int base_die, gold, hitpoints, strength, defense,exp, sound, vision, nohit, touch_damage;
  int buff[5];
};

struct small_map
{
  char name[20];
  tile t[97];
  int v[40];
  char s[80];
  sprite_placement sprite[101];
  
  char script[13];
  char random[13];
  char load[13];
  char buffer[1000];
};


struct item_struct
{
	bool active;
	char name[10];
    int seq;
	int frame;
	
};

struct mydata
{
	unsigned char type[100];
	unsigned short seq[100];
	unsigned char frame[100];
	int last_time;
};

struct varman
{
	int var;
	char name[20];
	int scope;
	bool active;
};

struct player_info
{
  int version;
  char gameinfo[196];
  int minutes;
  int x,y,die, size, defense, dir, pframe, pseq, seq, frame, strength, base_walk, base_idle,
    
    
    base_hit,que;
  
  item_struct	mitem[9]; //added one to these, because I don't like referring to a 0 item
  item_struct item[17];
  
  int curitem, unused;
  int counter;
  bool idle;
  mydata spmap[769];
  int button[10];
  varman var[max_vars];
  
  
  bool push_active;
  int push_dir;
  DWORD push_timer;
  int last_talk;
  int mouse;
  bool item_magic;
  int last_map;
  int crap;
  int buff[95];
  DWORD dbuff[20];
  
  long lbuff[10];
  
  char  cbuff[6000];
};

struct sequence
{
	
	int frame[51];
	int delay[51];
	unsigned char special[51];
	char data[80];
	bool active;
};

struct show_bmp
{
	bool active;
	bool showdot;
	int reserved;
	int script;
	int stime;
	int picframe;
};

struct seth_joy
{
	BOOL joybit[17]; //is button held down?
	BOOL letgo[17]; //copy of old above
	BOOL button[17]; //has button been pressed recently?
	BOOL key[256];
	BOOL kletgo[256];
	bool realkey[256];
	BOOL right,left,up,down;
	BOOL rightd,leftd,upd,downd;
	BOOL rightold,leftold,upold,downold;
	
	
	
};

struct sp
{
  int x,moveman;
  int y; 
  int mx,my;
  int lpx[51],lpy[51];
  int speed;
  int brain;
  int seq_orig,dir;
  int seq;
  int frame;
  DWORD delay;
  int pseq;
  int pframe;
  
  BOOL active;
  int attrib;
  DWORD wait;
  int timer;
  int skip;
  int skiptimer;
  int size;
  int que;
  int base_walk;
  int base_idle;
  int base_attack;
  
  int base_hit;
  int last_sound;
  int hard;
  RECT alt;
  int althard;
  int sp_index;
  BOOL nocontrol;
  int idle;
  int strength;
  int damage;
  int defense;
  int hitpoints;
  int exp;
  int gold;
  int base_die;
  int kill;
  Uint32 kill_timer;
  int script_num;
  char text[200];
  int owner;
  int script;
  int sound;
  int callback;
  int freeze;
  bool move_active;
  int move_script;
  int move_dir;
  int move_num;
  BOOL move_nohard;
  int follow;
  int nohit;
  BOOL notouch;
  DWORD notouch_timer;
  BOOL flying;
  int touch_damage;
  int brain_parm;
  int brain_parm2;
  BOOL noclip;
  BOOL reverse;
  BOOL disabled;
  int target;
  int attack_wait;
  int move_wait;
  int distance;
  int last_hit;
  BOOL live;
  int range;
  int attack_hit_sound;
  int attack_hit_sound_speed;
  int action;
  int nodraw;
  int frame_delay;
  int picfreeze;
};


struct talk_struct
{
  char line[21][101];
  int line_return[21];
  char buffer[3000];
  int cur;
  int last;
  bool active;
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

struct pic_info
{
	
	
	LPDIRECTDRAWSURFACE     k;       // Sprites
	RECT                box,hardbox;
	
	
	
	int yoffset;
	int xoffset;
};

struct wait_for_button
{
	int script;
	int button;
	bool active;
};

//sub struct for hardness map
struct mega_y
{
	byte y[401];
};

//struct for hardness map
struct hit_map
{
	mega_y x[601];
};


//sub struct for tile hardness

struct block_y
{
	byte y[51];
};

struct ts_block
{
	block_y x[51];
	bool used;
	int hold;
};

//struct for hardness info, INDEX controls which hardness each block has.  800 max
//types available.
struct hardness
{
	ts_block tile[800];
	int index[8000];
};

extern int GetKeyboard(int key);
extern void Msg( LPSTR fmt, ... );
extern int add_sprite(int x1, int y, int brain,int pseq, int pframe );
extern void check_seq_status(int h);
extern void dderror(HRESULT hErr);
extern void draw_sprite_game(LPDIRECTDRAWSURFACE lpdest,int h);
extern void draw_status_all(void);
extern void drawallhard( void);
extern void duck_brain(int h);
extern BOOL init_mouse(HWND hwnd);
extern void load_map(const int num);
extern int load_script(char filename[15], int sprite, bool set_sprite);
extern bool locate(int script, char proc[20]);
extern void process_callbacks(void);
extern void run_script (int script);
extern void update_sound(void);
extern void update_status_all(void);

/* string_utils */
extern bool compare(char *orig, char *comp);
bool separate_string (char str[255], int num, char liney, char *return1);

extern bool abort_this_flip;
extern int base_timing;
extern attackinfo_struct bow;
extern bool debug_mode;
extern int dinkspeed;
extern char dversion_string[7];
extern int flife;
extern int flub_mode;
extern int fps_final;
extern HANDLE g_hevtMouse;
extern LPDIRECTINPUTDEVICE g_pMouse;
extern LPDIRECTINPUT g_pdi;
extern bool item_screen;
extern pic_info k[];
extern int stop_entire_game;
extern int getpic(int h);
extern HWND hWndMain;
extern HFONT hfont_small;
extern sprite_index index[];
extern int keep_mouse;
extern char last_debug[200];
extern int last_sprite_created;
extern map_info map;
extern int mbase_count;
extern int mbase_timing;
extern int mcc;
extern int mode;
extern DWORD mold;
extern bool process_downcycle;
extern bool process_upcycle;
extern int process_warp;
extern int *pupdate_status;
extern bool move_gonna;
extern bool no_cheat;
extern bool no_transition;
extern small_map pam;
extern int plane_process;
extern player_info play;
extern int playl;
extern int playx;
extern int playy;
extern int *plife;
extern int *pmap;
extern int screenlock;
extern sequence seq[];
extern int show_dot;
extern show_bmp showb;
extern seth_joy sjoy;
extern sp spr[];
extern talk_struct talk;
extern Uint32 thisTickCount;
extern Uint32 lastTickCount;
extern bool total_trigger;
extern bool trig_man;
extern bool turn_on_plane;
extern int x;
extern int y;

/* Sound - BGM */
extern bool midi_active;
extern bool sound_on;
extern bool cd_inserted;

/* Drawing */
extern void initfonts(char fontname[255]);

extern HRESULT ddrval;
extern LPDIRECTDRAWPALETTE lpDDPal; /* The primary surface palette */
extern PALETTEENTRY    pe[256];


/* Joystick */
extern BOOL joystick;
extern JOYINFOEX jinfo;
extern struct wait_for_button wait;

/* Game state */
extern bool add_time_to_saved_game(int num);
extern void attach(void);
extern bool windowed;
extern int * pvision, * plife, * presult, * pspeed, * ptiming, *plifemax, \
  *pexper, *pmap, *pstrength, * pcur_weapon,* pcur_magic, *pdefense, \
  *pgold, *pmagic, *plevel, *plast_text, *pmagic_level;
extern int *pupdate_status, *pmissile_target, *penemy_sprite, \
  *pmagic_cost, *pmissle_source;
extern int flife, fexp, fstrength, fdefense, fgold, fmagic, fmagic_level, \
  flifemax, fraise, last_magic_draw;
extern int fcur_weapon, fcur_magic;
extern int push_active;
extern int move_screen;
extern int move_counter;
extern int weapon_script;
extern int magic_script;
extern DWORD cycle_clock;
extern int cycle_script;
extern int process_count;
extern int item_timer;
extern int item_pic;
extern int last_saved_game;

/* Player */
extern bool inside_box(int x1, int y1, RECT box);
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
extern int say_text(char text[200], int h, int script);

/* Map */
extern byte get_hard(int h,int x1, int y1);
extern byte get_hard_play(int h,int x1, int y1);
extern void load_hard(void);
extern void load_info(void);
extern "C" IDirectDrawSurface * DDTileLoad(IDirectDraw *pdd, LPCSTR szBitmap, int dx, int dy, int sprite);
SDL_Surface *GFX_DDTileLoad(char* filename, int sprite);

extern hit_map hm;

/* Dunno */
extern void kill_returning_stuff( int script);
extern void program_idata(void);
extern bool mouse1;
extern char dir[80];

/* Screen */
extern void update_screen_time(void);

/* OS */
extern void log_path(bool playing);
extern void TRACE( LPSTR fmt, ... );
extern bool exist(char name[255]);
extern int bActive; // is application active?
extern int g_b_no_write_ini; // -noini passed to command line?
extern char *command_line; // command line params, used by doInit
extern HINSTANCE MyhInstance; // app instance, used by DX init
extern char dinkpath[MAX_PATH]; // Dink installation directory

//if true, will close app as soon as the message pump is empty
extern int g_b_kill_app;


/* Startup */
extern void pre_figure_out(char line[255], int load_seq);

/* Metadata */
extern int burn_revision;

/*
 * Editor
 */
const int max_sequences = 1000; //Max # of sprite animations

bool get_box (int h, RECT * box_crap, RECT * box_real);
extern void check_sprite_status(int h);
extern void add_hardness(int sprite, int num);
extern void fill_whole_hard(void);
extern int add_sprite_dumb(int x1, int y, int brain,int pseq, int pframe,int size);
extern bool kill_last_sprite(void);
extern void check_frame_status(int h, int frame);
extern void Say(char thing[500], int px, int py);
extern void flip_it_second(void);
extern void save_map(const int num);
extern void add_text(char *tex ,char *filename);
extern void save_info(void);
extern int realhard(int tile);
extern void SaySmall(char thing[500], int px, int py, int r, int g, int b);
extern void save_hard(void);

extern RECT tilerect[tile_screens];
extern SDL_Rect GFX_tilerect[tile_screens];
extern int draw_map_tiny;
extern int cur_map, cur_tile;
extern int map_vision;
extern hardness hmap;
extern int hard_tile;
extern int m1x,m1y;
extern int m2x, m2y;
extern int m3x, m3y;
extern int last_sprite_added;
extern int sp_brain;
extern int sp_speed;
extern bool sp_screenmatch;
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
extern bool in_enabled;
extern int sp_mode;
extern bool dinkedit;


/* Used by gfx_tiles.cpp only */
extern void place_sprites_game(void);
extern void place_sprites_game_background(void);
extern void kill_all_scripts(void);
extern void init_scripts(void);
extern bool no_running_main;

extern void fill_screen(int num);
#endif
