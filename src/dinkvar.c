/**
 * Common code for FreeDink and FreeDinkedit

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2003  Shawn Betts
 * Copyright (C) 2005, 2007  Sylvain Beucler

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

#ifndef __DEMO
#define __REAL_THING
#endif

//set language
#ifndef __LANGUAGE
#define __ENGLISH
#endif

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

#ifdef _WIN32
/* GetWindowsDirectory */
#include <windows.h>
#endif
/* #include <windowsx.h> */
/* #include <direct.h> */
/* #include <io.h> */
#include <fcntl.h>
/* #include <process.h> */


/* #include <mmsystem.h> */
/* #define DIRECTINPUT_VERSION 0x0700 */
/* #include <dinput.h> */
/* #include <ddraw.h> */

#include "SDL.h"
#include "SDL_framerate.h"
#include "SDL_rotozoom.h"

/* #include "ddutil.h" */
#include "fastfile.h"
#include "io_util.h"


#include "freedink.h"
#include "dinkvar.h"
#include "gfx.h"
#include "gfx_tiles.h"
#include "gfx_utils.h"
/* for DinkC's initfonts(): */
#include "gfx_fonts.h"
#include "bgm.h"
#include "sfx.h"

#include "str_util.h"

int g_b_no_write_ini = 0;
/*bool*/int no_cheat =  /*true*/1;
const int dversion = 107;
int last_saved_game = 0;

//if true, will close app as soon as the message pump is empty
int g_b_kill_app = 0;

int dinkspeed = 3;
char current_map[255] = "MAP.DAT";
time_t time_start;
/*bool*/int item_screen = /*false*/0;
/*bool*/int midi_active = /*true*/1;
char dversion_string[7] = "v1.07";

int load_script(char filename[15], int sprite, /*bool*/int set_sprite);
void strchar(char *string, char ch);
/* Path where Dink is installed. Used to write dinksmallwood.ini in
   log_path(), and in the editor (?) */
char dinkpath[PATH_MAX];

void update_status_all(void);
int add_sprite(int x1, int y, int brain,int pseq, int pframe );

void add_exp(int num, int h);
/*bool*/int locate(int script, char proc[20]);
/*bool*/int exist(char name[255]);
void draw_status_all(void);
void check_seq_status(int h);

void get_word(char line[300], int word, char *crap);
void run_script (int script);
void add_text(char *tex ,char *filename);
void program_idata(void);
int map_vision = 0;
int realhard(int tile);
int flub_mode = -500;
unsigned short decipher_savegame = 0;
int draw_map_tiny = -1;

char last_debug[200];
int walk_off_screen = /*false*/0;
char cbuf[64000];
/*bool*/int cd_inserted;
const int old_burn = 0;

/* Skip flipping the double buffer for this frame only - used when
   setting up show_bmp and copy_bmp */
/*bool*/int abort_this_flip = /*false*/0;

int var_exists(char name[20], int scope);
void make_int(char name[80], int value, int scope, int script);





struct show_bmp showb;

int keep_mouse = 0;

struct wait_for_button wait;



struct attackinfo_struct bow;

int screenlock = 0;

struct talk_struct talk;

struct idata
{
        int type;
        int seq;
        int frame;
        int xoffset, yoffset;
        rect hardbox;
};

int * pvision, * plife, * presult, * pspeed, * ptiming, *plifemax, *pexper, *pmap,
*pstrength, * pcur_weapon,* pcur_magic, *pdefense, *pgold, *pmagic, *plevel, *plast_text, *pmagic_level;
int *pupdate_status, *pmissile_target, *penemy_sprite, *pmagic_cost, *pmissle_source;
int flife, fexp, fstrength, fdefense, fgold, fmagic, fmagic_level, flifemax, fraise, last_magic_draw;
int fcur_weapon, fcur_magic;
#define MAX_IDATA 600
int mbase_timing;
struct idata id[MAX_IDATA];

unsigned long mold;

int item_timer;
int item_pic;

int mbase_count;
/*bool*/int no_transition = /*false*/0;




struct player_short_info
{
        int version;
        char gameinfo[196];
        int minutes;

};

struct player_short_info short_play;


int push_active = 1;


/*bool*/int turn_on_plane = /*FALSE*/0;
const int text_timer = 77;
const int text_min = 2700;
#define MAX_SPRITES 4000
int stop_entire_game;
#define MAX_CALLBACKS 100
#define MAX_SCRIPTS 200
const int max_game = 20;
/*bool*/int in_enabled = /*false*/0;
char *in_string;


struct refinfo
{
        char name[10];
        long location;
        long current;
        int level;
        long end;
        int sprite; //if more than 0, it was spawned and is owned by a sprite, if 1000 doesn't die
        /*bool*/int skipnext;
        int onlevel;
        int proc_return;
};


/*bool*/int first_frame = /*false*/0;

/* If true, and if the engine is executing a screen's attached script,
   and if main() loads new graphics (preload_seq()...), then
   load_sprites and load_sprite_pak will display a "Please Wait"
   animation. */
/*bool*/int no_running_main = /*false*/0;

struct call_back
{

        int owner;
        /*bool*/int active;
    int type;
        char name[20];
        int offset;
        long min, max;
    int lifespan;
    unsigned long timer;
};


struct call_back callback[MAX_CALLBACKS];
/* TODO: Used 1->100 in the game, should it be MAX_CALLBACKS+1 ? */

char *rbuf[MAX_SCRIPTS]; //pointers to buffers we may need

struct refinfo *rinfo[MAX_SCRIPTS];

int process_warp = 0;
/* Tell the engine that we're fading down */
/*bool*/int process_downcycle = /*false*/0;
/* or fading up */
/*bool*/int process_upcycle = /*false*/0;
unsigned long cycle_clock = 0;
int cycle_script = 0;

int *in_int;
int in_x, in_y;
int sp_brain = 0;
int returnint = 0;
char returnstring[200];
int sp_speed = 0;
char slist[10][200];
long nlist[10];
int process_count = 0;
rect sp_alt;
int hard_tile = 0;
/*bool*/int sp_screenmatch = 0;
char in_temp[200];
int in_command;
int in_finish;
int in_onflag;
int in_max = 10;
int in_huh = 0;
char in_default[200];
int in_master = 0;

/*bool*/int sound_on = /*true*/1;
char dir[80];


//defaults




int process_line (int script, char *s, /*bool*/int doelse);




/*bool*/int please_wait = /*false*/0;
int  show_dot = /*FALSE*/0;
int  plane_process = /*TRUE*/1;
struct hit_map hm;

/* HWND     g_hWnd; */
struct sprite_index s_index[MAX_SEQUENCES];
int last_sprite_added = 0;
unsigned long timer = 0;
char *command_line;
/*bool*/int dinkedit = /*false*/0;
int base_timing;
int weapon_script = 0;
int magic_script = 0;

int sp_mode = 0;
int fps,fps_final = 0;
int last_sprite_created;
int move_screen = 0;
/*bool*/int move_gonna = /*false*/0;
int move_counter = 0;
int m1x,m1y;
int m2x,m2y;
int m3x,m3y;
int playx = 620;
/*bool*/int windowed = /*false*/0;
int playl = 20;
/* HINSTANCE MyhInstance = NULL; */
/*bool*/int mouse1 = /*false*/0;
int cur_sprite = 1;
int playy = 399;
int cur_map,cur_tile;
struct seth_joy sjoy;

/* Number of ms since an arbitrarily fixed point */
Uint32 thisTickCount,lastTickCount;
/* SDL_gfx accurate framerate */
FPSmanager framerate_manager;

unsigned long timecrap;
rect math,box_crap,box_real;

/* HRESULT             ddrval; */
int sz,sy,x_offset,y_offset;
/* DDBLTFX     ddbltfx; */



int mode;

struct sequence seq[MAX_SEQUENCES];
struct map_info map;
struct small_map pam;


/*bool*/int trig_man = /*false*/0;
/*bool*/int total_trigger = /*false*/0;
/*bool*/int debug_mode = /*false*/0;

struct pic_info     k[MAX_SPRITES];       // Sprite data
struct GFX_pic_info GFX_k[MAX_SPRITES];   // Sprite data (SDL)

struct player_info play;




/* LPDIRECTDRAWSURFACE     game[max_game];       // Game pieces */

struct sp spr[MAX_SPRITES_AT_ONCE]; //max sprite control systems at once
/* LPDIRECTDRAWPALETTE     lpDDPal = NULL;        // The primary surface palette */
/* PALETTEENTRY    pe[256]; */

int bActive = /*false*/0;        // is application active/foreground?
//LPDIRECTINPUT lpDI;


//direct input stuff for mouse reading

/* LPDIRECTINPUT          g_pdi = NULL; */
/* LPDIRECTINPUTDEVICE    g_pMouse = NULL; */
/* #define DINPUT_BUFFERSIZE           16 */

/* HANDLE                 g_hevtMouse = NULL; */


//LPCDIDATAFORMAT lpc;

unsigned char torusColors[256];  // Marks the colors used in the torus


/* HWND                    hWndMain = NULL; */
//JOYINFOEX jinfo; //joystick info
SDL_Joystick* jinfo;
int joystick = /*true*/1;
struct hardness hmap;



void replace(const char *this1, char *that, char *line)
{

        char hold[500];
        char thisup[200],lineup[500];
        int u,i;
        int checker;
start:
        strcpy(hold,"");

        strcpy(lineup,line);
        strcpy(thisup,this1);

        strtoupper(lineup);
        strtoupper(thisup);
        if (strstr(lineup,thisup) == NULL) return;
        checker = -1;
        strcpy(hold,"");
        for (u = 0; u < strlen(line); u++)
        {
                if (checker > -1)
                {
                        if (toupper(line[u]) == toupper(this1[checker]))
                        {
                                if (checker+1 == strlen(this1))
                                {
doit:
                                u = u - strlen(this1);
                                u++;
                                for (i = 0; i < u; i++) hold[i] = line[i];
                                for (i = 0; i < strlen(that); i++) hold[(u)+i]=that[i];
                                hold[strlen(that)+u] = 0;
                                for (i = 0; i < (strlen(line)-u)-strlen(this1); i++)
                                {
                                        hold[(u+strlen(that))+i] = line[(u+strlen(this1))+i];
                                }
                                hold[(strlen(line)-strlen(this1))+strlen(that)] = 0;
                                strcpy(line,hold);
                                goto start;
                                }
                                checker++;
                          } else { checker = -1;    }
                }
                if( checker == -1)
                {
                        if (toupper(line[u]) == toupper(this1[0]))
                        {

                                //      if (line[u] < 97) that[0] = toupper(that[0]);
                                checker = 1;
                                if (strlen(this1) == 1) goto doit;
                        }
                }
        }
}


/**
 * Split 'str' in words separated by 'liney', and copy the #'num' one
 * to 'return1'. The function does not alter 'str'.
 **/
/*bool*/int separate_string (char str[255], int num, char liney, char *return1)
{
  int l;
  int k;

  l = 1;
  strcpy(return1, "");

  for (k = 0; k <= strlen(str); k++)
    {
      if (str[k] == liney)
	{
	  l++;
	  if (l == num+1)
	    goto done;

	  if (k < strlen(str))
	    strcpy(return1, "");
	}
      if (str[k] != liney)
	sprintf(return1, "%s%c", return1, str[k]);
    }

  if (l < num)
    strcpy(return1, "");

  replace("\r", "", return1); //Take the /r off it.
  replace("\n", "", return1); //Take the /n off it.

  return /*false*/0;

done:
  if (l < num)
    strcpy(return1, "");

  replace("\r", "", return1); //Take the /r off it.
  replace("\n", "", return1); //Take the /n off it.

  //Msg("Took %s and turned it to %s.",str, return1);
  return /*true*/1;
}



void clear_talk(void)
{
        memset(&talk, 0, sizeof(talk));
        play.mouse = 0;
}



void reverse(char *st)
{
        int i,ii;
        char don[255];
        don[0] = 0;
        ii = strlen(st);
        for (i=ii; i > -1; i--)
        {
                strchar(don, st[i]);
        }
        strcpy(st, don);
}


char * lmon(long money, char *dest)
{
        char ho[30];
        int k,c;
        char lmon1[30];
        char buffer[30];
        /*BOOL*/int quit1;
        quit1 = /*FALSE*/0;

        sprintf(buffer, "%ld", money);
        strcpy(lmon1, buffer);
        // prf("ORG IS '%s'",lmon1);

        if (strlen(lmon1) < 4)
        {
                strcpy(dest, lmon1);
                return(dest);
        }

        sprintf(buffer, "%ld", money);
        strcpy(ho, buffer);
        k = strlen(ho);
        c = -1;
        lmon1[0]=0;
        do {
                strchar(lmon1,ho[k]);
                k--;
                c++;
                if (c == 3)
                {
                        if (k > -1)
                        {
                                strchar(lmon1,',');
                                c = 0;
                        }
                }
                if (k < 0) quit1 = /*TRUE*/1;
        }while (quit1 == /*FALSE*/0);
        reverse(lmon1);

        strcpy(dest, lmon1);
        return(dest);
}



/* void dderror(HRESULT hErr) */
/* {        */
/*     switch (hErr) */
/*     { */
/*         case DDERR_ALREADYINITIALIZED: */
/*                 Msg("DDERR_ALREADYINITIALIZED"); break; */
/*         case DDERR_CANNOTATTACHSURFACE: */
/*                 Msg("DDERR_CANNOTATTACHSURFACE"); break; */
/*         case DDERR_CANNOTDETACHSURFACE: */
/*                 Msg("DDERR_CANNOTDETACHSURFACE"); break; */
/*         case DDERR_CURRENTLYNOTAVAIL: */
/*                 Msg("DDERR_CURRENTLYNOTAVAIL"); break; */
/*         case DDERR_EXCEPTION: */
/*                 Msg("DDERR_EXCEPTION"); break; */
/*         case DDERR_GENERIC: */
/*                 Msg("DDERR_GENERIC"); break; */
/*         case DDERR_HEIGHTALIGN: */
/*                 Msg("DDERR_HEIGHTALIGN"); break; */
/*         case DDERR_INCOMPATIBLEPRIMARY: */
/*                 Msg("DDERR_INCOMPATIBLEPRIMARY"); break; */
/*         case DDERR_INVALIDCAPS: */
/*                 Msg("DDERR_INVALIDCAPS"); break; */
/*         case DDERR_INVALIDCLIPLIST: */
/*                 Msg("DDERR_INVALIDCLIPLIST"); break; */
/*         case DDERR_INVALIDMODE: */
/*                 Msg("DDERR_INVALIDMODE"); break; */
/*         case DDERR_INVALIDOBJECT: */
/*                 Msg("DDERR_INVALIDOBJECT"); break; */
/*         case DDERR_INVALIDPARAMS: */
/*                 Msg("DDERR_INVALIDPARAMS"); break; */
/*         case DDERR_INVALIDPIXELFORMAT: */
/*                 Msg("DDERR_INVALIDPIXELFORMAT"); break; */
/*         case DDERR_INVALIDRECT: */
/*                 Msg("DDERR_INVALIDRECT"); break; */
/*         case DDERR_LOCKEDSURFACES: */
/*                 Msg("DDERR_LOCKEDSURFACES"); break; */
/*         case DDERR_NO3D: */
/*                 Msg("DDERR_NO3D"); break; */
/*         case DDERR_NOALPHAHW: */
/*                 Msg("DDERR_NOALPHAHW"); break; */
/*         case DDERR_NOCLIPLIST: */
/*                 Msg("DDERR_NOCLIPLIST"); break; */
/*         case DDERR_NOCOLORCONVHW: */
/*                 Msg("DDERR_NOCOLORCONVHW"); break; */
/*         case DDERR_NOCOOPERATIVELEVELSET: */
/*                 Msg("DDERR_NOCOOPERATIVELEVELSET"); break; */
/*         case DDERR_NOCOLORKEY: */
/*                 Msg("DDERR_NOCOLORKEY"); break; */
/*         case DDERR_NOCOLORKEYHW: */
/*                 Msg("DDERR_NOCOLORKEYHW"); break; */
/*         case DDERR_NODIRECTDRAWSUPPORT: */
/*                 Msg("DDERR_NODIRECTDRAWSUPPORT"); break; */
/*         case DDERR_NOEXCLUSIVEMODE: */
/*                 Msg("DDERR_NOEXCLUSIVEMODE"); break; */
/*         case DDERR_NOFLIPHW: */
/*                 Msg("DDERR_NOFLIPHW"); break; */
/*         case DDERR_NOGDI: */
/*                 Msg("DDERR_NOGDI"); break; */
/*         case DDERR_NOMIRRORHW: */
/*                 Msg("DDERR_NOMIRRORHW"); break; */
/*         case DDERR_NOTFOUND: */
/*                 Msg("DDERR_NOTFOUND"); break; */
/*         case DDERR_NOOVERLAYHW: */
/*                 Msg("DDERR_NOOVERLAYHW"); break; */
/*         case DDERR_NORASTEROPHW: */
/*                 Msg("DDERR_NORASTEROPHW"); break; */
/*         case DDERR_NOROTATIONHW: */
/*                 Msg("DDERR_NOROTATIONHW"); break; */
/*         case DDERR_NOSTRETCHHW: */
/*                 Msg("DDERR_NOSTRETCHHW"); break; */
/*         case DDERR_NOT4BITCOLOR: */
/*                 Msg("DDERR_NOT4BITCOLOR"); break; */
/*         case DDERR_NOT4BITCOLORINDEX: */
/*                 Msg("DDERR_NOT4BITCOLORINDEX"); break; */
/*         case DDERR_NOT8BITCOLOR: */
/*                 Msg("DDERR_NOT8BITCOLOR"); break; */
/*         case DDERR_NOTEXTUREHW: */
/*                 Msg("DDERR_NOTEXTUREHW"); break; */
/*         case DDERR_NOVSYNCHW: */
/*                 Msg("DDERR_NOVSYNCHW"); break; */
/*         case DDERR_NOZBUFFERHW: */
/*                 Msg("DDERR_NOZBUFFERHW"); break; */
/*         case DDERR_NOZOVERLAYHW: */
/*                 Msg("DDERR_NOZOVERLAYHW"); break; */
/*         case DDERR_OUTOFCAPS: */
/*                 Msg("DDERR_OUTOFCAPS"); break; */
/*         case DDERR_OUTOFMEMORY: */
/*                 Msg("DDERR_OUTOFMEMORY"); break; */
/*         case DDERR_OUTOFVIDEOMEMORY: */
/*                 Msg("DDERR_OUTOFVIDEOMEMORY"); break; */
/*         case DDERR_OVERLAYCANTCLIP: */
/*                 Msg("DDERR_OVERLAYCANTCLIP"); break; */
/*         case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: */
/*                 Msg("DDERR_OVERLAYCOLORKEYONLYONEACTIVE"); break; */
/*         case DDERR_PALETTEBUSY: */
/*                 Msg("DDERR_PALETTEBUSY"); break; */
/*         case DDERR_COLORKEYNOTSET: */
/*                 Msg("DDERR_COLORKEYNOTSET"); break; */
/*         case DDERR_SURFACEALREADYATTACHED: */
/*                 Msg("DDERR_SURFACEALREADYATTACHED"); break; */
/*         case DDERR_SURFACEALREADYDEPENDENT: */
/*                 Msg("DDERR_SURFACEALREADYDEPENDENT"); break; */
/*         case DDERR_SURFACEBUSY: */
/*                 Msg("DDERR_SURFACEBUSY"); break; */
/*         case DDERR_CANTLOCKSURFACE: */
/*                 Msg("DDERR_CANTLOCKSURFACE"); break; */
/*         case DDERR_SURFACEISOBSCURED: */
/*                 Msg("DDERR_SURFACEISOBSCURED"); break; */
/*         case DDERR_SURFACELOST: */
/*                 Msg("DDERR_SURFACELOST"); break; */
/*         case DDERR_SURFACENOTATTACHED: */
/*                 Msg("DDERR_SURFACENOTATTACHED"); break; */
/*         case DDERR_TOOBIGHEIGHT: */
/*                 Msg("DDERR_TOOBIGHEIGHT"); break; */
/*         case DDERR_TOOBIGSIZE: */
/*                 Msg("DDERR_TOOBIGSIZE"); break; */
/*         case DDERR_TOOBIGWIDTH: */
/*                 Msg("DDERR_TOOBIGWIDTH"); break; */
/*         case DDERR_UNSUPPORTED: */
/*                 Msg("DDERR_UNSUPPORTED"); break; */
/*         case DDERR_UNSUPPORTEDFORMAT: */
/*                 Msg("DDERR_UNSUPPORTEDFORMAT"); break; */
/*         case DDERR_UNSUPPORTEDMASK: */
/*                 Msg("DDERR_UNSUPPORTEDMASK"); break; */
/*         case DDERR_VERTICALBLANKINPROGRESS: */
/*                 Msg("DDERR_VERTICALBLANKINPROGRESS"); break; */
/*         case DDERR_WASSTILLDRAWING: */
/*                 Msg("DDERR_WASSTILLDRAWING"); break; */
/*         case DDERR_XALIGN: */
/*                 Msg("DDERR_XALIGN"); break; */
/*         case DDERR_INVALIDDIRECTDRAWGUID: */
/*                 Msg("DDERR_INVALIDDIRECTDRAWGUID"); break; */
/*         case DDERR_DIRECTDRAWALREADYCREATED: */
/*                 Msg("DDERR_DIRECTDRAWALREADYCREATED"); break; */
/*         case DDERR_NODIRECTDRAWHW: */
/*                 Msg("DDERR_NODIRECTDRAWHW"); break; */
/*         case DDERR_PRIMARYSURFACEALREADYEXISTS: */
/*                 Msg("DDERR_PRIMARYSURFACEALREADYEXISTS"); break; */
/*         case DDERR_NOEMULATION: */
/*                 Msg("DDERR_NOEMULATION"); break; */
/*         case DDERR_REGIONTOOSMALL: */
/*                 Msg("DDERR_REGIONTOOSMALL"); break; */
/*         case DDERR_CLIPPERISUSINGHWND: */
/*                 Msg("DDERR_CLIPPERISUSINGHWND"); break; */
/*         case DDERR_NOCLIPPERATTACHED: */
/*                 Msg("DDERR_NOCLIPPERATTACHED"); break; */
/*         case DDERR_NOHWND: */
/*                 Msg("DDERR_NOHWND"); break; */
/*         case DDERR_HWNDSUBCLASSED: */
/*                 Msg("DDERR_HWNDSUBCLASSED"); break; */
/*         case DDERR_HWNDALREADYSET: */
/*                 Msg("DDERR_HWNDALREADYSET"); break; */
/*         case DDERR_NOPALETTEATTACHED: */
/*                 Msg("DDERR_NOPALETTEATTACHED"); break; */
/*         case DDERR_NOPALETTEHW: */
/*                 Msg("DDERR_NOPALETTEHW"); break; */
/*         case DDERR_BLTFASTCANTCLIP: */
/*                 Msg("DDERR_BLTFASTCANTCLIP"); break; */
/*         case DDERR_NOBLTHW: */
/*                 Msg("DDERR_NOBLTHW"); break; */
/*         case DDERR_NODDROPSHW: */
/*                 Msg("DDERR_NODDROPSHW"); break; */
/*         case DDERR_OVERLAYNOTVISIBLE: */
/*                 Msg("DDERR_OVERLAYNOTVISIBLE"); break; */
/*         case DDERR_NOOVERLAYDEST: */
/*                 Msg("DDERR_NOOVERLAYDEST"); break; */
/*         case DDERR_INVALIDPOSITION: */
/*                 Msg("DDERR_INVALIDPOSITION"); break; */
/*         case DDERR_NOTAOVERLAYSURFACE: */
/*                 Msg("DDERR_NOTAOVERLAYSURFACE"); break; */
/*         case DDERR_EXCLUSIVEMODEALREADYSET: */
/*                 Msg("DDERR_EXCLUSIVEMODEALREADYSET"); break; */
/*         case DDERR_NOTFLIPPABLE: */
/*                 Msg("DDERR_NOTFLIPPABLE"); break; */
/*         case DDERR_CANTDUPLICATE: */
/*                 Msg("DDERR_CANTDUPLICATE"); break; */
/*         case DDERR_NOTLOCKED: */
/*                 Msg("DDERR_NOTLOCKED"); break; */
/*         case DDERR_CANTCREATEDC: */
/*                 Msg("DDERR_CANTCREATEDC"); break; */
/*         case DDERR_NODC: */
/*                 Msg("DDERR_NODC"); break; */
/*         case DDERR_WRONGMODE: */
/*                 Msg("DDERR_WRONGMODE"); break; */
/*         case DDERR_IMPLICITLYCREATED: */
/*                 Msg("DDERR_IMPLICITLYCREATED"); break; */
/*         case DDERR_NOTPALETTIZED: */
/*                 Msg("DDERR_NOTPALETTIZED"); break; */
/*         case DDERR_UNSUPPORTEDMODE: */
/*                 Msg("DDERR_UNSUPPORTEDMODE"); break; */
/*         case DDERR_NOMIPMAPHW: */
/*                 Msg("DDERR_NOMIPMAPHW"); break; */
/*         case DDERR_INVALIDSURFACETYPE: */
/*                 Msg("DDERR_INVALIDSURFACETYPE"); break; */
/*         case DDERR_DCALREADYCREATED: */
/*                 Msg("DDERR_DCALREADYCREATED"); break; */
/*         case DDERR_CANTPAGELOCK: */
/*                 Msg("DDERR_CANTPAGELOCK"); break; */
/*         case DDERR_CANTPAGEUNLOCK: */
/*                 Msg("DDERR_CANTPAGEUNLOCK"); break; */
/*         case DDERR_NOTPAGELOCKED: */
/*                 Msg("DDERR_NOTPAGELOCKED"); break; */
/*         case DDERR_NOTINITIALIZED: */
/*                 Msg("DDERR_NOTINITIALIZED"); break; */
/*         default: */
/*                 Msg("Unknown Error"); break; */
/*         } */
/*         Msg("\n"); */
/* } */


/*bool*/int compare(char *orig, char *comp)
{

        int len;

        //strcpy(comp, _strupr(comp));
        //strcpy(orig, _strupr(orig));


        len = strlen(comp);
        if (strlen(orig) != len) return(/*false*/0);


        if (strncasecmp(orig,comp,len) == 0)
        {
                return(/*true*/1);
        }

        //Msg("I'm sorry, but %s does not equal %s.",orig, comp);

        return(/*false*/0);
}

/**
 * Save where Dink is installed in a .ini file, read by third-party
 * applications like the DinkFrontEnd. Also notify whether Dink is
 * running or not.
 */
void log_path(/*bool*/int playing)
{
  if (g_b_no_write_ini)
    return; //fix problem with NT security if -noini is set
  /* TODO: saves it in the user home instead. Think about where to
     cleanly store additional DMods. */

#ifdef _WIN32
  char windir[100];
  char inifile[256];
  GetWindowsDirectory(windir, 256);
  sprintf(inifile, "%s\\dinksmallwood.ini", windir);

  unlink(inifile);

  add_text("[Dink Smallwood Directory Information for the CD to read]\r\n", inifile);
  add_text(dinkpath, inifile);
  add_text("\r\n", inifile);
  if (playing)
    add_text("TRUE\r\n", inifile);
  else
    add_text("FALSE\r\n", inifile);
#endif
}



void Msg(char *fmt, ...)
{
    char    buff[350];
    va_list  va;

    va_start(va, fmt);

    //
    // format message with header
    //

    strcpy( buff, "Dink:" );
    vsprintf( &buff[strlen(buff)], fmt, va );
    strcat( buff, "\r\n" );

    vfprintf(stderr, fmt, va);
    fprintf(stderr, "\n");
    //
    // To the debugger unless we need to be quiet
    //



/*         OutputDebugString( buff ); */
        strcpy(last_debug, buff);
        if (debug_mode) add_text(buff, "DEBUG.TXT");


} /* Msg */

void TRACE(char *fmt, ...)
{
    char    buff[350];
    va_list  va;

    va_start(va, fmt);

    //
    // format message with header
    //

    strcpy( buff, "Dink:" );
    vsprintf( &buff[strlen(buff)], fmt, va );
    strcat( buff, "\r\n" );

    //
    // To the debugger unless we need to be quiet
    //



/*         OutputDebugString( buff ); */
        strcpy(last_debug, buff);
        if (debug_mode) add_text(buff, "DEBUG.TXT");


} /* Msg */


/* Like DDLoadBitmap, except that we don't check the existence of
   szBitmap, and we define the .box sprite attribute with the
   dimentions of the picture */
/* Used in load_sprites() and in freedinkedit.cpp */
/* extern "C" IDirectDrawSurface * DDSethLoad(IDirectDraw *pdd, LPCSTR szBitmap, int dx, int dy, int sprite) */
/* { */
/*         HBITMAP             hbm; */
/*         BITMAP              bm; */
/*         DDSURFACEDESC       ddsd; */
/*         IDirectDrawSurface *pdds; */

/*         // */
/*         //  try to load the bitmap as a resource, if that fails, try it as a file */
/*         // */
/*         hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION); */

/*         if (hbm == NULL) */
/*                 hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION); */

/*         if (hbm == NULL) */
/*                 return NULL; */

/*         // */
/*         // get size of the bitmap */
/*         // */
/*         GetObject(hbm, sizeof(bm), &bm);      // get size of bitmap */

/*         // */
/*         // create a DirectDrawSurface for this bitmap */
/*         // */
/*         ZeroMemory(&ddsd, sizeof(ddsd)); */
/*         ddsd.dwSize = sizeof(ddsd); */
/*         ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH; */
/*         ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; */
/*         ddsd.dwWidth = bm.bmWidth; */
/*         ddsd.dwHeight = bm.bmHeight; */

/*         if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK) */
/*                 return NULL; */

/*         DDCopyBitmap(pdds, hbm, 0, 0, 0, 0); */

/*         DeleteObject(hbm); */
/*         if (sprite > 0) */
/*         { */
/*                 k[sprite].box.top = 0; */
/*                 k[sprite].box.left = 0; */
/*                 k[sprite].box.right = ddsd.dwWidth; */
/*                 k[sprite].box.bottom = ddsd.dwHeight; */

/*         } */



/*         return pdds; */
/* } */


void add_text(char *tex ,char *filename)
{
  char tmp_filename[PATH_MAX];

  /* TODO: if DEBUG.TXT cannot be opened, Dink crashes */
        FILE *          fp;
        if (strlen(tex) < 1) return;

        if (exist(filename) == /*FALSE*/0)
        {

                fp = fopen(ciconvertbuf(filename, tmp_filename), "wb");
                fwrite( tex, strlen(tex), 1, fp);       /* current player */
                fclose(fp);
                return;
        } else
        {
                fp = fopen(ciconvertbuf(filename, tmp_filename), "ab");
                fwrite( tex, strlen(tex), 1, fp);       /* current player */
                fclose(fp);
        }
}


//add hardness from a sprite

int getpic(int h)
{
        if (spr[h].pseq == 0) return(0);
        if (spr[h].pseq > MAX_SEQUENCES)
        {

                Msg("Sequence %d?  But max is %d!", spr[h].pseq, MAX_SEQUENCES);
                return(0);
        }
        return(seq[spr[h].pseq].frame[spr[h].pframe]);

}


void add_hardness (int sprite, int num)
{
  int xx;
  for (xx = spr[sprite].x + k[getpic(sprite)].hardbox.left; xx < spr[sprite].x + k[getpic(sprite)].hardbox.right; xx++)
    {
      int yy;
      for (yy = spr[sprite].y + k[getpic(sprite)].hardbox.top; yy < spr[sprite].y + k[getpic(sprite)].hardbox.bottom; yy++)
	{
	  if ( (xx-20 > 600) | (xx-20 < 0)| (yy > 400) | (yy < 0))
	    ; /* Nothing */
	  else
	    hm.x[xx-20].y[yy] = num;
	}
    }
}




void setup_anim (int fr, int start,int delay)
{
  //** start is sprite sequence #, don't know why it is called start
  int o;
  for (o = 1; o <= s_index[start].last; o++)
    {
      seq[fr].frame[o] = s_index[start].s+o;
      seq[fr].delay[o] = delay;
    }

  seq[fr].frame[s_index[start].last+1] = 0;
}


unsigned char get_hard(int h,int x1, int y1)
{
        int value;

        if ((x1 < 0) || (y1 < 0)) return(/*false*/0);
        if ((x1 > 599) ) return(/*false*/0);
        if (y1 > 399) return(/*false*/0);

        value =  hm.x[x1].y[y1];


        return(value);
}

unsigned char get_hard_play(int h,int x1, int y1)
{
        int value;
        x1 -= 20;
        if ((x1 < 0) || (y1 < 0)) return(/*false*/0);
        if ((x1 > 599) ) return(/*false*/0);
        if (y1 > 399) return(/*false*/0);

        value =  hm.x[x1].y[y1];

        if (value > 100)
        {

                if (pam.sprite[value-100].prop != 0)

                {
                        flub_mode = value;
                        value = 0;
                }
        }

        return(value);
}


unsigned char get_hard_map(int h,int x1, int y1)
{


        if ((x1 < 0) || (y1 < 0)) return(0);
        if ((x1 > 599) ) return(0);
        if (y1 > 399) return(0);


        int til = (x1 / 50) + ( ((y1 / 50)) * 12);
        //til++;

        int offx = x1 - ((x1 / 50) * 50);


        int offy = y1 - ((y1 / 50) * 50);

        //Msg("tile %d ",til);

        return( hmap.tile[ realhard(til )  ].x[offx].y[offy]);

}



void fill_hardxy(rect box)
{
  int x1, y1;
  //Msg("filling hard of %d %d %d %d", box.top, box.left, box.right, box.bottom);

  if (box.right > 599) box.right = 599;
  if (box.top < 0) box.top = 0;
  if (box.bottom > 399) box.bottom = 399;
  if (box.left < 0) box.left = 0;


  for (x1 = box.left; x1 < box.right; x1++)
    for (y1 = box.top; y1 < box.bottom; y1++)
      hm.x[x1].y[y1] = get_hard_map(0,x1,y1);
}



void add_exp(int num, int h)
{
        if (spr[h].last_hit != 1) return;

        if (num > 0)
        {
                //add experience


                *pexper += num;


                int crap2 = add_sprite(spr[h].x,spr[h].y,8,0,0);

                spr[crap2].y -= k[seq[spr[h].pseq].frame[spr[h].pframe]].yoffset;
                spr[crap2].x -= k[seq[spr[h].pseq].frame[spr[h].pframe]].xoffset;
                spr[crap2].y -= k[seq[spr[h].pseq].frame[spr[h].pframe]].box.bottom / 3;
                spr[crap2].x += k[seq[spr[h].pseq].frame[spr[h].pframe]].box.right / 5;
                spr[crap2].y -= 30;
                spr[crap2].speed = 1;
                spr[crap2].hard = 1;
                spr[crap2].brain_parm = 5000;
                spr[crap2].my = -1;
                spr[crap2].kill = 1000;
                spr[crap2].dir = 8;
                spr[crap2].damage = num;


                if (*pexper > 99999) *pexper = 99999;


        }

}

int realhard(int tile)
{

        //      if (pam.t[tile].num > 3000) Msg("Hard is %d", pam.t[tile].num );
        if (pam.t[tile].althard > 0) return(pam.t[tile].althard); else return(hmap.index[pam.t[tile].num]);

}


void fill_whole_hard(void)
{
  int til;
  for (til=0; til < 96; til++)
    {
      int offx = (til * 50 - ((til / 12) * 600));
      int offy = (til / 12) * 50;
      int x, y;
      for (x = 0; x < 50; x++)
	for (y = 0; y < 50; y++)
	  hm.x[offx +x].y[offy+y] = hmap.tile[  realhard(til)  ].x[x].y[y];
    }
}

/* Draw harness. Used by freedinkedit and updateFrame() in hard-coded
   cheat mode. */
void drawallhard( void)
{
/*   rect box_crap; */
/*   int ddrval; */
/*   DDBLTFX     ddbltfx; */
  int x1, y1;

  /* TODO: test me! Then factor the code */
  for (x1=0; x1 < 600; x1++)
    for (y1=0; y1 < 400; y1++)
      {
	if (hm.x[x1].y[y1] == 1)
	  {
/* 	    ddbltfx.dwFillColor = 1; */
/* 	    ddbltfx.dwSize = sizeof(ddbltfx); */
/* 	    box_crap.top = y1; */
/* 	    box_crap.bottom = y1+1; */
/* 	    box_crap.left = x1+playl; //20 is to compensate for the border */
/* 	    box_crap.right = x1+1+playl; */
/* 	    ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 	    if (ddrval != DD_OK) Msg("There was an error!"); */
	    // GFX
	    {
	      SDL_Rect GFX_box_crap;
	      GFX_box_crap.x = x1 + playl;
	      GFX_box_crap.y = y1;
	      GFX_box_crap.w = 1;
	      GFX_box_crap.h = 1;
	      SDL_FillRect(GFX_lpDDSBack, &GFX_box_crap, 1);
	    }
	  }

	if (hm.x[x1].y[y1] == 2)
	  {
/* 	    ddbltfx.dwFillColor = 128; */
/* 	    ddbltfx.dwSize = sizeof(ddbltfx); */
/* 	    box_crap.top = y1; */
/* 	    box_crap.bottom = y1+1; */
/* 	    box_crap.left = x1+playl; //20 is to compensate for the border */
/* 	    box_crap.right = x1+1+playl; */
/* 	    ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 	    if (ddrval != DD_OK) Msg("There was an error!"); */
	    // GFX
	    {
	      SDL_Rect GFX_box_crap;
	      GFX_box_crap.x = x1 + playl;
	      GFX_box_crap.y = y1;
	      GFX_box_crap.w = 1;
	      GFX_box_crap.h = 1;
	      SDL_FillRect(GFX_lpDDSBack, &GFX_box_crap, 128);
	    }
	  }

	if (hm.x[x1].y[y1] == 3)
	  {
/* 	    ddbltfx.dwFillColor = 45; */
/* 	    ddbltfx.dwSize = sizeof(ddbltfx); */
/* 	    box_crap.top = y1; */
/* 	    box_crap.bottom = y1+1; */
/* 	    box_crap.left = x1+playl; //20 is to compensate for the border */
/* 	    box_crap.right = x1+1+playl; */
/* 	    ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 	    if (ddrval != DD_OK) Msg("There was an error!"); */
	    // GFX
	    {
	      SDL_Rect GFX_box_crap;
	      GFX_box_crap.x = x1 + playl;
	      GFX_box_crap.y = y1;
	      GFX_box_crap.w = 1;
	      GFX_box_crap.h = 1;
	      SDL_FillRect(GFX_lpDDSBack, &GFX_box_crap, 45);
	    }
	  }

	if (hm.x[x1].y[y1] > 100)
	  {

	    if (pam.sprite[  (hm.x[x1].y[y1]) - 100].prop == 1)
	      {
		//draw a little pixel
/* 		ddbltfx.dwFillColor = 20; */
/* 		ddbltfx.dwSize = sizeof(ddbltfx); */
/* 		box_crap.top = y1; */
/* 		box_crap.bottom = y1+1; */
/* 		box_crap.left = x1+playl; //20 is to compensate for the border */
/* 		box_crap.right = x1+1+playl; */
/* 		ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 		if (ddrval != DD_OK) Msg("There was an error!"); */
		// GFX
		{
		  SDL_Rect GFX_box_crap;
		  GFX_box_crap.x = x1 + playl;
		  GFX_box_crap.y = y1;
		  GFX_box_crap.w = 1;
		  GFX_box_crap.h = 1;
		  SDL_FillRect(GFX_lpDDSBack, &GFX_box_crap, 20);
		}
	      }
	    else
	      {
		//draw a little pixel
/* 		ddbltfx.dwFillColor = 23; */
/* 		ddbltfx.dwSize = sizeof(ddbltfx); */
/* 		box_crap.top = y1; */
/* 		box_crap.bottom = y1+1; */
/* 		box_crap.left = x1+playl; //20 is to compensate for the border */
/* 		box_crap.right = x1+1+playl; */
/* 		ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 		if (ddrval != DD_OK) Msg("There was an error!"); */
		// GFX
		{
		  SDL_Rect GFX_box_crap;
		  GFX_box_crap.x = x1 + playl;
		  GFX_box_crap.y = y1;
		  GFX_box_crap.w = 1;
		  GFX_box_crap.h = 1;
		  SDL_FillRect(GFX_lpDDSBack, &GFX_box_crap, 23);
		}
	      }
	  }
      }
}


void fix_dead_sprites( void )
{
  int i;
  if (dinkedit) return;

  for (i = 1; i < 100; i++)
    {
      if (play.spmap[*pmap].type[i] == 6)
	{

                        if  ((thisTickCount > (play.spmap[*pmap].last_time +  300000)) ||
                                (thisTickCount  +400000 < play.spmap[*pmap].last_time +  300000) )


                        {
                                //this sprite can come back online now
                                play.spmap[*pmap].type[i] = 0;
                        }
                }

                if (play.spmap[*pmap].type[i] == 7)
                {

                        if (thisTickCount > (play.spmap[*pmap].last_time +  180000))
                        {
                                //this sprite can come back online now
                                play.spmap[*pmap].type[i] = 0;
                        }
                }

                if (play.spmap[*pmap].type[i] == 8)
                {

                        if (thisTickCount > (play.spmap[*pmap].last_time +  60000))
                        {
                                //this sprite can come back online now
                                play.spmap[*pmap].type[i] = 0;
                        }
                }


    }
}


void load_map(const int num)
{
        FILE *          fp;
        long holdme,lsize;
        //RECT box;
        // play.map = num;
        //Msg("Loading map %d...",num);

	fp = fopen(ciconvert(current_map), "rb");
                         if (!fp)
                         {
                                 Msg("Cannot find %s file!!!",current_map);
                                 return;
                         }
                         lsize = sizeof(struct small_map);
                         holdme = (lsize * (num-1));
                         fseek( fp, holdme, SEEK_SET);
                         //Msg("Trying to read %d bytes with offset of %d",lsize,holdme);
                         int shit = fread( &pam, lsize, 1, fp);       /* current player */
                         //              Msg("Read %d bytes.",shit);
                         if (shit == 0) Msg("ERROR:  Couldn't read map %d?!?", num);
                         fclose(fp);

                         spr[1].move_active = /*false*/0;
                         spr[1].freeze = /*false*/0;
                         screenlock = 0;
                         fill_whole_hard();
             fix_dead_sprites();


                         if (!dinkedit)
                                 check_midi();

                         //   draw_map_game();



}

void save_map(const int num)
{
  FILE *          fp;
  long holdme,lsize;
  char crap[80];


  Msg("Saving map data..");
  strcpy(crap, current_map);
  if (num > 0)
    {
      fp = fopen(ciconvert(crap), "r+b");
      lsize = sizeof(struct small_map);
      holdme = (lsize * (num-1));
      fseek( fp, holdme, SEEK_SET);
      fwrite( &pam, lsize, 1, fp);       /* current player */
      fclose(fp);
    }

  Msg("Done saving map data..");
}




void save_info(void)
{
  FILE *          fp;
  char crap[80];
  sprintf(crap, "DINK.DAT");
  fp = fopen(ciconvert(crap), "wb");
  fwrite(&map,sizeof(struct map_info),1,fp);
  fclose(fp);
}

void save_game(int num)
{
        FILE *          fp;
        char crap[80];

        //lets set some vars first

        play.x = spr[1].x;
        play.y = spr[1].y;
        play.version = dversion;
        play.pseq =  spr[1].pseq;
        play.pframe     =        spr[1].pframe;
        play.seq        =        spr[1].seq;
        play.frame      =        spr[1].frame;
        play.size       =        spr[1].size;
        play.dir        =        spr[1].dir;
        play.strength = spr[1].strength;
        play.defense  =  spr[1].defense;
        play.que  =  spr[1].que;
        time_t ct;

        time(&ct);
        play.minutes += (int) (difftime(ct,time_start) / 60);
        //reset timer
        time(&time_start);

        play.base_idle = spr[1].base_idle;
        play.base_walk = spr[1].base_walk;
        play.base_hit = spr[1].base_hit;

                                sprintf(play.gameinfo, "Level %d",*plevel);


                                last_saved_game = num;
                                sprintf(crap, "SAVE%d.DAT", num);
                                fp = fopen(ciconvert(crap), "wb");
                                fwrite(&play,sizeof(play),1,fp);
                                fclose(fp);


}


void kill_all_vars(void)
{
  memset(&play, 0, sizeof(play));
}

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



/*bool*/int add_time_to_saved_game(int num)
{
        FILE *          fp;
        char crap[80];

        sprintf(crap, "SAVE%d.DAT", num);

	fp = fopen(ciconvert(crap), "rb");
                                if (!fp)
                                {
                                        Msg("Couldn't load save game %d", num);
                                        return(/*false*/0);
                                }
                                else
                                {

                                        fread(&play,sizeof(play),1,fp);
                                        fclose(fp);


                                }


                                //great, now let's resave it with added time
                                Msg("Ok, adding time.");
                                time_t ct;

                                time(&ct);
                                play.minutes += (int) (difftime(ct,time_start) / 60);



                                sprintf(crap, "SAVE%d.DAT", num);
                                fp = fopen(ciconvert(crap), "wb");
                                if (fp)
                                {
                                        fwrite(&play,sizeof(play),1,fp);
                                        fclose(fp);
                                }
                                Msg("Wrote it.(%d of time)", play.minutes);

                                return(/*true*/1);
}




/*bool*/int load_game(int num)
{
        FILE *          fp;
        char crap[80];

        //lets get rid of our magic and weapon scripts
        if (weapon_script != 0)

        {

                if (locate(weapon_script, "DISARM"))
                {
                        run_script(weapon_script);
                } else
                {
        }

        }
        if (magic_script != 0) if (locate(magic_script, "DISARM")) run_script(magic_script);





        bow.active = /*false*/0;
    weapon_script = 0;
        magic_script = 0;
        midi_active = /*true*/1;


        if (last_saved_game > 0)
        {
                Msg("Modifying saved game.");

                if (!add_time_to_saved_game(last_saved_game))
                        Msg("Error modifying saved game.");
        }
        StopMidi();

        sprintf(crap, "SAVE%d.DAT", num);



	fp = fopen(ciconvert(crap), "rb");
                                if (!fp)
                                {
                                        Msg("Couldn't load save game %d", num);
                                        return(/*false*/0);
                                }
                                else
                                {

                                        fread(&play,sizeof(play),1,fp);
                                        fclose(fp);

                                        spr[1].damage = 0;
                                        spr[1].x = play.x;
                                        spr[1].y = play.y;
                                        walk_off_screen = 0;
                                        spr[1].nodraw = 0;
                    push_active = 1;
                                        spr[1].pseq = play.pseq;
                                        spr[1].pframe = play.pframe;
                                        spr[1].size = play.size;
                                        spr[1].seq = play.seq;
                                        spr[1].frame = play.frame;
                                        spr[1].dir = play.dir;
                                        spr[1].strength = play.strength;
                                        spr[1].defense = play.defense;
                                        spr[1].que = play.que;

                                        time(&time_start);

                                        spr[1].base_idle = play.base_idle;
                                        spr[1].base_walk = play.base_walk;
                                        spr[1].base_hit = play.base_hit;

                                        int script = load_script("main", 0, /*true*/1);
                                        locate(script, "main");
                                        run_script(script);
                                        //lets attach our vars to the scripts

                                        attach();
                                        Msg("Attached vars.");



                                        if (*pcur_weapon != 0)
                                        {
                                                if (play.item[*pcur_weapon].active == /*false*/0)
                                                {
                                                        *pcur_weapon = 1;
                                                        weapon_script = 0;
                                                        Msg("Loadgame error: Player doesn't have armed weapon - changed to 1.");
                                                } else
                                                {

                                                        weapon_script = load_script(play.item[*pcur_weapon].name, 1000, /*false*/0);


                                                        if (locate(weapon_script, "DISARM")) run_script(weapon_script);

                                                        weapon_script = load_script(play.item[*pcur_weapon].name, 1000, /*false*/0);


                                                        if (locate(weapon_script, "ARM")) run_script(weapon_script);
                                                }
                                        }
                                        if (*pcur_magic != 0)
                                        {
                                                if (play.item[*pcur_magic].active == /*false*/0)
                                                {
                                                        *pcur_magic = 0;
                                                        magic_script = 0;
                                                        Msg("Loadgame error: Player doesn't have armed magic - changed to 0.");
                                                } else
                                                {

                                                        magic_script = load_script(play.mitem[*pcur_magic].name, 1000, /*false*/0);
                                                        if (locate(magic_script, "DISARM")) run_script(magic_script);
                                                        magic_script = load_script(play.mitem[*pcur_magic].name, 1000, /*false*/0);
                                                        if (locate(magic_script, "ARM")) run_script(magic_script);
                                                }
                                        }
                                        kill_repeat_sounds_all();
                                        load_map(map.loc[*pmap]);
                                        Msg("Loaded map.");
                                        draw_map_game();
                                        Msg("Map drawn.");

                                        last_saved_game = num;

                                        return(/*true*/1);
                                }

}

void kill_cur_item( void )
{

        if (*pcur_weapon != 0)
        {
                if (play.item[*pcur_weapon].active == /*true*/1)
                {

                        if (weapon_script != 0) if (locate(weapon_script, "DISARM")) run_script(weapon_script);
                        weapon_script = load_script(play.item[*pcur_weapon].name, 0, /*false*/0);
                        play.item[*pcur_weapon].active = /*false*/0;
                        *pcur_weapon = 0;
                        if (weapon_script != 0) if (locate(weapon_script, "HOLDINGDROP")) run_script(weapon_script);

                        if (weapon_script != 0) if (locate(weapon_script, "DROP")) run_script(weapon_script);
                        weapon_script = 0;
                } else
                {
                        Msg("Error:  Can't kill cur item, none armed.");
                }
        }
}



void kill_cur_item_script( char name[20])
{
  int select = 0;
  int i;
  for (i = 1; i < 17; i++)
    {
      if (play.item[i].active)
	if (compare(play.item[i].name, name))
	  {
	    select = i;
	    goto found;
	  }
    }
  return;

 found:

        if (*pcur_weapon == select)

        {
                //holding it right now
                if (locate(weapon_script, "HOLDINGDROP")) run_script(weapon_script);
                if (locate(weapon_script, "DISARM")) run_script(weapon_script);


                *pcur_weapon = 0;
                weapon_script = 0;
        }

        int script = load_script(play.item[select].name, 0, /*false*/0);
        play.item[select].active = /*false*/0;



        if (locate(script, "DROP")) run_script(script);

        draw_status_all();

}


void kill_cur_magic_script( char name[20])
{
  int select = 0;
  int i;
  for (i = 1; i < 9; i++)
    {
      if (play.mitem[i].active)
	if (compare(play.mitem[i].name, name))
	  {
	    select = i;
	    goto found;
	  }
    }
  return;

found:

        if (*pcur_magic == select)

        {
                //holding it right now
                if (locate(magic_script, "HOLDINGDROP")) run_script(magic_script);
                if (locate(magic_script, "DISARM")) run_script(magic_script);


                *pcur_weapon = 0;
                magic_script = 0;
        }

        int script = load_script(play.mitem[select].name, 0, /*false*/0);
        play.mitem[select].active = /*false*/0;


        if (locate(script, "DROP")) run_script(script);

        draw_status_all();

}




void kill_cur_magic( void )
{

        if (*pcur_magic != 0)
        {
                if (play.mitem[*pcur_magic].active == /*true*/1)
                {

                        if (magic_script != 0) if (locate(magic_script, "DISARM")) run_script(magic_script);
                        magic_script = load_script(play.mitem[*pcur_magic].name, 0, /*false*/0);
                        play.mitem[*pcur_magic].active = /*false*/0;
                        *pcur_magic = 0;

                        if (magic_script != 0) if (locate(magic_script, "HOLDINGDROP")) run_script(magic_script);
                        if (magic_script != 0) if (locate(magic_script, "DROP")) run_script(magic_script);
                        magic_script = 0;
                } else
                {
                        Msg("Error:  Can't kill cur magic, none armed.");
                }
        }
}


void update_screen_time(void )
{
        //Msg("Cur time is %d", play.spmap[*pmap].last_time);
        //Msg("Map is %d..", *pmap);
        play.spmap[*pmap].last_time = thisTickCount;
        //Msg("Time was saved as %d", play.spmap[*pmap].last_time);
}

/*bool*/int load_game_small(int num, char * line, int *mytime)
{
        FILE *          fp;
        char crap[80];
        sprintf(crap, "SAVE%d.DAT", num);


	fp = fopen(ciconvert(crap), "rb");
                                if (!fp)
                                {
                                        Msg("Couldn't quickload save game %d", num);
                                        return(/*false*/0);
                                }
                                else
                                {

                                        fread(&short_play,sizeof(struct player_short_info),1,fp);
                                        fclose(fp);
                                        *mytime = short_play.minutes;
                                        strcpy(line, short_play.gameinfo);
                                        return(/*true*/1);
                                }

}


void load_info(void)
{
        FILE *          fp;
        char crap[80];
        sprintf(crap, "DINK.DAT");


	fp = fopen(ciconvert(crap), "rb");
                                if (!fp)
                                {
                                        //fclose(fp);
				  fp = fopen(ciconvert(crap), "wb");
                                        //make new data file
                                        strcpy(map.name, "Smallwood");
                                        fwrite(&map,sizeof(struct map_info),1,fp);
                                        fclose(fp);
                                }
                                else
                                {
                                        Msg("World data loaded.");
                                        fread(&map,sizeof(struct map_info),1,fp);
                                        fclose(fp);

                                }

}

void save_hard(void)
{
        FILE *          fp;
        char crap[80];
        sprintf(crap, "HARD.DAT");
	fp = fopen(ciconvert(crap), "wb");

                                if (!fp)
                                {

                                        Msg("Couldn't save hard.dat for some reason.  Out of disk space?");
                                }
                                fwrite(&hmap,sizeof(struct hardness),1,fp);
                                fclose(fp);
}


void load_hard(void)
{
        FILE *          fp;
        char crap[80];
        sprintf(crap, "HARD.DAT");
        if (!dinkedit)
        {
                if (!exist(crap)) sprintf(crap, "..\\dink\\hard.dat");
        }

	fp = fopen(ciconvert(crap), "rb");
                                if (!fp)
                                {
                                        //fclose(fp);
				  fp = fopen(ciconvert(crap), "wb");
                                        //make new data file
                                        memset(&hmap, 0, sizeof(struct hardness));
                                        fwrite(&hmap,sizeof(struct hardness),1,fp);
                                        fclose(fp);
                                }
                                else
                                {
                                        fread(&hmap,sizeof(struct hardness),1,fp);
                                        fclose(fp);

                                }

}

/* Display a flashing "Please Wait" anim directly on the screen, just
   before switching to a screen that requires loading new graphics
   from the disk. */
void draw_wait()
{
  if (seq[423].frame[8] != 0)
    {

      if (please_wait)
	{
/*                         ddrval = lpDDSPrimary->BltFast(232, 0, k[seq[423].frame[7]].k, */
/*                                 &k[seq[423].frame[7]].box, DDBLTFAST_SRCCOLORKEY); */
	  // GFX
	  {
	    SDL_Rect dst = {232, 0, GFX_k[seq[423].frame[7]].k->w, GFX_k[seq[423].frame[7]].k->h};
	    SDL_BlitSurface(GFX_k[seq[423].frame[7]].k, NULL, GFX_lpDDSBack, &dst);
	    SDL_UpdateRects(GFX_lpDDSBack, 1, &dst);
	  }
	  please_wait = /*false*/0;
	}
      else
	{
/*                         ddrval = lpDDSPrimary->BltFast( 232, 0, k[seq[423].frame[8]].k, */
/*                                 &k[seq[423].frame[7]].box, DDBLTFAST_SRCCOLORKEY); */
	  // GFX
	  {
	    SDL_Rect dst = {232, 0, GFX_k[seq[423].frame[8]].k->w, GFX_k[seq[423].frame[8]].k->h};
	    SDL_BlitSurface(GFX_k[seq[423].frame[8]].k, NULL, GFX_lpDDSBack, &dst);
	    SDL_UpdateRects(GFX_lpDDSBack, 1, &dst);
	  }
	  please_wait = /*true*/1;

	}
    }
}



void load_sprite_pak(char org[100], int nummy, int speed, int xoffset, int yoffset,
			 rect hardbox, /*bool*/int notanim, /*bool*/int black, /*bool*/int leftalign, /*bool*/int samedir)
{
  int work;

  HFASTFILE                  pfile;
/*   BITMAPFILEHEADER UNALIGNED *pbf; */
/*   BITMAPINFOHEADER UNALIGNED *pbi; */
/*   DDSURFACEDESC       ddsd; */
/*   BITMAP              bm; */

/*   DDCOLORKEY          ddck; */

/*   int x,y,dib_pitch; */
/*   unsigned char *src, *dst; */
  char fname[20];

  //IDirectDrawSurface *pdds;

  int sprite = 71;
/*   /\*BOOL*\/int trans = /\*FALSE*\/0; */
  /*bool*/int reload = /*false*/0;

  char crap[200];

  int save_cur = cur_sprite;

  int oo;

  if (s_index[nummy].last != 0)
    {
      //  Msg("Saving sprite %d", save_cur);
      cur_sprite = s_index[nummy].s+1;
      //Msg("Temp cur_sprite is %d", cur_sprite);
      reload = /*true*/1;
    }


  s_index[nummy].s = cur_sprite -1;

  if (no_running_main) draw_wait();

  char crap2[200];
  strcpy(crap2, org);
  while(crap2[strlen(crap2)-1] != '\\')
    {
      crap2[strlen(crap2)-1] = 0;
    }
  crap2[strlen(crap2)-1] = 0;

  int num = strlen(org) - strlen(crap2)-1;
  strcpy(fname, &org[strlen(org)-num]);
  if (samedir)
    sprintf(crap, "%s/dir.ff", crap2);
  else
    sprintf(crap, "../dink/%s/dir.ff", crap2);


  if (!FastFileInit(ciconvert(crap), 5))
    {
      Msg( "Could not load dir.ff art file %s" , crap);

      cur_sprite = save_cur;
      return;
    }

  // No color conversion for sprite paks - they need to use the Dink
  // Palette, otherwise weird colors will appear!
  /*           if (!windowed)
	       {
	       lpDDPal->GetEntries(0,0,256,holdpal);
	       lpDDPal->SetEntries(0,0,256,real_pal);
	       }
  */

  for (oo = 1; oo <= 51; oo++)
    {
      //load sprite
      sprite = cur_sprite;
      //if (reload) Msg("Ok, programming sprite %d", sprite);
      if (oo < 10) strcpy(crap2, "0"); else strcpy(crap2, "");
      sprintf(crap, "%s%s%d.bmp", fname, crap2, oo);

      pfile = FastFileOpen(crap);

      if (pfile == NULL)
	{
	  FastFileClose( pfile );
	  //   FastFileFini();
	  if (oo == 1)
	    Msg("Sprite_load_pak error:  Couldn't load %s.",crap);

	  s_index[nummy].last = (oo - 1);
	  //      initFail(hWndMain, crap);
	  setup_anim(nummy,nummy,speed);
	  //                           if (!windowed)  lpDDPal->SetEntries(0,0,256,holdpal);

	  //if (reload) Msg("Ok, tacking %d back on.", save_cur);
	  cur_sprite = save_cur;
	  return;
	}
      else
	{
	  //got file
/* 	  pbf = (BITMAPFILEHEADER *)FastFileLock(pfile, 0, 0); */
/* 	  pbi = (BITMAPINFOHEADER *)(pbf+1); */

/* 	  if (pbf->bfType != 0x4d42 || */
/* 	      pbi->biSize != sizeof(BITMAPINFOHEADER)) */
/* 	    { */
/* 	      Msg("Failed to load"); */
/* 	      Msg(crap); */
/* 	      cur_sprite = save_cur; */
/* 	      FastFileClose( pfile ); */
/* 	      //   FastFileFini(); */

/* 	      return; */
/* 	    } */

/* 	  byte *pic; */

/* 	  pic = (byte *)pbf + 1078; */

	  //Msg("Pic's size is now %d.",sizeof(pic));

/* 	  bm.bmWidth = pbi->biWidth; */
/* 	  bm.bmHeight = pbi->biHeight; */
/*  	  bm.bmWidthBytes = 32; */
/* 	  bm.bmPlanes = pbi->biPlanes; */
/* 	  bm.bmBitsPixel = pbi->biBitCount; */
/* 	  bm.bmBits = pic; */

	  //
	  // create a DirectDrawSurface for this bitmap
	  //
/* 	  ZeroMemory(&ddsd, sizeof(ddsd)); */
/* 	  ddsd.dwSize = sizeof(ddsd); */
/* 	  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH; */
/* 	  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY; */
/* 	  ddsd.dwWidth = pbi->biWidth; */
/* 	  ddsd.dwHeight = pbi->biHeight; */

	  if (GFX_k[sprite].k != NULL)
	    {
/* 	      k[sprite].k->Release(); */
	      SDL_FreeSurface(GFX_k[sprite].k);
	    }

/* 	  if (lpDD->CreateSurface(&ddsd, &k[sprite].k, NULL) != DD_OK) */
/* 	    { */
/* 	      Msg("Failed to create pdd surface description"); */
/* 	    } */
/* 	  else */
/* 	    { */
/* 	      ddsd.dwSize = sizeof(ddsd); */
/* 	      ddrval = IDirectDrawSurface_Lock(k[sprite].k, NULL, &ddsd, DDLOCK_WAIT, NULL); */

/* 	      if( ddrval == DD_OK ) */
/* 		{ */
/* 		  dib_pitch = (pbi->biWidth+3)&~3; */
/* 		  src = (BYTE *)pic + dib_pitch * (pbi->biHeight-1); */
/* 		  dst = (BYTE *)ddsd.lpSurface; */
/* 		  if (leftalign) */
/* 		    { */
/* 		      //Msg("left aligning.."); */

/* 		      for( y=0; y<(int)pbi->biHeight; y++ ) */
/* 			{ */
/* 			  for( x=0; x<(int)pbi->biWidth; x++ ) */
/* 			    { */
/* 			      dst[x] = src[x]; */
/* 			      if (dst[x] == 0) */
/* 				{ */
/* 				  // Msg("Found a 255..."); */
/* 				  dst[x] = 30; */
/* 				} else */
/* 				if (dst[x] == 255) */
/* 				  { */
/* 				    dst[x] = 249; */
/* 				  } */
/* 			    } */
/* 			  dst += ddsd.lPitch; */
/* 			  src -= dib_pitch; */
/* 			} */
/* 		    } */
/* 		  else if (black) */
/* 		    { */
/* 		      for( y=0; y<(int)pbi->biHeight; y++ ) */
/* 			{ */
/* 			  for( x=0; x<(int)pbi->biWidth; x++ ) */
/* 			    { */
/* 			      dst[x] = src[x]; */

/* 			      if (dst[x] == 0) */
/* 				{ */
/* 				  dst[x] = 30; */
/* 				} */
/* 			    } */
/* 			  dst += ddsd.lPitch; */
/* 			  src -= dib_pitch; */
/* 			} */
/* 		    } */
/* 		  else */
/* 		    { */
/* 		      //doing white */
/* 		      for( y=0; y<(int)pbi->biHeight; y++ ) */
/* 			{ */
/* 			  for( x=0; x<(int)pbi->biWidth; x++ ) */
/* 			    { */
/* 			      dst[x] = src[x]; */

/* 			      if (dst[x] == 255) */
/* 				{ */
/* 				  // Msg("Found a 255..."); */
/* 				  dst[x] = 249; */
/* 				} */
/* 			    } */
/* 			  dst += ddsd.lPitch; */
/* 			  src -= dib_pitch; */
/* 			} */
/* 		    } */

/* 		  IDirectDrawSurface_Unlock(k[sprite].k, NULL); */


	  // GFX
	  /* TODO: perform the same manual palette conversion
	     like above? */
	  {
	    Uint8 *buffer;
	    SDL_RWops *rw;

	    buffer = (Uint8 *) FastFileLock (pfile, 0, 0);
	    rw = SDL_RWFromMem (buffer, FastFileLen (pfile));

	    GFX_k[sprite].k = load_bmp_from_mem(rw);
	    // bmp_surf = IMG_Load_RW (rw, 0);
	    if (GFX_k[sprite].k == NULL)
	      {
		Msg("unable to load %s from fastfile", crap);
	      }

	    SDL_FreeRW (rw);

	    if (leftalign)
	      ; // ?
	    else if (black)

	      /* TODO: use SDL_RLEACCEL? "RLE acceleration can
		 substantially speed up blitting of images with large
		 horizontal runs of transparent pixels" (man
		 SDL_SetColorKey) */
	      /* We might want to directly use the hard-coded
		 '0' index for efficiency */
	      SDL_SetColorKey(GFX_k[sprite].k, SDL_SRCCOLORKEY,
			      SDL_MapRGB(GFX_k[sprite].k->format, 0, 0, 0));
	    else
	      /* We might want to directly use the hard-coded
		 '255' index for efficiency */
	      SDL_SetColorKey(GFX_k[sprite].k, SDL_SRCCOLORKEY,
			      SDL_MapRGB(GFX_k[sprite].k->format, 255, 255, 255));
	  }
/* 		} */
/* 	      else */
/* 		{ */
/* 		  Msg("Lock failed err=%d", ddrval); */
/* 		  //return; */
/* 		} */

	  if (sprite > 0)
	    {
	      k[sprite].box.top = 0;
	      k[sprite].box.left = 0;
	      k[sprite].box.right = GFX_k[sprite].k->w;
	      k[sprite].box.bottom = GFX_k[sprite].k->h;

	      if ( (oo > 1) & (notanim) )
		{
		  k[cur_sprite].yoffset = k[s_index[nummy].s+1].yoffset;
		}
	      else
		{
		  if (yoffset > 0)
		    k[cur_sprite].yoffset = yoffset; else
		    {
		      k[cur_sprite].yoffset = (k[cur_sprite].box.bottom -
					       (k[cur_sprite].box.bottom / 4)) - (k[cur_sprite].box.bottom / 30);
		    }
		}

	      if ( (oo > 1 ) & (notanim))
		{
		  k[cur_sprite].xoffset =  k[s_index[nummy].s+1].xoffset;
		    }
	      else
		{
		  if (xoffset > 0)
		    k[cur_sprite].xoffset = xoffset;
		  else
		    {
		      k[cur_sprite].xoffset = (k[cur_sprite].box.right -
					       (k[cur_sprite].box.right / 2)) + (k[cur_sprite].box.right / 6);
		    }
		}
	      //ok, setup main offsets, lets build the hard block

	      if (hardbox.right > 0)
		{
		  //forced setting
		  k[cur_sprite].hardbox.left = hardbox.left;
		  k[cur_sprite].hardbox.right = hardbox.right;
		}
	      else
		{
		  //guess setting
		  work = k[cur_sprite].box.right / 4;
		  k[cur_sprite].hardbox.left -= work;
		  k[cur_sprite].hardbox.right += work;
		}

	      if (hardbox.bottom > 0)
		{
		  k[cur_sprite].hardbox.top = hardbox.top;
		  k[cur_sprite].hardbox.bottom = hardbox.bottom;
		}
	      else
		{
		  work = k[cur_sprite].box.bottom / 10;
		  k[cur_sprite].hardbox.top -= work;
		  k[cur_sprite].hardbox.bottom += work;
		}

	      if (black)
		{
/* 		      ddck.dwColorSpaceLowValue  = DDColorMatch(k[cur_sprite].k, RGB(255,255,255)); */

/* 		      ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue; */
/* 		      k[cur_sprite].k->SetColorKey(DDCKEY_SRCBLT, &ddck); */
		}
	      else
		{
/* 		      ddck.dwColorSpaceLowValue  = DDColorMatch(k[cur_sprite].k, RGB(0,0,0)); */
/* 		      ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue; */
/* 		      k[cur_sprite].k->SetColorKey(DDCKEY_SRCBLT, &ddck); */
		    }
	      cur_sprite++;
	      if (!reload)
		save_cur++;
	      FastFileClose( pfile );
	    }
	}
    }
  // FastFileFini();
  return;
}


/* Load sprite, either from a dir.ff pack (delegated to
   load_sprite_pak), either from a BMP file */
/* - org: path to the file, relative to the current game (dink or dmod) */
/* - nummy: ??? */
void load_sprites(char org[100], int nummy, int speed, int xoffset, int yoffset,
		  rect hardbox, /*bool*/int notanim, /*bool*/int black, /*bool*/int leftalign)
{
  int work;
/*   PALETTEENTRY holdpal[256]; */
  char crap[200],hold[5];
  int oo;

  if (no_running_main) draw_wait();

  /* dirname(): */
  // PORT: either use dirname(), or allow '/' as an alternative to
  // '\\'
  char crap2[200];
  strcpy(crap2, org);
  while(crap2[strlen(crap2)-1] != '\\')
    {
      crap2[strlen(crap2)-1] = 0;
    }
  crap2[strlen(crap2)-1] = 0;

  /* Order: */
  /* - dmod/.../dir.ff */
  /* - dmod/.../...01.BMP */
  /* - ../dink/.../dir.ff */
  /* - ../dink/.../...01.BMP */
  sprintf(crap, "%s\\dir.ff",crap2);
  //Msg("Checking for %s..", crap);
  if (exist(ciconvert(crap)))
    {
      load_sprite_pak(org, nummy, speed, xoffset, yoffset, hardbox, notanim, black, leftalign, /*true*/1);
      return;
    }
  sprintf(crap, "%s01.BMP",org);
  if (!exist(ciconvert(crap)))
    {
      sprintf(crap, "..\\dink\\%s\\dir.ff",crap2);
      //Msg("Checking for %s..", crap);
      if (exist(ciconvert(crap)))
	{
	  load_sprite_pak(org, nummy, speed, xoffset, yoffset, hardbox, notanim, black, leftalign, /*false*/0);
	  return;
	}
      //    Msg("Dir bad for sprite, changing");
      sprintf(crap, "..\\dink\\%s",org);
      strcpy(org,crap);
    }
  s_index[nummy].s = cur_sprite -1;

  /* Possibly used to temporarily use the reference palette even if
     the screen palette was changed. */
/*   if (!windowed) */
/*     { */
/*       lpDDPal->GetEntries(0,0,256,holdpal); */
/*       lpDDPal->SetEntries(0,0,256,real_pal); */
/*     } */

  /* Load the whole sequence (prefix-01.bmp, prefix-02.bmp, ...) */
  for (oo = 1; oo <= 1000; oo++)
    {
      if (oo < 10) strcpy(hold, "0"); else strcpy(hold,"");
      sprintf(crap, "%s%s%d.BMP",org,hold,oo);

      /* Set the pixel data */
/*       k[cur_sprite].k = DDSethLoad(lpDD, crap, 0, 0, cur_sprite); */
      // GFX
      GFX_k[cur_sprite].k = load_bmp(crap);
      if (GFX_k[cur_sprite].k == NULL && oo == 1)
	{
	  /* First frame didn't load! */
	  /* It's normal if we're at the end of a sequence */
	  fprintf(stderr, "load_sprites: couldn't open %s: %s\n", crap, SDL_GetError());
	}

      /* Define the offsets / center of the image */
      if (GFX_k[cur_sprite].k != NULL)
	{
	  k[cur_sprite].box.top = 0;
	  k[cur_sprite].box.left = 0;
	  k[cur_sprite].box.right = GFX_k[cur_sprite].k->w;
	  k[cur_sprite].box.bottom = GFX_k[cur_sprite].k->h;
	  if ((oo > 1) & (notanim))
	    {
	      k[cur_sprite].yoffset = k[s_index[nummy].s+1].yoffset;
	    }
	  else
	    {
	      if (yoffset > 0)
		k[cur_sprite].yoffset = yoffset;
	      else
		{
		  k[cur_sprite].yoffset = (k[cur_sprite].box.bottom -
					   (k[cur_sprite].box.bottom / 4)) - (k[cur_sprite].box.bottom / 30);
		}
	    }

	  if ((oo > 1) & (notanim))
	    {
	      k[cur_sprite].xoffset = k[s_index[nummy].s+1].xoffset;
	    }
	  else
	    {
	      if (xoffset > 0)
		k[cur_sprite].xoffset = xoffset; else
		{
		  k[cur_sprite].xoffset = (k[cur_sprite].box.right -
					   (k[cur_sprite].box.right / 2)) + (k[cur_sprite].box.right / 6);
		}
	    }
	  //ok, setup main offsets, lets build the hard block

	  if (hardbox.right > 0)
	    {
	      //forced setting
	      k[cur_sprite].hardbox.left = hardbox.left;
	      k[cur_sprite].hardbox.right = hardbox.right;
	    }
	  else
	    {
	      //guess setting
	      work = k[cur_sprite].box.right / 4;
	      k[cur_sprite].hardbox.left -= work;
	      k[cur_sprite].hardbox.right += work;
	    }

	  if (hardbox.bottom > 0)
	    {
	      //forced setting
	      k[cur_sprite].hardbox.top = hardbox.top;
	      k[cur_sprite].hardbox.bottom = hardbox.bottom;
	    }
	  else
	    {
	      //guess setting
	      /* eg: graphics\dink\push\ds-p2- and
		 graphics\effects\comets\sm-comt2\fbal2- */
	      work = k[cur_sprite].box.bottom / 10;
	      k[cur_sprite].hardbox.top -= work;
	      k[cur_sprite].hardbox.bottom += work;
	    }
	}

      if (leftalign)
	{
	  //     k[cur_sprite].xoffset = 0;
	  //     k[cur_sprite].yoffset = 0;
	}

      //add_text(crap,"LOG.TXT");

      if (GFX_k[cur_sprite].k == NULL)
	{
	  /* oo == 1 => not even one sprite was loaded, error */
	  /* oo > 1 => the sequence ends */

	  if (oo < 2)
	    {
	      Msg("load_sprites:  Anim %s not found.",org);
	    }

	  s_index[nummy].last = (oo - 1);
	  //       initFail(hWndMain, crap);
	  setup_anim(nummy,nummy,speed);

	  /* Restore screen palette to what it was */
/* 	  if (!windowed) */
/* 	    lpDDPal->SetEntries(0,0,256,holdpal); */

	  return;
	}

      //if (show_dot) Msg( "%s", crap);

      /* Set transparent color: either black or white */
      if (black)
	{
/* 	  DDSetColorKey(k[cur_sprite].k, RGB(0,0,0)); */
	  // GFX
	  SDL_SetColorKey(GFX_k[cur_sprite].k, SDL_SRCCOLORKEY,
			  SDL_MapRGB(GFX_k[cur_sprite].k->format, 0, 0, 0));
	}
      else
	{
/* 	  DDSetColorKey(k[cur_sprite].k, RGB(255,255,255)); */
	  // GFX
	  SDL_SetColorKey(GFX_k[cur_sprite].k, SDL_SRCCOLORKEY,
			  SDL_MapRGB(GFX_k[cur_sprite].k->format, 255, 255, 255));
	}
      cur_sprite++;

      //if (first_frame) if  (oo == 1) return;
    }
}

void figure_out(char line[255], int load_seq)
{
        char ev[15][100];
        rect hardbox;
        memset(&ev, 0, sizeof(ev));
        int myseq = 0,myframe = 0; int special = 0;
        int special2 = 0;
	int i;
        for (i = 1; i <= 14; i++)
        {
                separate_string(line, i,' ',ev[i]);
                //   Msg("Word %d is \"%s\"",i,ev[i]);
        }

        if (    (compare(ev[1],"LOAD_SEQUENCE_NOW")) | ( compare(ev[1],"LOAD_SEQUENCE"))  )
                //      if (     (load_seq == -1) | (load_seq == atol(ev[3]))  )
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy

	  memset(&hardbox, 0, sizeof(rect));
                seq[atol(ev[3])].active = /*true*/1;
		if (seq[atol(ev[3])].data != line)
		  strcpy(seq[atol(ev[3])].data, line);
                if (compare(ev[4], "BLACK"))
                {
                        load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,/*true*/1, /*true*/1, /*false*/0);
                } else
                        if (compare(ev[4], "LEFTALIGN"))
                        {
                                load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,/*false*/0, /*false*/0, /*true*/1);
                        } else

                                if (compare(ev[4], "NOTANIM"))
                                {

                                        //not an animation!
                                        load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,/*false*/0, /*false*/0, /*false*/0); //Crap
                                } else
                                {
                                        //yes, an animation!
                                        hardbox.left = atol(ev[7]);
                                        hardbox.top = atol(ev[8]);
                                        hardbox.right = atol(ev[9]);
                                        hardbox.bottom = atol(ev[10]);

                                        load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,/*true*/1, /*false*/0, /*false*/0); //Crap
                                }


                                program_idata();
                                return;
        }
        if (compare(ev[1],"SET_SPRITE_INFO"))
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy


                //if (k[seq[myseq].frame[myframe]].frame = 0) Msg("Changing sprite that doesn't exist...");

                myseq = atol(ev[2]);
                myframe = atol(ev[3]);
                k[seq[myseq].frame[myframe]].xoffset = atol(ev[4]);
                k[seq[myseq].frame[myframe]].yoffset = atol(ev[5]);
                k[seq[myseq].frame[myframe]].hardbox.left = atol(ev[6]);
                k[seq[myseq].frame[myframe]].hardbox.top = atol(ev[7]);
                k[seq[myseq].frame[myframe]].hardbox.right = atol(ev[8]);
                k[seq[myseq].frame[myframe]].hardbox.bottom = atol(ev[9]);
        }

        if (compare(ev[1],"SET_FRAME_SPECIAL"))
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy


                //if (k[seq[myseq].frame[myframe]].frame = 0) Msg("Changing sprite that doesn't exist...");

                myseq = atol(ev[2]);
                myframe = atol(ev[3]);
                special = atol(ev[4]);

                seq[myseq].special[myframe] = special;
                Msg("Set special.  %d %d %d",myseq, myframe, special);
        }

        if (compare(ev[1],"SET_FRAME_DELAY"))
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy


                //if (k[seq[myseq].frame[myframe]].frame = 0) Msg("Changing sprite that doesn't exist...");

                myseq = atol(ev[2]);
                myframe = atol(ev[3]);
                special = atol(ev[4]);

                seq[myseq].delay[myframe] = special;
                Msg("Set delay.  %d %d %d",myseq, myframe, special);
        }

        if (compare(ev[1],"STARTING_DINK_X"))
        {
                myseq = atol(ev[2]);
                play.x = myseq;
        }




        if (compare(ev[1],"STARTING_DINK_Y"))
        {
                myseq = atol(ev[2]);
                play.y = myseq;
        }



        if (compare(ev[1],"SET_FRAME_FRAME"))
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy


                //if (k[seq[myseq].frame[myframe]].frame = 0) Msg("Changing sprite that doesn't exist...");

                myseq = atol(ev[2]);
                myframe = atol(ev[3]);
                special = atol(ev[4]);
                special2 = atol(ev[5]);

                if (special == -1)
                        seq[myseq].frame[myframe] = special; else
                        seq[myseq].frame[myframe] = seq[special].frame[special2];
                Msg("Set frame.  %d %d %d",myseq, myframe, special);
        }

}


void program_idata(void)
{
  int i;
        for (i = 1; i < MAX_IDATA; i++)
        {
                if (id[i].type == 0) return;

                if (id[i].type == 1)
                {
                        k[seq[id[i].seq].frame[id[i].frame]].xoffset = id[i].xoffset;
                        k[seq[id[i].seq].frame[id[i].frame]].yoffset = id[i].yoffset;
                        rect_copy(&k[seq[id[i].seq].frame[id[i].frame]].hardbox, &id[i].hardbox);

                        //       Msg("Programming idata type %d in %d...Seq %d Frame %d (Hardbox is %d %d %d %d)", id[i].type, i,
                        //      id[i].seq, id[i].frame, id[i].hardbox.left,id[i].hardbox.right, id[i].hardbox.top, id[i].hardbox.bottom);

                }

                if (id[i].type == 2)
                {
                        //set special
                        seq[id[i].seq].special[id[i].frame] = id[i].xoffset;
                }
                if (id[i].type == 3)
                {
                        //set delay

                        seq[id[i].seq].delay[id[i].frame] = id[i].xoffset;
                }

                if (id[i].type == 4)
                {


                        if (id[i].xoffset == -1)
                                seq[id[i].seq].frame[id[i].frame] = id[i].xoffset; else
                                seq[id[i].seq].frame[id[i].frame] = seq[id[i].xoffset].frame[id[i].yoffset];

                }

        }



}

void make_idata(int type, int myseq, int myframe, int xoffset, int yoffset, rect crect)
{
  int i;
        for (i = 1; i < MAX_IDATA; i++)
        {
                if (id[i].type == 0)
                {
                        //  Msg("Loading idata type %d in %d...", type, i);
                        //found empty one
                        id[i].type = type;
                        id[i].seq = myseq;
                        id[i].frame = myframe;
                        id[i].xoffset = xoffset;
                        id[i].yoffset = yoffset;
                        rect_copy(&id[i].hardbox, &crect);

                        return;
                }

        }

        Msg("Out of idata spots (max is %d), no more sprite corrections can be allowed.", MAX_IDATA);
}

void pre_figure_out(char line[255], int load_seq)
{
        char ev[15][100];
        rect hardbox;
        memset(&ev, 0, sizeof(ev));
        int myseq = 0,myframe = 0; int special = 0;
        int special2 = 0;
	int i;
        for (i=1; i <= 14; i++)
        {
                separate_string(line, i,' ',ev[i]);
                //   Msg("Word %d is \"%s\"",i,ev[i]);
        }


        if (compare(ev[1],"playmidi"))
        {
                if (!dinkedit)
                        PlayMidi(ev[2]);
        }

        if ( compare(ev[1],"LOAD_SEQUENCE_NOW") )
                //      if (     (load_seq == -1) | (load_seq == atol(ev[3]))  )
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy

	  memset(&hardbox, 0, sizeof(rect));
                seq[atol(ev[3])].active = /*true*/1;
                strcpy(seq[atol(ev[3])].data, line);
                if (compare(ev[4], "BLACK"))
                {
                        load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,/*true*/1, /*true*/1, /*false*/0);
                } else
                        if (compare(ev[4], "LEFTALIGN"))
                        {
                                load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,/*false*/0, /*false*/0, /*true*/1);
                        } else

                                if (compare(ev[4], "NOTANIM"))
                                {

                                        //not an animation!
                                        load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,/*false*/0, /*false*/0, /*false*/0); //Crap
                                } else
                                {
                                        //yes, an animation!
                                        hardbox.left = atol(ev[7]);
                                        hardbox.top = atol(ev[8]);
                                        hardbox.right = atol(ev[9]);
                                        hardbox.bottom = atol(ev[10]);

                                        load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,/*true*/1, /*false*/0, /*false*/0); //Crap
                                }


                                myseq = atol(ev[2]);
                                myframe = atol(ev[3]);
                                rect_set(&hardbox, atol(ev[6]), atol(ev[7]), atol(ev[8]), atol(ev[9]));
                                make_idata(1, myseq, myframe,atol(ev[4]), atol(ev[5]),hardbox);

                                //    program_idata();
                                return;
        }
        /*
        if (compare(ev[1],"LOAD_SEQUENCE_NOW"))
        //      if (     (load_seq == -1) | (load_seq == atol(ev[3]))  )
        {
        //           name   seq    speed       offsetx     offsety       hardx      hardy

          ZeroMemory(&hardbox, sizeof(RECT));
          strcpy(seq[atol(ev[3])].data, line);
          seq[atol(ev[3])].active = true;


                  if (compare(ev[4], "NOTANIM"))
                  {

                        //not an animation!
                        load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,false, false); //Crap
                        } else
                        {
                        //yes, an animation!
                        hardbox.left = atol(ev[7]);
                        hardbox.top = atol(ev[8]);
                        hardbox.right = atol(ev[9]);
                        hardbox.bottom = atol(ev[10]);

                          load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]), hardbox,true, false); //Crap
                          }


                                  //program_idata();
                                  return;
                                  }

        */
        if (compare(ev[1],"LOAD_SEQUENCE"))

        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy
                strcpy(seq[atol(ev[3])].data, line);
                seq[atol(ev[3])].active = /*true*/1;
                return;
        }

        if (compare(ev[1],"SET_SPRITE_INFO"))
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy


                //if (k[seq[myseq].frame[myframe]].frame = 0) Msg("Changing sprite that doesn't exist...");

                myseq = atol(ev[2]);
                myframe = atol(ev[3]);
                rect_set(&hardbox, atol(ev[6]), atol(ev[7]), atol(ev[8]), atol(ev[9]));
                make_idata(1, myseq, myframe,atol(ev[4]), atol(ev[5]),hardbox);
                return;
        }



        if (compare(ev[1],"SET_FRAME_SPECIAL"))
        {
                myseq = atol(ev[2]);
                myframe = atol(ev[3]);
                special = atol(ev[4]);
                make_idata(2, myseq, myframe,atol(ev[4]), 0,hardbox);
                return;
        }

        if (compare(ev[1],"SET_FRAME_DELAY"))
        {
                myseq = atol(ev[2]);
                myframe = atol(ev[3]);
                special = atol(ev[4]);
                make_idata(3, myseq, myframe,atol(ev[4]), 0,hardbox);
                return;
        }

        if (compare(ev[1],"STARTING_DINK_X"))
        {
                myseq = atol(ev[2]);
                play.x = myseq;

        }

        if (compare(ev[1],"STARTING_DINK_Y"))
        {
                myseq = atol(ev[2]);
                play.y = myseq;
        }



        if (compare(ev[1],"SET_FRAME_FRAME"))
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy


                //if (k[seq[myseq].frame[myframe]].frame = 0) Msg("Changing sprite that doesn't exist...");

                myseq = atol(ev[2]);
                myframe = atol(ev[3]);
                special = atol(ev[4]);
                special2 = atol(ev[5]);


                make_idata(4, myseq, myframe,atol(ev[4]), atol(ev[5]),hardbox);

                //       Msg("Set frame.  %d %d %d",myseq, myframe, special);

        }

}


int draw_num(int mseq, char nums[50], int mx, int my)
{
  int length = 0;
/*   HRESULT             ddrval; */
  int rnum = 0;
  int i;

  for (i=0; i < strlen(nums); i++)
    {
      if (nums[i] == '0') rnum = 10;
      else if (nums[i] == '1') rnum = 1;
      else if (nums[i] == '2') rnum = 2;
      else if (nums[i] == '3') rnum = 3;
      else if (nums[i] == '4') rnum = 4;
      else if (nums[i] == '5') rnum = 5;
      else if (nums[i] == '6') rnum = 6;
      else if (nums[i] == '7') rnum = 7;
      else if (nums[i] == '8') rnum = 8;
      else if (nums[i] == '9') rnum = 9;
      else if (nums[i] == '/') rnum = 11;
/*     again: */
      if ((rnum != 11) && (!(mseq == 442)))
	{
/* 	  ddrval = lpDDSTwo->BltFast(mx+length, my, k[seq[mseq].frame[rnum]].k, */
/* 				     &k[seq[mseq].frame[rnum]].box, DDBLTFAST_NOCOLORKEY); */
	  // GFX
	  /* TODO: we should not allow color transparency here; on the
	     other hand this doesn't seem useful; numbers are loaded
	     with LEFTALIGN, which convert black and white to the
	     nearest non-black and non-white colors from the Dink
	     Palette - hence disabling black and white
	     transparency. */
	  /* TODO: test what happens if dir.ff files are replaced by
	     .bmp's. Possibly number will carry transparency. */
	  {
	    SDL_Rect dst = {mx+length, my};
	    SDL_BlitSurface(GFX_k[seq[mseq].frame[rnum]].k, NULL, GFX_lpDDSTwo, &dst);
	  }
	}
      else
	{
/* 	  ddrval = lpDDSTwo->BltFast(mx+length, my, k[seq[mseq].frame[rnum]].k, */
/* 				     &k[seq[mseq].frame[rnum]].box, DDBLTFAST_SRCCOLORKEY); */
	  // GFX
	  /* Transparency is meaningly less here as well, at least in
	     the original game */
	  {
	    SDL_Rect dst = {mx+length, my};
	    SDL_BlitSurface(GFX_k[seq[mseq].frame[rnum]].k, NULL, GFX_lpDDSTwo, &dst);
	  }
	}


/*       if (ddrval != DD_OK) */
/* 	{ */
/* 	  if (ddrval == DDERR_WASSTILLDRAWING) goto again; */
/* 	  //dderror(ddrval); */
/* 	} */
/*       else */
/* 	{ */
	  length += k[seq[mseq].frame[rnum]].box.right;
/* 	} */
    }
  return(length);
}

int next_raise(void)
{
        int crap = *plevel;
        int num = ((100 * crap) * crap);

        if (num > 99999) num = 99999;
        return(num);

}


void draw_exp()
{
        char buffer[30];
        char nums[30];
        char final[30];

        //Msg("Drawing exp.. which is %d and %d",fexp, *pexp);
        strcpy(final, "");
	sprintf(buffer, "%d", fexp);
        strcpy(nums, buffer);
        if (strlen(nums) < 5)
	  {
	    int i;
	    for (i = 1; i < (6 - strlen(nums)); i++)
	      strcat(final, "0");
	  }
	strcat(final, nums);
	strcat(final,"/");

		sprintf(buffer, "%d", fraise);
                strcpy(nums, buffer);
                if (strlen(nums) < 5)
		  {
		    int i;
		    for (i = 1; i < (6 - strlen(nums)); i++)
		      strcat(final, "0");
		  }
		strcat(final, nums);
		draw_num(181, final, 404, 459);

}


void draw_strength()
{
        char final[30];
        char buffer[30];
        char nums[30];
        //Msg("Drawing exp.. which is %d and %d",fexp, *pexp);
        strcpy(final, "");

	sprintf(buffer, "%d", fstrength);
        strcpy(nums, buffer);
        if (strlen(nums) < 3)
	  {
	    int i;
	    for (i = 1; i < (4 - strlen(nums)); i++)
	      strcat(final, "0");
	  }
	strcat(final, nums);
	//Msg("Drawing %s..",final);
	draw_num(182, final, 81, 415);
}


void draw_defense()
{
        char final[30];
        char buffer[30];
        char nums[30];
        //Msg("Drawing exp.. which is %d and %d",fexp, *pexp);
        strcpy(final, "");
	sprintf(buffer, "%d", fdefense);
        strcpy(nums, buffer);
        if (strlen(nums) < 3)
	  {
	    int i;
	    for (i = 1; i < (4 - strlen(nums)); i++)
	      strcat(final, "0");
	  }
	strcat(final, nums);
	draw_num(183, final, 81, 437);
}


void draw_magic()
{
        char final[30];
        char buffer[30];
        char nums[30];
        //Msg("Drawing exp.. which is %d and %d",fexp, *pexp);
        strcpy(final, "");
	sprintf(buffer, "%d", fmagic);
        strcpy(nums, buffer);
        if (strlen(nums) < 3)
	  {
	    int i;
	    for (i = 1; i < (4 - strlen(nums)); i++)
	      strcat(final, "0");
	  }
	strcat(final, nums);
	draw_num(184, final, 81, 459);
}


void draw_level()
{
        char final[30];
        char buffer[30];
        //*plevel = 15;
        //Msg("Drawing level.. which is %d ",*plevel);
	sprintf(buffer, "%d", *plevel);
        strcpy(final, buffer);

        if (strlen(final) == 1)

                draw_num(442, final, 528, 456); else
                draw_num(442, final, 523, 456);

}


void draw_gold()
{
        char final[30];
        char buffer[30];
        char nums[30];
        //Msg("Drawing exp.. which is %d and %d",fexp, *pexp);
        strcpy(final, "");
	sprintf(buffer, "%d", fgold);
        strcpy(nums, buffer);
        if (strlen(nums) < 5)
	  {
	    int i;
	    for (i = 1; i < (6 - strlen(nums)); i++)
	      strcat(final, "0");
	  }
	strcat(final, nums);
	draw_num(185, final, 298, 457);
}


void draw_bar(int life, int seqman)
{
  int cur = 0;
  int curx = 284;
  int cury = 412;
  int rnum = 3;
  int curx_start = curx;

  rect box;
  while(1)
    {
      cur++;
      if (cur > life)
	{
	  cur--;
	  int rem = (cur) - (cur / 10) * 10;
	  if (rem != 0)
	    {
	      rect_copy(&box, &k[seq[seqman].frame[rnum]].box);
	      //Msg("Drawing part bar . cur is %d", rem);
	      box.right = (box.right * ((rem) * 10)/100);
	      //woah, there is part of a bar remaining.  Lets do it.
/* 	    again: */
/* 	      ddrval = lpDDSTwo->BltFast(curx, cury, k[seq[seqman].frame[rnum]].k, */
/* 					 &box, DDBLTFAST_NOCOLORKEY); */
/* 	      if (ddrval == DDERR_WASSTILLDRAWING) */
/* 		goto again; */
	      // GFX
	      {
		SDL_Rect src, dst;
		src.x = 0; src.y = 0;
		src.w = GFX_k[seq[seqman].frame[rnum]].k->w * (rem * 10) / 100;
		src.h = GFX_k[seq[seqman].frame[rnum]].k->h;
		dst.x = curx; dst.y = cury;
		SDL_BlitSurface(GFX_k[seq[seqman].frame[rnum]].k, &src, GFX_lpDDSTwo, &dst);
	      }
	    }
	  //are we done?
	  return;
	}

      rnum = 2;
      if (cur < 11) rnum = 1;
      if (cur == *plifemax) rnum = 3;

      if ((cur / 10) * 10 == cur)
	{
/* 	again2: */
/* 	  ddrval = lpDDSTwo->BltFast( curx, cury, k[seq[seqman].frame[rnum]].k, */
/* 				      &k[seq[seqman].frame[rnum]].box  , DDBLTFAST_NOCOLORKEY); */
/* 	  if (ddrval == DDERR_WASSTILLDRAWING) goto again2; */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = curx;
	    dst.y = cury;
	    SDL_BlitSurface(GFX_k[seq[seqman].frame[rnum]].k, NULL, GFX_lpDDSTwo, &dst);
	  }

	  //if (ddrval != DD_OK) dderror(ddrval);
	  curx += k[seq[seqman].frame[rnum]].box.right;
	  if (cur == 110)
	    {cury += k[seq[seqman].frame[rnum]].box.bottom+5;
	      curx = curx_start;

	    }

	  if (cur == 220) return;
	}
    }
}


void draw_health( void )
{
        flifemax = *plifemax;
        draw_bar(flifemax, 190);
        flife = *plife;
        draw_bar(flife, 451);
}

void draw_icons( void )
{
  if (*pcur_weapon != 0 && play.item[*pcur_weapon].active)
    {
      //disarm old weapon
      //play.item[*pcur_weapon].seq,
/*     again: */

      check_seq_status(play.item[*pcur_weapon].seq);

/*       ddrval = lpDDSTwo->BltFast(557, 413, k[seq[play.item[*pcur_weapon].seq].frame[play.item[*pcur_weapon].frame]].k, */
/* 				 &k[seq[play.item[*pcur_weapon].seq].frame[play.item[*pcur_weapon].frame]].box, */
/* 				 DDBLTFAST_SRCCOLORKEY); */
/*       if (ddrval == DDERR_WASSTILLDRAWING) goto again; */
      // GFX
      {
	SDL_Rect dst = {557, 413};
	SDL_BlitSurface(GFX_k[seq[play.item[*pcur_weapon].seq].frame[play.item[*pcur_weapon].frame]].k, NULL,
			GFX_lpDDSTwo, &dst);
      }
    }

  if (*pcur_magic != 0 && play.mitem[*pcur_magic].active)
    {
      //disarm old weapon
      //play.mitem[*pcur_magic].seq,
      check_seq_status(play.mitem[*pcur_magic].seq);

/*     again2: */
/*       ddrval = lpDDSTwo->BltFast( 153, 413, k[seq[play.mitem[*pcur_magic].seq].frame[play.mitem[*pcur_magic].frame]].k, */
/* 				  &k[seq[play.mitem[*pcur_magic].seq].frame[play.mitem[*pcur_magic].frame]].box, DDBLTFAST_SRCCOLORKEY); */
/*       if (ddrval == DDERR_WASSTILLDRAWING) goto again2; */
      // GFX
      {
	SDL_Rect dst = {153, 413};
	SDL_BlitSurface(GFX_k[seq[play.mitem[*pcur_magic].seq].frame[play.mitem[*pcur_magic].frame]].k, NULL,
			GFX_lpDDSTwo, &dst);
      }
    }
}


/** draw_virtical, draw_hor, draw_virt2, draw_hor2: used to draw the
    magic jauge (in that order) (dinkvar.cpp:draw_mlevel() only) **/

void draw_virtical(int percent, int mx, int my, int mseq, int mframe)
{
  int cut;
  if (percent > 25) percent = 25;
  percent = (percent * 4);
  rect myrect;
  rect_copy(&myrect, &k[seq[mseq].frame[mframe]].box);
  int full = myrect.bottom;
  cut = (full * percent) / 100;
  myrect.bottom = cut;

  my += (full - cut);

/*   ddrval = lpDDSTwo->BltFast(mx, my, k[seq[mseq].frame[mframe]].k, */
/* 			     &myrect, DDBLTFAST_NOCOLORKEY); */
  // GFX
  {
    /* TODO: test me! */
    SDL_Rect src, dst;
    src.x = src.y = 0;
    src.w = GFX_k[seq[mseq].frame[mframe]].k->w;
    src.h = GFX_k[seq[mseq].frame[mframe]].k->h * percent / 100;
    dst.x = mx;
    dst.y = my;
    SDL_BlitSurface(GFX_k[seq[mseq].frame[mframe]].k, &src, GFX_lpDDSTwo, &dst);
  }
}

void draw_virt2(int percent, int mx, int my, int mseq, int mframe)
{
  int cut;
  if (percent > 25) percent = 25;
  percent = (percent * 4);
  rect myrect;
  rect_copy(&myrect, &k[seq[mseq].frame[mframe]].box);
  int full = myrect.bottom;
  cut = (full * percent) / 100;
  myrect.bottom = cut;

/*  again: */
/*   ddrval = lpDDSTwo->BltFast( mx, my, k[seq[mseq].frame[mframe]].k, */
/* 			      &myrect, DDBLTFAST_NOCOLORKEY); */
/*   if (ddrval == DDERR_WASSTILLDRAWING) goto again; */
  // GFX
  {
    SDL_Rect src, dst;
    src.x = src.y = 0;
    src.w = GFX_k[seq[mseq].frame[mframe]].k->w;
    src.h = GFX_k[seq[mseq].frame[mframe]].k->h * percent / 100;
    dst.x = mx; dst.y = my;
    SDL_BlitSurface(GFX_k[seq[mseq].frame[mframe]].k, &src, GFX_lpDDSTwo, &dst);
  }
}

void draw_hor(int percent, int mx, int my, int mseq, int mframe)
{
  int cut;
  if (percent > 25) percent = 25;
  percent = (percent * 4);
  rect myrect;
  rect_copy(&myrect, &k[seq[mseq].frame[mframe]].box);
  int full = myrect.right;
  cut = (full * percent) / 100;
  full = cut;
  myrect.right = full;
/*  again: */
/*   ddrval = lpDDSTwo->BltFast( mx, my, k[seq[mseq].frame[mframe]].k, */
/* 			      &myrect, DDBLTFAST_NOCOLORKEY); */
/*   if (ddrval == DDERR_WASSTILLDRAWING) goto again; */
  // GFX
  {
    /* TODO: test me! */
    SDL_Rect src, dst;
    src.x = src.y = 0;
    src.w = GFX_k[seq[mseq].frame[mframe]].k->w * percent / 100;
    src.h = GFX_k[seq[mseq].frame[mframe]].k->h;
    dst.x = mx; dst.y = my;
    SDL_BlitSurface(GFX_k[seq[mseq].frame[mframe]].k, &src, GFX_lpDDSTwo, &dst);
  }
}

void draw_hor2(int percent, int mx, int my, int mseq, int mframe)
{
  int cut;
  if (percent > 25) percent = 25;
  percent = (percent * 4);
  rect myrect;
  rect_copy(&myrect, &k[seq[mseq].frame[mframe]].box);
  int full = myrect.right;
  cut = (full * percent) / 100;

  myrect.right = cut;
  mx += (full - cut);

/*  again: */
/*   ddrval = lpDDSTwo->BltFast( mx, my, k[seq[mseq].frame[mframe]].k, */
/* 			      &myrect, DDBLTFAST_NOCOLORKEY); */
/*   if (ddrval == DDERR_WASSTILLDRAWING) goto again; */
  // GFX
  {
    SDL_Rect src, dst;
    src.x = src.y = 0;
    src.w = GFX_k[seq[mseq].frame[mframe]].k->w * percent / 100;
    src.h = GFX_k[seq[mseq].frame[mframe]].k->h;
    dst.x = mx;
    dst.y = my;
    SDL_BlitSurface(GFX_k[seq[mseq].frame[mframe]].k, &src, GFX_lpDDSTwo, &dst);
  }
}

void draw_mlevel(int percent)
{
  //if (*pmagic_level < 1) return;

  int mseq = 180;
  int bary = 6;
  int barx = 7;

  if (percent > 0) draw_virtical(percent, 149, 411, mseq, bary);
  percent -= 25;
  if (percent > 0) draw_hor(percent, 149, 409, mseq, barx);
  percent -= 25;
  if (percent > 0) draw_virt2(percent, 215, 411, mseq, bary);
  percent -= 25;
  if (percent > 0) draw_hor2(percent, 149, 466, mseq, barx);
}


/* Draw the status bar and the magic jauge */
void draw_status_all(void)
{
/*   RECT rcRect; */
/*   rcRect.left = 0; */
/*   rcRect.top = 0; */
/*   rcRect.right = 640; */
/*   rcRect.bottom = 80; */
/*  again: */
/*   ddrval = lpDDSTwo->BltFast(0, 400, k[seq[180].frame[3]].k, */
/* 			     &rcRect, DDBLTFAST_NOCOLORKEY); */
/*   if (ddrval == DDERR_WASSTILLDRAWING) goto again; */
  // GFX
  {
    SDL_Rect src = {0, 0, 640, 80}, dst = {0, 400};
    SDL_BlitSurface(GFX_k[seq[180].frame[3]].k, &src, GFX_lpDDSTwo, &dst);
  }

/*   rcRect.left = 0; */
/*   rcRect.top = 0; */
/*   rcRect.right = 20; */
/*   rcRect.bottom = 400; */
/*  again2: */
/*   ddrval = lpDDSTwo->BltFast(0, 0, k[seq[180].frame[1]].k, */
/* 			     &rcRect, DDBLTFAST_NOCOLORKEY); */
/*   if (ddrval == DDERR_WASSTILLDRAWING) goto again2; */
/*  again3: */
/*   ddrval = lpDDSTwo->BltFast(620, 0, k[seq[180].frame[2]].k, */
/* 			     &rcRect, DDBLTFAST_NOCOLORKEY); */
/*   if (ddrval == DDERR_WASSTILLDRAWING) goto again3; */
  // GFX
  {
    SDL_Rect src = {0, 0, 20, 400}, dst1 = {0, 0}, dst2 = {620, 0};
    SDL_BlitSurface(GFX_k[seq[180].frame[1]].k, &src, GFX_lpDDSTwo, &dst1);
    SDL_BlitSurface(GFX_k[seq[180].frame[2]].k, &src, GFX_lpDDSTwo, &dst2);
  }

  fraise = next_raise();
  fexp = *pexper;
  fstrength = *pstrength;
  fmagic = *pmagic;
  fgold = *pgold;
  fdefense = *pdefense;
  last_magic_draw = 0;
  draw_exp();
  draw_health();
  draw_strength();
  draw_defense();
  draw_magic();
  draw_gold();
  draw_level();
  draw_icons();
  if (*pmagic_cost > 0 && *pmagic_level > 0)
    draw_mlevel(*pmagic_level / (*pmagic_cost / 100));
}



/*bool*/int inside_box(int x1, int y1, rect box)
{

        if (x1 > box.right) return(/*false*/0);
        if (x1 < box.left) return(/*false*/0);

        if (y1 > box.bottom) return(/*false*/0);
        if (y1 < box.top) return(/*false*/0);

        return(/*true*/1);

}



int add_sprite_dumb(int x1, int y, int brain,int pseq, int pframe,int size )
{
  int x;
    for (x = 1; x < MAX_SPRITES_AT_ONCE; x++)
        {
                if (spr[x].active == /*FALSE*/0)
                {
                        memset(&spr[x], 0, sizeof(spr[x]));

                        //Msg("Making sprite %d.",x);
                        spr[x].active = /*TRUE*/1;
                        spr[x].x = x1;
                        spr[x].y = y;
                        spr[x].my = 0;
                        spr[x].mx = 0;
                        spr[x].speed = 0;
                        spr[x].brain = brain;
                        spr[x].frame = 0;
                        spr[x].pseq = pseq;
                        spr[x].pframe = pframe;
                        spr[x].size = size;
                        spr[x].seq = 0;
                        if (x > last_sprite_created)
                                last_sprite_created = x;

                        spr[x].timer = 0;
                        spr[x].wait = 0;
                        spr[x].lpx[0] = 0;
                        spr[x].lpy[0] = 0;
                        spr[x].moveman = 0;
                        spr[x].seq_orig = 0;


            spr[x].base_hit = -1;
                        spr[x].base_walk = -1;
                        spr[x].base_die = -1;
                        spr[x].base_idle = -1;
                        spr[x].base_attack = -1;
                        spr[x].last_sound = 0;
                        spr[x].hard = 1;

                        rect_set(&spr[x].alt, 0,0,0,0);
                        spr[x].althard = 0;
                        spr[x].sp_index = 0;
                        spr[x].nocontrol = 0;
                        spr[x].idle = 0;
                        spr[x].strength = 0;
                        spr[x].damage = 0;
                        spr[x].defense = 0;

                        return(x);
                }

        }

        return(0);
}


/*bool*/int get_box (int h, rect * box_crap, rect * box_real )
{
  rect math;
  int sz,sy,x_offset,y_offset;

  int txoffset = k[getpic(h)].xoffset;
  int tyoffset = k[getpic(h)].yoffset;

  int mplayx = playx;
  int mplayl = playl;
  int mplayy = playy;

  if (spr[h].noclip)
    {
      mplayx = 640;
      mplayl = 0;
      mplayy = 480;
    }

  rect krect;

  if (getpic(h) < 1)
    {
      if (dinkedit) Msg("Yo, sprite %d has a bad pic. (Map %d) Seq %d, Frame %d",h,cur_map, spr[h].pseq, spr[h].pframe);
      else
	Msg("Yo, sprite %d has a bad pic. (Map %d) Seq %d, Frame %d",h,*pmap, spr[h].pseq, spr[h].pframe);
      //spr[h].pic = 44;
    }

  *box_real = k[getpic(h)].box;
  rect_copy(&krect, &k[getpic(h)].box);

  if (spr[h].size != 100) sz =    ((krect.right * spr[h].size) / 100); else sz = 0;
  if (spr[h].size != 100) sy =    ((krect.bottom * spr[h].size) / 100); else sy = 0;

  if (spr[h].size != 100)
    {
      sz = ((sz - krect.right) / 2);
      sy = ((sy - krect.bottom) / 2);
    }

  box_crap->left = spr[h].x-txoffset-sz;
  math.left = spr[h].x-txoffset;

  box_crap->top = spr[h].y - tyoffset-sy;
  math.top = spr[h].y-tyoffset;

  box_crap->right = (math.left+ (krect.right -krect.left)) + sz;
  math.right = math.left+ krect.right;

  box_crap->bottom = (math.top + (krect.bottom - krect.top)) + sy;
  math.bottom = math.top + krect.bottom;
  //if (OffsetRect(&spr[h].alt2,44,33) == 0) Msg("Error with set rect");


  if ( (spr[h].alt.right != 0) | (spr[h].alt.left != 0) | (spr[h].alt.top != 0) | (spr[h].alt.right != 0))
    {
      //spr[h].alt.bottom = 10;

      box_crap->left = box_crap->left +  spr[h].alt.left;
      box_crap->top = box_crap->top + spr[h].alt.top;
      box_crap->right = box_crap->right -  (k[getpic(h)].box.right - spr[h].alt.right);

      box_crap->bottom = box_crap->bottom - (k[getpic(h)].box.bottom - spr[h].alt.bottom);
      rect_copy(box_real, &spr[h].alt);
      //Msg("I should be changing box size... %d %d,%d,%d",spr[h].alt.right,spr[h].alt.left,spr[h].alt.top,spr[h].alt.bottom);
    }

  //********* Check to see if they need to be cut down and do clipping

  if (spr[h].size == 0) spr[h].size = 100;

  if (dinkedit) if ( (mode == 1) | (mode == 5) ) if (draw_map_tiny < 1) goto do_draw;

  if (box_crap->left < mplayl)
    {
      x_offset = box_crap->left * (-1) + mplayl;
      box_crap->left = mplayl;
      //box_real->left += (math.left * (-1)) + mplayl;

      if (spr[h].size != 100)
	box_real->left += (((x_offset * 100) / (spr[h].size )) ); else

	box_real->left += x_offset;
      if (box_crap->right-1 < mplayl) goto nodraw;
    }

  if (box_crap->top < 0)
    {
      y_offset = box_crap->top * (-1);
      box_crap->top = 0;

      //box_real->top += math.top * (-1) + 0;
      if (spr[h].size != 100)
	box_real->top += (((y_offset * 100) / (spr[h].size ))  );

      else box_real->top += y_offset;
      if (box_crap->bottom-1 < 0) goto nodraw;
    }

  if (box_crap->right > mplayx)
    {
      x_offset = (box_crap->right) - mplayx;
      box_crap->right = mplayx;
      //x_real->right -= math.right - mplayx;
      if (spr[h].size != 100)
	box_real->right -= ((x_offset * 100) / (spr[h].size ));
      else box_real->right -= x_offset;
      if (box_crap->left+1 > mplayx) goto nodraw;

      //      Msg("ok, crap right is %d, real right is %d.",box_crap->right - box_crap->left,box_real->right);
    }

  if (box_crap->bottom > mplayy)
    {
      y_offset = (box_crap->bottom) - mplayy;
      box_crap->bottom = mplayy;
      if (spr[h].size != 100)
	box_real->bottom -= ((y_offset * 100) / (spr[h].size ));
      else box_real->bottom -= y_offset;
      if (box_crap->top+1 > mplayy) goto nodraw;
    }

 do_draw:
  /*if (  (box_crap->right-box_crap->left) != (box_real->right-box_real->left) )
{
Msg("Ok, sprite %d is being scaled.", h);
}
        */
    return(/*true*/1);

 nodraw:
    return(/*false*/0);
}


/* void reload_sprites(char name[100], int nummy, int junk) */
/* { */
/*         HRESULT     ddrval; */
/*     PALETTEENTRY    holdpal[256];          */

/*         char crap[100],hold[10]; */
/*         int n; */
/*         n = 0;   */

/*         lpDDPal->GetEntries(0,0,256,holdpal);      */
/*         lpDDPal->SetEntries(0,0,256,real_pal); */


/*         for (int oo = index[nummy].s+1; oo <= index[nummy].s + index[nummy].last; oo++) */
/*         { */
/*                 n++; */

                //  Msg( "%s", crap);

                //      initFail(hWndMain, crap);
/*                 ddrval = k[oo].k->Restore(); */
/*         if( ddrval == DD_OK ) */
/*         { */


/*                         if (n < 10) strcpy(hold, "0"); else strcpy(hold,""); */
/*                         sprintf(crap, "%s%s%d.BMP",name,hold,n); */

/*                         DDReLoadBitmap(k[oo].k, crap); */
                        //Msg("Sprite %s%d.bmp reloaded into area %d. ",name,n,oo);


/*         } */
/*         } */
/*         lpDDPal->SetEntries(0,0,256,holdpal);    */
/* } */





void refigure_out(char line[255])
{
        char ev[15][100];
/*         int myseq = 0,myframe = 0; */
        int i;
        memset(&ev, 0, sizeof(ev));
        for (i = 1; i <= 14; i++)
        {
                separate_string(line, i,' ',ev[i]);
                //   Msg("Word %d is \"%s\"",i,ev[i]);
        }

        if (compare(ev[1],"LOAD_SEQUENCE_NOW"))
        {
                //           name   seq    speed       offsetx     offsety       hardx      hardy
                //      reload_sprites(ev[2],atol(ev[3]),0);
        }


}


void strchar(char *string, char ch)
/* This acts in the same way as strcat except it combines a string and
a single character, updating the null at the end. */
{
        int last;
        last=strlen(string);
        string[last]=ch;
        string[last+1]=0;

}


void kill_callbacks_owned_by_script(int script)
{
  int i;
        for (i = 1; i < MAX_CALLBACKS; i++)
        {
        if (callback[i].owner == script)
                {
                        if (debug_mode) Msg("Kill_all_callbacks just killed %d for script %d", i, script);
                        //killed callback
                        callback[i].active = /*false*/0;
                }
        }


}


void kill_script(int k)
{
  if (rinfo[k] != NULL)
    {
      int i;

      kill_callbacks_owned_by_script(k);
      
      // Now let's kill all local vars associated with this script
      for (i = 1; i < MAX_VARS; i++)
	{
	  if (play.var[i].active && play.var[i].scope == k)
	    play.var[i].active = /*false*/0;
	}
      if (debug_mode)
	Msg("Killed script %s. (num %d)", rinfo[k]->name, k);
      
      free(rinfo[k]);
      rinfo[k] = NULL;
      free(rbuf[k]);
      rbuf[k] = NULL;
    }
}


/* Used by gfx_tiles only - what's the difference with
   kill_all_scripts_for_real()? */
void kill_all_scripts(void)
{
        int k = 1;
        for (; k < MAX_SCRIPTS; k++)
        {

                if (rinfo[k] != NULL) if (rinfo[k]->sprite != 1000)
                        kill_script(k);
        }

        for (k = 1; k < MAX_CALLBACKS; k++)
        {
        if (callback[k].active)
                {
                        if ( (rinfo[callback[k].owner] != NULL) && (rinfo[callback[k].owner]->sprite == 1000) )
                        {

                        } else
                        {
                                if (debug_mode) Msg("Killed callback %d.  (was attached to script %d.)",k, callback[k].owner);
                                callback[k].active = 0;
                        }
                }
        }
}

void kill_all_scripts_for_real(void)
{
        int k = 1;
        for (; k < MAX_SCRIPTS; k++)
        {

                if (rinfo[k] != NULL)
                        kill_script(k);
        }

        for (; k <= MAX_CALLBACKS; k++)
        {

                callback[k].active = 0;
        }
}


/*bool*/int read_next_line(int script, char *line)
{
  int k;
        if (  (rinfo[script] == NULL) || (rbuf == NULL) )
        {

                Msg("  ERROR:  Tried to read script %d, it doesn't exist.", script);
                return(/*false*/0);
        }

        if (rinfo[script]->current >= rinfo[script]->end)
        {
                //at end of buffer
                return(/*false*/0);
        }

        /*              if (rinfo[script]->current < -1);
        {
        //something errored out, why did it go negetive?
        return(false);
        }
        */

        strcpy(line, "");

        for (k = rinfo[script]->current;  (k < rinfo[script]->end); k++)
        {


                //              Msg("..%d",k);
                strchar(line, rbuf[script][k]);
                rinfo[script]->current++;

                if (  (rbuf[script][k] == '\n') || (rbuf[script][k] == '\r')  )
                {
                        return(/*true*/1);
                }

                if (rinfo[script]->current >= rinfo[script]->end) return(/*false*/0);

        }

        return(/*false*/0);
}


void decompress (FILE *in)
{
        unsigned char stack[16], pair[128][2];
        short c, top = 0;

        /* Check for optional pair count and pair table */
        if ((c = getc(in)) > 127)
                fread(pair,2,c-128,in);
        else
        {
                if (c == '\r') c = '\n';
                if (c == 9) c = ' ';

                strchar(cbuf,c);
        }
        //        putc(c,out);

        for (;;) {

                /* Pop byte from stack or read byte from file */
                if (top)
                        c = stack[--top];
                else if ((c = getc(in)) == EOF)
                        break;

                /* Push pair on stack or output byte to file */
                if (c > 127) {
                        stack[top++] = pair[c-128][1];
                        stack[top++] = pair[c-128][0];
                }
                else
                {
                        if (c == '\r') c = '\n';
                        if (c == 9) c = ' ';

                        strchar(cbuf,c);//     putc(c,out);
                }
        }
}


void decompress_nocomp (FILE *in)
{
        //let's do it, only this time decompile OUR style

        unsigned char stack[16], pair[128][2];
        short c, top = 0;

        /* Check for optional pair count and pair table */
        if ((c = getc(in)) > 255)
                fread(pair,2,c-128,in);
        else
        {
                if (c == '\r') c = '\n';
                if (c == 9) c = ' ';

                strchar(cbuf,c);
        }
        //        putc(c,out);

        for (;;) {

                /* Pop byte from stack or read byte from file */
                if (top)
                        c = stack[--top];
                else if ((c = getc(in)) == EOF)
                        break;

                /* Push pair on stack or output byte to file */
                if (c > 255) {
                        stack[top++] = pair[c-128][1];
                        stack[top++] = pair[c-128][0];
                }
                else
                {
                        if (c == '\r') c = '\n';
                        if (c == 9) c = ' ';

                        strchar(cbuf,c);//     putc(c,out);
                }
        }
}


/**
 * Load script from 'filename', save it in the first available script
 * slot, attach to game sprite #'sprite' if 'set_sprite' is 1.
 **/
int load_script(char filename[15], int sprite, /*bool*/int set_sprite)
{
  char temp[100];
  int script;
  FILE *stream;
  /*bool*/int comp = /*false*/0;
  char tab[10];
  
  Msg("LOADING %s",filename);
  sprintf(tab, "%c",9);
  
  sprintf(temp, "story\\%s.d", filename);
  
  if (!exist(ciconvert(temp)))
    {
      sprintf(temp, "story\\%s.c", filename);
      if (!exist(ciconvert(temp)))
	{
	  sprintf(temp, "..\\dink\\story\\%s.d", filename);
	  if (!exist(ciconvert(temp)))
	    {
	      sprintf(temp, "..\\dink\\story\\%s.c", filename);
	      if (!exist(ciconvert(temp)))
		{
		  Msg("Script %s not found. (checked for .C and .D) (requested by %d?)", temp, sprite);
		  return 0;
		}
	    }
	}
    }
  
  strtoupper(temp);
  Msg("Temp thingie is %c",temp[strlen(temp)-1]);
  if (temp[strlen(temp)-1] == 'D')
    comp = 1;
  else
    comp = 0;
  
  int k;
  for (k = 1; k < MAX_SCRIPTS; k++)
    {
      if (rbuf[k] == NULL)
	{
	  //found one not being used
	  goto found;
	}
    }
  
  Msg("Couldn't find unused buffer for script.");
  return 0;
  

 found:
  Msg("Loading script %s..", temp);

  script = k;
  rinfo[script] = (struct refinfo *) malloc( sizeof(struct refinfo));
  memset(rinfo[script], 0, sizeof(struct refinfo));
  
  //if compiled
  {
    //load compiled script
    if ((stream = fopen(ciconvert(temp), "rb")) == NULL)
      {
	Msg("Script %s not found. (checked for .C and .D) (requested by %d?)",temp, sprite);
	return(0);
      }
    
    cbuf[0] = 0;
    //Msg("decompressing!");
    
    if (comp)
      decompress(stream);
    else
      decompress_nocomp(stream);
    
    fclose(stream);
    
    //Msg("done decompressing!");
    
    //file is now in cbuf!!
    

    rinfo[script]->end = (strlen(cbuf));
    //Msg("dlength is %d!", rinfo[script]->end);
    
    rbuf[script] = (char *) malloc(rinfo[script]->end);
    
    //rbuf[script] = new [script]->end;
    
    if (rbuf[script] == NULL)
      {
	Msg("Couldn't allocate rbuff %d.",script);
	return 0;
      }
    
    memcpy(rbuf[script], &cbuf, rinfo[script]->end);
    
    if (rinfo[script] == NULL)
      {
	Msg("Couldn't allocate rscript %d.",script);
	return 0;
      }
  }
  //Msg("Script %s loaded by sprite %d into space %d.",temp, sprite,script);
  strcpy(rinfo[script]->name, filename);
  rinfo[script]->sprite = sprite;
  
  
  if (set_sprite && sprite != 0 && sprite != 1000)
    spr[sprite].script = script;

  return (script);
}


void strip_beginning_spaces(char *str)
{
  char *pc = str;
  int diff = 0;
/*   int i; */

  /* Find first non-space character (pos) */
  while (*pc == ' ')
    pc++;
  diff = pc - str;

  /* Shift string to the left from pos */
  /* Don't use str(str, pc) to avoid memory overlap */
  while (*pc != '\0')
    {
      *(pc - diff) = *pc;
      pc++;
    }
  *(pc - diff) = '\0';
}



/*bool*/int locate(int script, char proc[20])
{

        if (rinfo[script] == NULL)
        {
                return(/*false*/0);

        }
        int saveme = rinfo[script]->current;
        rinfo[script]->current = 0;
        char line[200];
        char ev[3][100];
        char temp[100];


        //Msg("locate is looking for %s in %s", proc, rinfo[script]->name);

        while(read_next_line(script, line))
        {
                strip_beginning_spaces(line);
                memset(&ev, 0, sizeof(ev));

                get_word(line, 1, ev[1]);
                if (compare(ev[1], "VOID"))
                {
                        get_word(line, 2, ev[2]);

                        separate_string(ev[2], 1,'(',temp);

                        //              Msg("Found procedure %s.",temp);
                        if (compare(temp,proc))
                        {
                                //                              Msg("Located %s",proc);
                                //clean up vars so it is ready to run
                                if (rinfo[script]->sprite != 1000)
                                {
                                        spr[rinfo[script]->sprite].move_active = /*false*/0;

                                }
                                rinfo[script]->skipnext = /*false*/0;
                                rinfo[script]->onlevel = 0;
                                rinfo[script]->level = 0;

                                return(/*true*/1);
                                //this is desired proc

                        }
                }

        }

        //Msg("Locate ended on %d.", saveme);
        rinfo[script]->current = saveme;
        return(/*false*/0);

}

/**
 * Look for the 'proc' label (e.g. 'loop:'), that is used by a "goto"
 * instruction. This sets the script->current field appropriately.
 **/
/*bool*/int locate_goto(char proc[50], int script)
{
  rinfo[script]->current = 0;
  char line[200];
  char ev[3][100];
  replace(";", "", proc);
  strchar(proc, ':');
  // Msg("locate is looking for %s", proc);
  
  while (read_next_line(script, line))
    {
      strip_beginning_spaces(line);
      
      get_word(line, 1, ev[1]);
      replace("\n", "",ev[1]);
      
      if (compare(ev[1], proc))
	{
	  if (debug_mode) Msg("Found goto : Line is %s, word is %s.", line, ev[1]);
	  
	  rinfo[script]->skipnext = /*false*/0;
	  rinfo[script]->onlevel = 0;
	  rinfo[script]->level = 0;
	  
	  return /*true*/1;
	  //this is desired proc
	}
    }
  Msg("ERROR:  Cannot goto %s in %s.", proc, rinfo[script]->name);
  return /*false*/0;
}


/*void check_for_real_vars(char crap[20], int i)
{
if (compare(play.var[i].name, "&vision")) map_vision = play.var[i].var;
if (compare(play.var[i].name, "&life")) map_vision = play.var[i].var;
if (compare(play.var[i].name, "&exp")) map_vision = play.var[i].var;

  }
*/

void decipher(char *crap, int script)
{
  int i;

        if (compare(crap, "&current_sprite"))
        {

                sprintf(crap, "%d",rinfo[script]->sprite);
                //Msg("cur sprite returning %s, ",crap);
                return;
        }

        if (compare(crap, "&current_script"))
        {
                sprintf(crap, "%d",script);
                return;
        }





        for (i = 1; i < MAX_VARS; i ++)
        {


                if (play.var[i].active == /*true*/1) if (  (play.var[i].scope == 0) | (play.var[i].scope == script) )
                        if (compare(play.var[i].name, crap))
                        {


                                sprintf(crap, "%d",play.var[i].var);
                                //        check_for_real_vars(crap, i);

                                return;
                        }

        }


}

void decipher_string(char line[200], int script)
{
        char crap[20];
        char buffer[20];
        char crab[100];
        int mytime;
	int i;
        for (i = 1; i < MAX_VARS; i ++)
        {

                if (play.var[i].active == /*true*/1) if (  (play.var[i].scope == 0) | (play.var[i].scope == script) )

                {
                        sprintf(crap, "%d", play.var[i].var);
                        replace(play.var[i].name, crap, line);
                        //        check_for_real_vars(crap, i);


                }

        }

        if ((strchr(line, '&') != NULL) && (script != 0))
        {
	  sprintf(buffer, "%d", rinfo[script]->sprite);
	  replace("&current_sprite", buffer, line);
	  sprintf(buffer, "%d", script);
	  replace("&current_script", buffer, line);


                if (decipher_savegame != 0)
                {
                        if (play.button[decipher_savegame] == 1)        replace("&buttoninfo", "Attack", line);
                        else
                                if (play.button[decipher_savegame] == 2)        replace("&buttoninfo", "Talk/Examine", line);
                                if (play.button[decipher_savegame] == 3)        replace("&buttoninfo", "Magic", line);
                                if (play.button[decipher_savegame] == 4)        replace("&buttoninfo", "Item Screen", line);
                                if (play.button[decipher_savegame] == 5)        replace("&buttoninfo", "Main Menu", line);
                                if (play.button[decipher_savegame] == 6)        replace("&buttoninfo", "Map", line);
                                if (play.button[decipher_savegame] == 7)        replace("&buttoninfo", "Unused", line);
                                if (play.button[decipher_savegame] == 8)        replace("&buttoninfo", "Unused", line);
                                if (play.button[decipher_savegame] == 9)        replace("&buttoninfo", "Unused", line);
                                if (play.button[decipher_savegame] == 10)       replace("&buttoninfo", "Unused", line);


                }

        }


        if (decipher_savegame != 0)
                if (compare(line, "&savegameinfo"))
                {
                        sprintf(crap, "save%d.dat",decipher_savegame);
                        if (exist(crap))
                        {
                                load_game_small(decipher_savegame, crab, &mytime);
                                sprintf(line, "Slot %d - %d:%d - %s",decipher_savegame, (mytime / 60),
                                        mytime - ((mytime / 60) * 60) , crab);
                                //sprintf(line, "In Use");
                        } else
                        {

#ifdef __GERMAN
                                sprintf(line, "Slot %d - Ungebraucht",decipher_savegame);
#endif

#ifdef __ENGLISH
                                sprintf(line, "Slot %d - Empty",decipher_savegame);
#endif


                        }

                }


}



/*bool*/int get_parms(char proc_name[20], int script, char *h, int p[10])
{
  int i;
        char crap[100];

        strip_beginning_spaces(h);
        if (h[0] == '(')
        {
                //Msg("Found first (.");
                h = &h[1];

        } else
        {
                Msg("Missing ( in %s, offset %d.", rinfo[script]->name, rinfo[script]->current);


                return(/*false*/0);
        }



        for (i = 0; i < 10; i++)
        {

                strip_beginning_spaces(h);


                if (p[i] == 1)
                {
                        // Msg("Checking for number..");


                        if (strchr(h, ',') != NULL)
                                separate_string(h, 1,',',crap); else
                                if (strchr(h, ')') != NULL)
                                        separate_string(h, 1,')',crap);


                                h = &h[strlen(crap)];


                                if (crap[0] == '&')
                                {
                                        replace(" ", "", crap);
                                        //      Msg("Found %s, 1st is %c",crap, crap[0]);
                                        decipher(crap, script);


                                }

                                nlist[i] = atol( crap);

                } else

                        if (p[i] == 2)
                        {
                                // Msg("Checking for string..");
                                separate_string(h, 2,'"',crap);
                                h = &h[strlen(crap)+2];

                                //Msg("Found %s",crap);
                                strcpy(slist[i], crap);

                        }


                        if ( p[i+1] == 0)
                        {
                                //finish
                                strip_beginning_spaces(h);

                                if (h[0] == ')')
                                {
                                        h = &h[1];
                                } else
                                {

                                        Msg("Missing ) in %s, offset %d.", rinfo[script]->name, rinfo[script]->current);
                                        h = &h[1];

                                        return(/*false*/0);
                                }

                                strip_beginning_spaces(h);

                                if (h[0] == ';')
                                {
                                        //  Msg("Found ending ;");
                                        h = &h[1];

                                } else
                                {
                                        //Msg("Missing ; in %s, offset %d.", rinfo[script]->name, rinfo[script]->current);
                                        //      h = &h[1];

                                        return(/*true*/1);
                                }





                                return(/*true*/1);
                        }


                        //got a parm, but there is more to get, lets make sure there is a comma there
                        strip_beginning_spaces(h);

                        if (h[0] == ',')
                        {
                                //     Msg("Found expected ,");
                                h = &h[1];

                        } else
                        {
                                Msg("Procedure %s does not take %d parms in %s, offset %d. (%s?)", proc_name, i+1, rinfo[script]->name, rinfo[script]->current, h);

                                return(/*false*/0);
                        }

        }


        return(/*true*/1);
}


/**
 * 
 * name: name of the procedure() to call
 * n1: wait at least n1 milliseconds before callback
 * n2: wait at most n1+n2 milliseconds before callback
 * script: number of script currently interpreted
 **/
int add_callback(char name[20], int n1, int n2, int script)
{
  int k;
  for (k = 1; k < MAX_CALLBACKS; k++)
    {
      if (callback[k].active == /*false*/0)
	{
	  memset(&callback[k], 0, sizeof(callback[k]));
	  
	  callback[k].active = /*true*/1;
	  callback[k].min = n1;
	  callback[k].max = n2;
	  callback[k].owner = script;
	  strcpy(callback[k].name, name);
	  
	  if (debug_mode)
	    Msg("Callback added to %d.", k);
	  return(k);
	}
    }
  
  Msg("Couldn't add callback, all out of space");
  return(0);
}


int add_sprite(int x1, int y, int brain,int pseq, int pframe )
{
  int x;
    for (x = 1; x < MAX_SPRITES_AT_ONCE; x++)
        {
                if (spr[x].active == /*FALSE*/0)
                {
                        memset(&spr[x], 0, sizeof(spr[x]));

                        spr[x].active = /*TRUE*/1;
                        spr[x].x = x1;
                        spr[x].y = y;
                        spr[x].my = 0;
                        spr[x].mx = 0;
                        spr[x].speed = 1;
                        spr[x].brain = brain;
                        spr[x].frame = 0;
                        spr[x].pseq = pseq;
                        spr[x].pframe = pframe;
                        spr[x].seq = 0;
                        if (x > last_sprite_created)
                                last_sprite_created = x;
                        spr[x].timer = 33;
                        spr[x].wait = 0;
                        spr[x].lpx[0] = 0;
                        spr[x].lpy[0] = 0;
                        spr[x].moveman = 0;
                        spr[x].size = 100;
                        spr[x].que = 0;
                        spr[x].strength = 0;
                        spr[x].damage = 0;
                        spr[x].defense = 0;
                        spr[x].hard = 1;


                        return(x);
                }

        }

        return(0);
}

/* Editor only */
void check_sprite_status(int h)
{
/*         HRESULT dderror; */
/*         char word1[80]; */
        //is sprite in memory?
        if (spr[h].pseq > 0)
        {
                // Msg("Smartload: Loading seq %d..", spr[h].seq);
                if (seq[spr[h].pseq].frame[1] == 0)
                {
                        figure_out(seq[spr[h].pseq].data, 0);
                }
                else
                {
                        //it's been loaded before.. is it lost or still there?
                        //Msg("Sprite %d's seq is %d",h,spr[h].seq);

/*                         dderror = k[seq[spr[h].pseq].frame[1]].k->IsLost(); */

/*                         if (dderror == DDERR_SURFACELOST) */
/*                         { */
/*                                 get_word(seq[spr[h].pseq].data, 2, word1); */

/*                                 reload_sprites(word1, spr[h].pseq,0); */
/*                                 //Msg("Reloaded seq %d with path of %s should be %s", spr[h].seq, word1,seq[spr[h].seq].data ); */
/*                         } */


                }
        }




}

/* Editor only */
void check_frame_status(int h, int frame)

{
/*         HRESULT dderror; */
/*         char word1[80]; */

        if (seq[h].active == /*false*/0) return;

        if (h > 0)
        {
                // Msg("Smartload: Loading seq %d..", spr[h].seq);
                if (seq[h].frame[1] == 0)
                {
                        first_frame = /*true*/1;
                        figure_out(seq[h].data, 0);
                        first_frame = /*false*/0;
                }
                else
                {
                        //it's been loaded before.. is it lost or still there?
                        //Msg("Sprite %d's seq is %d",h,spr[h].seq);

/*                         dderror = k[seq[h].frame[1]].k->IsLost(); */

/*                         if (dderror == DDERR_SURFACELOST) */
/*                         { */
/*                                 get_word(seq[h].data, 2, word1); */

/*                                 reload_sprites(word1, h,0); */
/*                                 //Msg("Reloaded seq %d with path of %s should be %s", spr[h].seq, word1,seq[spr[h].seq].data ); */
/*                         } */
                }
        }


}

void check_seq_status(int h)

{
/*         HRESULT dderror; */
/*         char word1[80]; */

        if (seq[h].active == /*false*/0) return;
        if (h > 0) if (h < MAX_SEQUENCES)
        {
                // Msg("Smartload: Loading seq %d..", spr[h].seq);

                if (seq[h].frame[1] == 0)
                {
                        figure_out(seq[h].data, 0);
                }
                else
                {
                        //it's been loaded before.. is it lost or still there?
                        //Msg("Sprite %d's seq is %d",h,spr[h].seq);

/*                         dderror = k[seq[h].frame[1]].k->IsLost(); */

/*                         if (dderror == DDERR_SURFACELOST) */
/*                         { */
/*                                 get_word(seq[h].data, 2, word1); */

/*                                 reload_sprites(word1, h,0); */
/*                                 //Msg("Reloaded seq %d with path of %s should be %s", spr[h].seq, word1,seq[spr[h].seq].data ); */
/*                         } */
                }
        }


}


void check_base(int base)
{
  int i;
        for (i = 1; i < 10; i++)
        {
                if (seq[base+i].active == /*TRUE*/1) check_seq_status(base+i);

        }


}

void check_sprite_status_full(int h)
{
        //same as above but checks for all seq's used by the (base) commands

        //is sprite in memory?

        check_seq_status(spr[h].pseq);

        if (spr[h].base_walk > -1) check_base(spr[h].base_walk);


}

/* say_text, say_text_xy: used by the game only (not the editor) */
int say_text(char text[200], int h, int script)
{
        int crap2;
        //Msg("Creating new sprite with %s connect to %d.",text, h);
        if (h == 1000) crap2 = add_sprite(100,100,8,0,0);
        else crap2 = add_sprite(spr[h].x,spr[h].y,8,0,0);

        if (crap2 == 0)
        {
                Msg("Couldn't say something, out of sprites.");
                return(0);

        }
        *plast_text = crap2;
        strcpy(spr[crap2].text, text);
        spr[crap2].kill = strlen(text) * text_timer;
        if (spr[crap2].kill < text_min) spr[crap2].kill = text_min;
        spr[crap2].damage = -1;
        spr[crap2].owner = h;
        spr[crap2].hard = 1;
        spr[crap2].script = script;
        //set X offset for text, using strength var since it's unused
        spr[crap2].strength = 75;
        //spr[h].x - spr[crap2;
        spr[crap2].nohit = 1;
        check_seq_status(spr[spr[crap2].owner].seq);
        spr[crap2].defense = ( ((k[getpic(spr[crap2].owner)].box.bottom) - k[getpic(spr[crap2].owner)].yoffset) + 100);

        spr[crap2].x = spr[spr[crap2].owner].x - spr[crap2].strength;
        spr[crap2].y = spr[spr[crap2].owner].y - spr[crap2].defense;

        return(crap2);
}


int say_text_xy(char text[200], int mx, int my, int script)
{
        int crap2;
        //Msg("Creating new sprite with %s connect to %d.",text, h);
        crap2 = add_sprite(mx,my,8,0,0);

        if (crap2 == 0)
        {
                Msg("Couldn't say something, out of sprites.");
                return(0);

        }
        *plast_text = crap2;
        strcpy(spr[crap2].text, text);
        spr[crap2].kill = strlen(text) * text_timer;
        if (spr[crap2].kill < text_min) spr[crap2].kill = text_min;
        spr[crap2].damage = -1;
        spr[crap2].nohit = 1;
        spr[crap2].owner = 1000;
        spr[crap2].hard = 1;
        spr[crap2].script = script;
        return(crap2);
}



int does_sprite_have_text(int sprite)
{
  int k;
        //Msg("getting callback # with %d..", sprite);
        for (k = 1; k <= MAX_SPRITES_AT_ONCE; k++)
        {
                if (   spr[k].active) if (spr[k].owner == sprite) if (spr[k].brain == 8)
                {
                        //Msg("Found it!  returning %d.", k);

                        return(k);
                }

        }

        return(0);

}

int var_exists(char name[20], int scope)
{
  int i;
        for (i = 1; i < MAX_VARS; i++)
        {
                if (play.var[i].active)
                {
                        if (compare(play.var[i].name, name))
                        {

                                if (scope == play.var[i].scope)
                                {
                                        //Msg("Found match for %s.", name);
                                        return(i);
                                }
                        }



                }
        }

        return(0);
}

void make_int(char name[80], int value, int scope, int script)
{
        int dupe;
	int i;
        if (strlen(name) > 19)
        {

                Msg("ERROR:  Varname %s is too long in script %s.",name, rinfo[script]->name);
                return;
        }
        dupe = var_exists(name, scope);

        if (dupe > 0)
        {
                if (scope != 0)
                {
                        Msg("Local var %s already used in this procedure in script %s.",name, rinfo[script]->name);

                        play.var[dupe].var = value;

                } else
                        Msg("Var %s is already a global, not changing value.",name);

                return;
        }


        //make new var

        for (i = 1; i < MAX_VARS; i++)
        {
                if (play.var[i].active == /*false*/0)
                {

                        play.var[i].active = /*true*/1;
                        play.var[i].scope = scope;
                        strcpy(play.var[i].name, name);
                        //g("var %s created, used slot %d ", name,i);
                        play.var[i].var = value;
                        return;
                }
        }


        Msg("ERROR: Out of var space, all %d used.", MAX_VARS);
}


int var_equals(char name[20], char newname[20], char math, int script, char rest[200])
{
        int k;
    int newret;

        int newval = 0;


        if (name[0] != '&')
        {
                Msg("ERROR (var equals): Unknown var %s in %s offset %d.",name, rinfo[script]->name, rinfo[script]->current);
                return(0);
        }

        int i = 1;
        for (; i < MAX_VARS; i++)
        {
                if  (play.var[i].active == /*true*/1)
                {

                        if ( (play.var[i].scope == 0) | (play.var[i].scope == script))
                                if (compare(name, play.var[i].name))
                                {
                                        //found var
                                        goto next;
                                }



                }
        }





        Msg("ERROR: (var equals2) Unknown var %s in %s offset %d.",name, rinfo[script]->name, rinfo[script]->current);
        return(0);

next:



        if (strchr(rest, '(') != NULL)

        {
                newret = process_line(script, rest, /*false*/0);
                newval = returnint;
                goto next2;
        }


        if (strchr(newname, ';') != NULL) replace(";", "", newname);
        for (k = 1; k < MAX_VARS; k++)
        {
                if (play.var[k].active == /*true*/1)
                {
                        if ( (play.var[i].scope == 0) | (play.var[i].scope == script))
                                if (compare(newname, play.var[k].name))
                                {
                                        newval = play.var[k].var;
                                        //found var
                                        goto next2;
                                }
                }
        }

        if (compare(newname, "&current_sprite"))
        {
                newval = rinfo[script]->sprite;
                goto next2;
        }

        if (compare(newname, "&current_script"))
        {
                newval = script;
                goto next2;

        }



        newval = atol(newname);

next2:

        if (math == '=')
                play.var[i].var = newval;

        if (math == '+')
                play.var[i].var += newval;

        if (math == '-')
                play.var[i].var -= newval;

        if (math == '/')
                play.var[i].var = play.var[i].var / newval;

        if (math == '*')
                play.var[i].var = play.var[i].var * newval;

        return(newret);
}




void get_word(char line[300], int word, char *crap)
{
        int cur = 0;
        int k;

        /*bool*/int space_mode = /*false*/0;
        char save_word[100];
        save_word[0] = 0;

        for (k = 0; k < strlen(line); k++)
        {

                if (space_mode == /*true*/1)
                {
                        if (line[k] != ' ')
                        {
                                space_mode = /*false*/0;
                                strcpy(save_word, "");

                        }
                }



                if (space_mode == /*false*/0)
                {
                        if (line[k] == ' ')
                        {
                                cur++;
                                if (word == cur) goto done;
                                space_mode = /*true*/1;
                                strcpy(save_word, "");

                                goto dooba;
                        } else
                        {
                                strchar(save_word, line[k]);

                        }
                }


dooba:;

        }

        if (space_mode == /*false*/0)
        {

                if (cur+1 != word) strcpy(save_word, "");
        }


done:

        strcpy(crap, save_word);

        //Msg("word %d of %s is %s.", word, line, crap);
}




int var_figure(char h[200], int script)
{
        char crap[200];
        int ret = 0;
        int n1 = 0, n2 = 0;
        //Msg("Figuring out %s...", h);
        get_word(h, 2, crap);
        //Msg("Word two is %s...", crap);

        if (compare(crap, ""))
        {
                //one word equation

                if (h[0] == '&')
                {
                        //its a var possibly
                        decipher_string(h, script);
                }

                //Msg("truth is %s", h);
                ret =  atol(h);
                //      Msg("returning %d, happy?", ret);
                return(ret);
        }


        //


        get_word(h, 1, crap);
        //Msg("Comparing %s...", crap);

        decipher_string(crap,script);
        n1 = atol(crap);

        get_word(h, 3, crap);
        replace(")", "", crap);
        //Msg("to  %s...", crap);
        decipher_string(crap,script);
        n2 = atol(crap);

        get_word(h, 2, crap);
        if (debug_mode)
                Msg("Compared %d to %d",n1, n2);

        if (compare(crap, "=="))
        {
                if (n1 == n2) ret = 1; else ret = 0;
                return(ret);
        }

        if (compare(crap, ">"))
        {
                if (n1 > n2) ret = 1; else ret = 0;
                return(ret);
        }

        if (compare(crap, ">="))
        {
                if (n1 >= n2) ret = 1; else ret = 0;
                return(ret);
        }


        if (compare(crap, "<"))
        {
                if (n1 < n2) ret = 1; else ret = 0;
                return(ret);
        }
        if (compare(crap, "<="))
        {
                if (n1 <= n2) ret = 1; else ret = 0;
                return(ret);
        }

        if (compare(crap, "!="))
        {
                if (n1 != n2) ret = 1; else ret = 0;
                return(ret);
        }

        return(ret);

}

void kill_text_owned_by(int sprite)
{
  int i;
  for (i = 1; i < MAX_SPRITES_AT_ONCE; i++)
    {
      if (spr[i].active && spr[i].brain == 8 && spr[i].owner == sprite)
	spr[i].active = /*false*/0;
    }
}

/*bool*/int text_owned_by(int sprite)
{
  int i;
        for (i = 1; i < MAX_SPRITES_AT_ONCE; i++)
        {
                if (spr[i].active)
                        if (spr[i].brain == 8) if (spr[i].owner == sprite)
                        {

                                return(/*true*/1);



                        }
        }
        return(/*false*/0);
}


void kill_scripts_owned_by(int sprite)
{
  int i;
        for (i = 1; i < MAX_SCRIPTS; i++)
        {
                if (rinfo[i] != NULL)
                {
                        if (rinfo[i]->sprite == sprite)
                        {
                                kill_script(i);

                        }

                }
        }

}

void kill_sprite_all (int sprite)
{
        spr[sprite].active = /*false*/0;

        kill_text_owned_by(sprite);
        kill_scripts_owned_by(sprite);

}


void kill_returning_stuff(int script)
{
  //Msg("Checking callbacks..");
  //check callbacks

  int i;
  // callbacks from wait() and run_script_by_number()
  for (i = 1; i < MAX_CALLBACKS; i++)
    {
      if (callback[i].active && callback[i].owner == script)
	//      if (compare(callback[i].name, ""))
	{
	  Msg("killed a returning callback, ha!");
	  callback[i].active = /*false*/0;
	}

    }

  // callbacks from say_*()
  for (i = 1; i <= last_sprite_created; i++)
    {
      if (spr[i].active && spr[i].brain == 8 && spr[i].callback == script)
	{
	  Msg("Killed sprites callback command");
	  spr[i].callback = 0;
	}
    }
}


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


void get_right(char line[200], char thing[100], char *ret)
        {
                char *dumb;
                int pos = strcspn(line, thing );


                if (pos == 0){ strcpy(ret, ""); return; }


                dumb = &ret[pos+1];
                strcpy(ret, dumb);
        }




        void int_prepare(char line[100], int script)
        {
                int def = 0;
                char hold[100];
                strcpy(hold, line);
                char name[100];
                char crap[100];
                replace("="," ",line);
                strcpy(crap, line);
                separate_string(crap, 1,';',line);
                get_word(line, 2, name);

                if (name[0] != '&')
                {
                        Msg("ERROR:  Can't create var %s, should be &%s.", name,name);
                        return;
                }


                make_int(name, def,script, script);

                strcpy(line, hold);

        }

        int change_sprite(int h,  int val, int * change)
        {
                //Msg("Searching sprite %s with val %d.  Cur is %d", h, val, *change);

                if (h < 1)
                {
                        Msg("Error with an SP command - Sprite %d is invalid.", h);
                        return(-1);
                }
                if (spr[h].active == /*false*/0) return(-1);
                if (val != -1)
                {
                        *change = val;
                }

                return(*change);

        }

        int change_edit(int h,  int val, unsigned short * change)
        {
                //Msg("Searching sprite %s with val %d.  Cur is %d", h, val, *change);

                if (h > 99) return(-1);
                if (h < 1) return(-1);
                if (val != -1)
                {
                        *change = val;
                }

                return(*change);

        }
        int change_edit_char(int h,  int val, unsigned char * change)
        {
                //Msg("Searching sprite %s with val %d.  Cur is %d", h, val, *change);
                //  Msg("h is %d..",val);
                if (h > 99) return(-1);
                if (h < 1) return(-1);
                if (val != -1)
                {
                        *change = val;
                }

                return(*change);

        }


        int change_sprite_noreturn(int h,  int val, int * change)
        {
                //Msg("Searching sprite %s with val %d.  Cur is %d", h, val, *change);
                if (spr[h].active == /*false*/0) return(-1);

                {
                        *change = val;
                }

                return(*change);

        }


void draw_sprite_game(SDL_Surface *GFX_lpdest, int h)
{
  if (g_b_kill_app) return; //don't try, we're quitting
  if (spr[h].brain == 8) return;

  if (spr[h].nodraw == 1) return;
  rect box_crap,box_real;

/*   HRESULT             ddrval; */

/*   DDBLTFX     ddbltfx; */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
/*   ddbltfx.dwFillColor = 0; */

  if (get_box(h, &box_crap, &box_real))
/*     while( 1) */
/*       { */
	//      Msg("Box_crap: %d %d %d %d, Box_real: %d %d %d %d",box_crap.left,box_crap.top,
	//              box_crap.right, box_crap.bottom,box_real.left,box_real.top,
	//              box_real.right, box_real.bottom);

/*       again: */
/* 	ddrval = lpdest->Blt(&box_crap, k[getpic(h)].k, */
/* 			     &box_real  , DDBLT_KEYSRC ,&ddbltfx ); */
/* 	if (ddrval == DDERR_WASSTILLDRAWING) goto again; */
	// GFX
	/* Generic scaling */
	/* Not perfectly accurate yet: move a 200% sprite to the
	   border of the screen to it is clipped: it's scaled size
	   will slighly vary. Maybe we need to clip the source zone
	   before scaling it.. */
	{
	  SDL_Rect src, dst;
	  SDL_Surface *scaled;
	  double sx, sy;
	  src.x = box_real.left;
	  src.y = box_real.top;
	  src.w = box_real.right - box_real.left;
	  src.h = box_real.bottom - box_real.top;
	  dst.x = box_crap.left;
	  dst.y = box_crap.top;
	  dst.w = box_crap.right - box_crap.left;
	  dst.h = box_crap.bottom - box_crap.top;
	  sx = 1.0 * dst.w / src.w;
	  sy = 1.0 * dst.h / src.h;
	  /* In principle, double's are precised up to 15 decimal
	     digits */
	  if (fabs(sx-1) > 1e-10 || fabs(sy-1) > 1e-10)
	    {
	      scaled = zoomSurface(GFX_k[getpic(h)].k, sx, sy, SMOOTHING_OFF);
	      /* Disable transparency if it wasn't active in the source surface
		 (SDL_gfx bug, report submitted to the author) */
	      if ((GFX_k[getpic(h)].k->flags & SDL_SRCCOLORKEY) == 0)
		SDL_SetColorKey(scaled, 0, 0);
	      src.x = (int) round(src.x * sx);
	      src.y = (int) round(src.y * sy);
	      src.w = (int) round(src.w * sx);
	      src.h = (int) round(src.h * sy);
	      if (SDL_BlitSurface(scaled, &src, GFX_lpdest, &dst) < 0)
		fprintf(stderr, "Could not draw sprite %d: %s\n", getpic(h), SDL_GetError());
	      SDL_FreeSurface(scaled);
	    }
	  else
	    {
	      /* No scaling */
	      if (SDL_BlitSurface(GFX_k[getpic(h)].k, &src, GFX_lpdest, &dst) < 0) {
		fprintf(stderr, "Could not draw sprite %d: %s\n", getpic(h), SDL_GetError());
		/* If we failed, then maybe the sprite was actually
		   loaded yet, let's try now */
		if (spr[h].pseq != 0)
		  check_seq_status(spr[h].pseq);
	      }
	    }
	}

/* 	if (ddrval != DD_OK) */
/* 	  { */
/* 	    dderror(ddrval); */

/* 	    Msg("MainSpriteDraw(): Could not draw sprite %d, pic %d.",h,getpic(h)); */
/* 	    Msg("Box_crap: %d %d %d %d, Box_real: %d %d %d %d",box_crap.left,box_crap.top, */
/* 		box_crap.right, box_crap.bottom,box_real.left,box_real.top, */
/* 		box_real.right, box_real.bottom); */
/* 	    if (spr[h].pseq != 0) check_seq_status(spr[h].pseq); */
/* 	    break; */
/* 	  } */
/* 	else */
/* 	  { */
/* 	    break; */
/* 	  } */
/*       } */
}

        void changedir( int dir1, int k,int base)
        {
                int hspeed;
                int speed_hold = spr[k].speed;
                if (k > 1) if (spr[k].brain != 9) if (spr[k].brain != 10)
                {

                        //if (mbase_timing > 20) mbase_timing = 20;

                        //   Msg(",base_timing is %d", base_timing);
                        hspeed = spr[k].speed * (base_timing / 4);
                        if (hspeed > 49)
                        {
                                Msg("Speed was %d", hspeed);
                                spr[k].speed = 49;
                        } else
                                spr[k].speed = hspeed;
                }
                int old_seq = spr[k].seq;
                spr[k].dir = dir1;

                if (dir1 == 1)
                {
                        spr[k].mx = (0 - spr[k].speed ) + (spr[k].speed / 3);
                        spr[k].my = spr[k].speed - (spr[k].speed / 3);

                        if (base != -1)
                        {


                                spr[k].seq = base + 1;
                                if (seq[spr[k].seq].active == /*false*/0)
                                {
                                        spr[k].seq = base + 9;

                                }

                        }

                        if (old_seq != spr[k].seq)
                        {
                                spr[k].frame = 0;
                                spr[k].delay = 0;
                        }


                }

                if (dir1 == 2)
                {
                        spr[k].mx = 0;
                        spr[k].my = spr[k].speed;
                        if (base != -1)
                                spr[k].seq = base + 2;

                        if (seq[spr[k].seq].active == /*false*/0) if (seq[base+3].active) spr[k].seq = base +3;
                        if (seq[spr[k].seq].active == /*false*/0) if (seq[base+1].active) spr[k].seq = base +1;


                        if (old_seq != spr[k].seq)
                        {
                                spr[k].frame = 0;
                                spr[k].delay = 0;
                        }


                }
                if (dir1 == 3)
                {
                        spr[k].mx = spr[k].speed - (spr[k].speed / 3);
                        spr[k].my = spr[k].speed - (spr[k].speed / 3);
                        if (base != -1)
                        {
                                spr[k].seq = base + 3;
                                if (seq[spr[k].seq].active == /*false*/0)
                                        spr[k].seq = base + 7;

                        }

                        if (old_seq != spr[k].seq)
                        {
                                spr[k].frame = 0;
                                spr[k].delay = 0;
                        }


                }

                if (dir1 == 4)
                {

                        //Msg("Changing %d to four..",k);
                        spr[k].mx = (0 - spr[k].speed);
                        spr[k].my = 0;
                        if (base != -1)
                                spr[k].seq = base + 4;
                        if (seq[spr[k].seq].active == /*false*/0) if (seq[base+7].active) spr[k].seq = base +7;
                        if (seq[spr[k].seq].active == /*false*/0) if (seq[base+1].active) spr[k].seq = base +1;
                }

                if (dir1 == 6)
                {
                        spr[k].mx = spr[k].speed;
                        spr[k].my = 0;
                        if (base != -1)
                                spr[k].seq = base + 6;

                        if (seq[spr[k].seq].active == /*false*/0) if (seq[base+3].active) spr[k].seq = base +3;
                        if (seq[spr[k].seq].active == /*false*/0) if (seq[base+9].active) spr[k].seq = base +9;

                }

                if (dir1 == 7)
                {
                        spr[k].mx = (0 - spr[k].speed) + (spr[k].speed / 3);
                        spr[k].my = (0 - spr[k].speed)+ (spr[k].speed / 3);
                        if (base != -1)
                        {
                                spr[k].seq = base + 7;


                                if (seq[spr[k].seq].active == /*false*/0)
                                {

                                        spr[k].seq = base + 3;
                                }
                        }

                }
                if (dir1 == 8)
                {
                        spr[k].mx = 0;
                        spr[k].my = (0 - spr[k].speed);
                        if (base != -1)
                                spr[k].seq = base + 8;

                        if (seq[spr[k].seq].active == /*false*/0) if (seq[base+7].active) spr[k].seq = base +7;
                        if (seq[spr[k].seq].active == /*false*/0) if (seq[base+9].active) spr[k].seq = base +9;

                }


                if (dir1 == 9)
                {
                        spr[k].mx = spr[k].speed- (spr[k].speed / 3);
                        spr[k].my = (0 - spr[k].speed)+ (spr[k].speed / 3);
                        if (base != -1)
                        {
                                spr[k].seq = base + 9;
                                if (seq[spr[k].seq].active == /*false*/0)
                                {
                                        spr[k].seq = base + 1;
                                }
                        }
                }



                if (old_seq != spr[k].seq)
                {
                        spr[k].frame = 0;
                        spr[k].delay = 0;
                }


                if (seq[spr[k].seq].active == /*false*/0)
                {
                        //spr[k].mx = 0;
                        //spr[k].my = 0;
                        spr[k].seq = old_seq;

                }

                //Msg("Leaving with %d..", spr[k].dir);

                //Msg("Changedir: Tried to switch sprite %d to dir %d",k,dir1);

                spr[k].speed = speed_hold;

}


void update_play_changes( void )
{
  int j;
        for (j = 1; j < 100; j++)
        {
                if (pam.sprite[j].active)
                        if (play.spmap[*pmap].type[j] != 0)
                        {
                                //lets make some changes, player has extra info
                                if (play.spmap[*pmap].type[j] == 1)
                                {
                                        pam.sprite[j].active = 0;

                                }

                                if (play.spmap[*pmap].type[j] == 2)
                                {
                                        pam.sprite[j].type = 1;
                    pam.sprite[j].hard = 1;
                                }
                                if (play.spmap[*pmap].type[j] == 3)
                                {

                                        //              Msg("Changing sprite %d", j);
                                        pam.sprite[j].type = 0;
                                        pam.sprite[j].hard = 1;

                                }

                                if (play.spmap[*pmap].type[j] == 4)
                                {
                                        pam.sprite[j].type = 1;
                    pam.sprite[j].hard = 0;
                                }

                                if (play.spmap[*pmap].type[j] == 5)
                                {
                                        pam.sprite[j].type = 0;
                    pam.sprite[j].hard = 0;
                                }

                                if (play.spmap[*pmap].type[j] == 6)
                                {
                                        pam.sprite[j].active = 0;

                                }
                                if (play.spmap[*pmap].type[j] == 7)
                                {
                                        pam.sprite[j].active = 0;

                                }
                                if (play.spmap[*pmap].type[j] == 8)
                                {
                                        pam.sprite[j].active = 0;

                                }

                                pam.sprite[j].seq = play.spmap[*pmap].seq[j];
                                pam.sprite[j].frame = play.spmap[*pmap].frame[j];
                                strcpy(pam.sprite[j].script, "");


                        }


        }
}

void update_status_all(void)
{
        /*bool*/int drawexp = /*false*/0;
        int next = next_raise();
    int script;
        if (next != fraise)
        {
                fraise += next / 40;

                if (fraise > next) fraise = next;
                //make noise here
                drawexp = /*true*/1;
                SoundPlayEffect( 13,15050, 0,0 ,0);


        }

        if (*pexper != fexp)
        {
                //update screen experience
                fexp += 10;
                //make noise here

                if (fexp > *pexper) fexp = *pexper;
                drawexp = /*true*/1;
                SoundPlayEffect( 13,29050, 0,0 ,0);

                if (fexp >= fraise)
                {

                        *pexper -= next;
                        fexp = 0;

                        script = load_script("lraise", 1, /*false*/0);
                        if (locate(script, "raise")) run_script(script);
                }
        }



        if (drawexp)
        {


                draw_exp();
        }


        if ( (flifemax != *plifemax) || (flife != *plife) )
        {
                if (flifemax < *plifemax) flifemax++;
                if (flifemax > *plifemax) flifemax--;
                if (flife > *plife) flife--;
                if (flife < *plife) flife++;
                if (flife > *plife) flife--;
                if (flife < *plife) flife++;
                draw_bar(flifemax, 190);
                draw_bar(flife, 451);
        }

        if ( fstrength != *pstrength)
        {
                if (fstrength < *pstrength) fstrength++;
                if (fstrength > *pstrength) fstrength--;
                SoundPlayEffect( 22,22050, 0,0 ,0);

                draw_strength();
        }

        if ( fdefense != *pdefense)
        {
                if (fdefense < *pdefense) fdefense++;
                if (fdefense > *pdefense) fdefense--;
                SoundPlayEffect( 22,22050, 0,0 ,0);
                draw_defense();
        }
        if ( fmagic != *pmagic)
        {
                if (fmagic < *pmagic) fmagic++;
                if (fmagic > *pmagic) fmagic--;
                SoundPlayEffect( 22,22050, 0,0 ,0);
                draw_magic();
        }

        if (fgold != *pgold)
        {
                if (fgold < *pgold)
                {
                        fgold += 20;
                        if (fgold > *pgold) fgold = *pgold;
                }

                if (fgold > *pgold)
                {
                        fgold -= 20;
                        if (fgold < *pgold) fgold = *pgold;
                }
                SoundPlayEffect( 14,22050, 0,0 ,0);
                draw_gold();
        }

        if (*pmagic_level < *pmagic_cost)
        {
                if (item_screen == /*false*/0)
                        *pmagic_level += *pmagic;
                if (*pmagic_level > *pmagic_cost) *pmagic_level = *pmagic_cost;
        }
        if (*pmagic_cost > 0) if (*pmagic_level > 0)
        {
                int mnum = *pmagic_level / (*pmagic_cost / 100);
                if (mnum != last_magic_draw)
                {

                        draw_mlevel(mnum);

                        //draw_status_all();
                        last_magic_draw = mnum;


                }
        }


        spr[1].strength = fstrength;
        spr[1].defense = fdefense;


        if (flife < 1)
        {
                script = load_script("dinfo", 1000, /*false*/0);
                if (locate(script, "die")) run_script(script);
        }

}



/* used by gfx_tiles only */
void place_sprites_game(void)
{
        int sprite;

        /*BOOL*/int bs[MAX_SPRITES_AT_ONCE];
        int rank[MAX_SPRITES_AT_ONCE];
        int highest_sprite;

        update_play_changes();

        memset(&bs,0,sizeof(bs));
        int hs;
        int r1;
        int j;
	int oo;

        for (r1 = 1; r1 < 100; r1++)
        {
	  int h1;
                highest_sprite = 20000; //more than it could ever be

                rank[r1] = 0;

                for (h1 = 1; h1 < 100;  h1++)
                {
                        if (bs[h1] == /*FALSE*/0)
                        {
                                if (pam.sprite[h1].active)
                                {
                                        if (pam.sprite[h1].que != 0) hs = pam.sprite[h1].que; else hs = pam.sprite[h1].y;
                                        if ( hs < highest_sprite )
                                        {
                                                highest_sprite =hs;
                                                rank[r1] = h1;
                                        }
                                }

                        }
                }
                if (rank[r1] != 0)
                        bs[rank[r1]] = /*TRUE*/1;
        }



        for (oo = 1; rank[oo] > 0; oo++)
        {
                //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
                j = rank[oo];



                if (pam.sprite[j].active == /*true*/1) if ( ( pam.sprite[j].vision == 0) || (pam.sprite[j].vision == *pvision))
                {



                        check_seq_status(pam.sprite[j].seq);

                        //we have instructions to make a sprite
                        if (  (pam.sprite[j].type == 0)  | (pam.sprite[j].type == 2) )

                        {
                                //make it part of the background (much faster)

                                sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y,0,
                                        pam.sprite[j].seq,pam.sprite[j].frame,
                                        pam.sprite[j].size);
                                //Msg("Background sprite %d has hard of %d..", j, pam.sprite[j].hard);
                                spr[sprite].hard = pam.sprite[j].hard;

                                spr[sprite].sp_index = j;

                                rect_copy(&spr[sprite].alt , &pam.sprite[j].alt);


                                check_sprite_status_full(sprite);
                                if (pam.sprite[j].type == 0)
				  draw_sprite_game(GFX_lpDDSTwo, sprite);


                                if (spr[sprite].hard == 0)
                                {
                                /*if (pam.sprite[j].prop == 0)
                                        add_hardness(sprite, 1); else */ add_hardness(sprite,100+j);
                                }
                                spr[sprite].active = /*false*/0;
                        }

                        if (pam.sprite[j].type == 1)
                        {
                                //make it a living sprite


                                sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y,0,
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
                if (pam.sprite[j].prop == 0) if (spr[sprite].sound != 0)
                                {
                                        //make looping sound
                                        Msg("making sound with sprite %d..", sprite);
                                        SoundPlayEffect( spr[sprite].sound,22050, 0,sprite, 1);


                                }
                                if (spr[sprite].brain == 3)
                                {

                                        check_seq_status(21);
                                        check_seq_status(23);
                                        check_seq_status(24);
                                        check_seq_status(26);
                                        check_seq_status(27);
                                        check_seq_status(29);
                                        check_seq_status(111);
                                        check_seq_status(113);
                                        check_seq_status(117);
                                        check_seq_status(119);
                                        check_seq_status(121);
                                        check_seq_status(123);
                                        check_seq_status(127);
                                        check_seq_status(129);

                                }


                                if (spr[sprite].hard == 0)
                                {
                                /*              if (pam.sprite[j].prop == 0)
                                        add_hardness(sprite, 1); else */ add_hardness(sprite,100+j);


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





/*bool*/int kill_last_sprite(void)
{
  int found = 0;
  /*bool*/int nosetlast = /*false*/0;
  int k;
  for (k=1; k < MAX_SPRITES_AT_ONCE; k++ )
    {
      if (spr[k].active)
        {
          if (spr[k].live)
            {
              nosetlast = /*true*/1;
            }
          else
            {
              found = k;
            }
        }
    }

  if (found > 1)
    {
      spr[found].active = /*FALSE*/0;
      if (nosetlast == /*false*/0)
	last_sprite_created = found - 1;
      return(/*true*/1);
    }

  //we didn't kill any sprites, only 1 remains
  return(/*false*/0);
}


void show_bmp( char name[80], int showdot, int reserved, int script)
{
  SDL_Surface *image = NULL;

  if (!exist(name))
    {
      Msg("Error: Can't find bitmap at %s.",name);
      return;
    }

  // memory leak?
  //lpDDSTrick = DDLoadBitmap(lpDD, name, 0, 0);

/*   lpDDPal = DDLoadPalette(lpDD, name); */

/*   if (lpDDPal) */
/*     lpDDSPrimary->SetPalette(lpDDPal); */

  image = load_bmp_setpal(name);
  if (image == NULL)
    {
      fprintf(stderr, "Couldn't load %s\n", name);
      return;
    }

  showb.active = /*true*/1;
  showb.showdot = showdot;
  showb.script = script;

  // After show_bmp(), and before the flip_it() call in updateFrame(),
  // other parts of the code will draw sprites on lpDDSBack and mess
  // the showbmp(). So skip the next flip_it().
  abort_this_flip = /*true*/1;

/*   RECT rcRect; */
/*   SetRect(&rcRect, 0,0,640, 480); */

/*  again: */
/*   ddrval = lpDDSBack->BltFast( 0, 0, lpDDSTrick, */
/* 			       &rcRect, DDBLTFAST_NOCOLORKEY); */
/*   if( ddrval == DDERR_WASSTILLDRAWING ) goto again; */

  // GFX
  {
    SDL_BlitSurface(image, NULL, GFX_lpDDSTrick, NULL);
    SDL_FreeSurface(image);
  }

  /* DEBUG: doesn't seem useful, will be done in the next
     updateFrame() anyway */
  //flip_it_second();
}


/* Used to implement DinkC's copy_bmp_to_screen(). Difference with
   show_cmp: does not set showb.* (wait for button), install the image
   to lpDDSTwo (background) and not lpDDSBack (screen double
   buffer) */
void copy_bmp( char name[80])
{
  SDL_Surface *image = NULL;

  if (!exist(name))
    {
      Msg("Error: Can't find bitmap at %s.",name);
      return;
    }

  // memory leak?
  //lpDDSTrick = DDLoadBitmap(lpDD, name, 0, 0);

/*   lpDDPal = DDLoadPalette(lpDD, name); */

/*   if (lpDDPal) */
/*     lpDDSPrimary->SetPalette(lpDDPal); */

  image = load_bmp_setpal(name);
  if (image == NULL)
    {
      fprintf(stderr, "Couldn't load %s\n", name);
      return;
    }


  abort_this_flip = /*true*/1;

/*   RECT rcRect; */
/*   SetRect(&rcRect, 0,0,640, 480); */


/*  again: */
/*   ddrval = lpDDSBack->BltFast( 0, 0, lpDDSTrick, */
/* 			       &rcRect, DDBLTFAST_NOCOLORKEY); */
/*   if( ddrval == DDERR_WASSTILLDRAWING ) goto again; */
/*  again1: */
  // Beuc: why copy the image twice?
/*   ddrval = lpDDSTwo->BltFast( 0, 0, lpDDSTrick, */
/* 			      &rcRect, DDBLTFAST_NOCOLORKEY); */
/*   if( ddrval == DDERR_WASSTILLDRAWING ) goto again1; */
  // GFX
  {
    SDL_BlitSurface(image, NULL, GFX_lpDDSTwo, NULL);
    SDL_FreeSurface(image);
  }

  // DEBUG: disabled because it seems useless
  //flip_it_second();
}

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

        int hurt_thing(int h, int damage, int special)
        {
                //lets hurt this sprite but good
                if (damage < 1) return(0);
                int num = damage - spr[h].defense;

                //      Msg("num is %d.. defense was %d.of sprite %d", num, spr[h].defense, h);
                if (num < 1) num = 0;

                if (num == 0)
                {
                        if ((rand() % 2)+1 == 1) num = 1;
                }

                spr[h].damage += num;
                return(num);
                //draw blood here
        }

        void random_blood(int mx, int my, int h)
        {
                int myseq;
                if ((rand() % 2) == 1) myseq = 188; else myseq = 187;

                int crap2 = add_sprite(mx,my,5,myseq,1);
                spr[crap2].speed = 0;
                spr[crap2].base_walk = -1;
                spr[crap2].nohit = 1;
                spr[crap2].seq = myseq;
                if (h > 0)
                        spr[crap2].que = spr[h].y+1;

        }








        void place_sprites_game_background(void )
        {
                int sprite;

                /*BOOL*/int bs[MAX_SPRITES_AT_ONCE];
                int rank[MAX_SPRITES_AT_ONCE];
                int highest_sprite;
                memset(&bs,0,sizeof(bs));
                int hs;
		int r1;
                for (r1 = 1; r1 < 100; r1++)
                {
		  int h1;
                        highest_sprite = 20000; //more than it could ever be

                        rank[r1] = 0;

                        for (h1 = 1; h1 < 100;  h1++)
                        {
                                if (bs[h1] == /*FALSE*/0)
                                {
                                        if (pam.sprite[h1].active) if (pam.sprite[h1].type == 0)
                                        {
                                                if (pam.sprite[h1].que != 0) hs = pam.sprite[h1].que; else hs = pam.sprite[h1].y;
                                                if ( hs < highest_sprite )
                                                {
                                                        highest_sprite =hs;
                                                        rank[r1] = h1;
                                                }
                                        }

                                }
                        }
                        if (rank[r1] != 0)
                                bs[rank[r1]] = /*TRUE*/1;
                }




                int j;
                int oo;

                for (oo =1; rank[oo] > 0; oo++)
                {
                        //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
                        j = rank[oo];

                        if (pam.sprite[j].active == /*true*/1) if ( ( pam.sprite[j].vision == 0) || (pam.sprite[j].vision == *pvision))
                        {

                                check_seq_status(pam.sprite[j].seq);

                                //we have instructions to make a sprite
                                if (  pam.sprite[j].type == 0  )

                                {
                                        //make it part of the background (much faster)

                                        sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y,0,
                                                pam.sprite[j].seq,pam.sprite[j].frame,
                                                pam.sprite[j].size);
                                        //Msg("Background sprite %d has hard of %d..", j, pam.sprite[j].hard);
                                        check_sprite_status_full(sprite);
                                        draw_sprite_game(GFX_lpDDSTwo, sprite);
                                        spr[sprite].active = /*false*/0;
                                }
                        }
                }
        }

        void fill_back_sprites(void )
        {
                int sprite;

                /*BOOL*/int bs[MAX_SPRITES_AT_ONCE];
                int rank[MAX_SPRITES_AT_ONCE];
                int highest_sprite;


                memset(&bs,0,sizeof(bs));
                int hs;
                int r1;
                for (r1 = 1; r1 < 100; r1++)
                {
		  int h1;
                        highest_sprite = 20000; //more than it could ever be

                        rank[r1] = 0;

                        for (h1 = 1; h1 < 100;  h1++)
                        {
                                if (bs[h1] == /*FALSE*/0)
                                {
                                        if (pam.sprite[h1].active) if (pam.sprite[h1].type != 1) if (pam.sprite[h1].hard == 0)
                                        {
                                                if (pam.sprite[h1].que != 0) hs = pam.sprite[h1].que; else hs = pam.sprite[h1].y;
                                                if ( hs < highest_sprite )
                                                {
                                                        highest_sprite =hs;
                                                        rank[r1] = h1;
                                                }
                                        }

                                }
                        }
                        if (rank[r1] != 0)
                                bs[rank[r1]] = /*TRUE*/1;
                }




                int j;
                int oo;
                for (oo =1; rank[oo] > 0; oo++)
                {
                        //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
                        j = rank[oo];

                        if (pam.sprite[j].active == /*true*/1) if ( ( pam.sprite[j].vision == 0) || (pam.sprite[j].vision == *pvision))
                        {
                                {
                                        //make it part of the background (much faster)

                                        sprite = add_sprite_dumb(pam.sprite[j].x,pam.sprite[j].y,0,
                                                pam.sprite[j].seq,pam.sprite[j].frame,
                                                pam.sprite[j].size);
                                        spr[sprite].hard = pam.sprite[j].hard;
                                        spr[sprite].sp_index = j;
                                        rect_copy(&spr[sprite].alt , &pam.sprite[j].alt);
                                        check_sprite_status_full(sprite);
                                        if (spr[sprite].hard == 0)
                                        {
                                        /*if (pam.sprite[j].prop == 0)
                                                add_hardness(sprite, 1); else */ add_hardness(sprite,100+j);

                                        }
                                        spr[sprite].active = /*false*/0;
                                }


                        }

                }


        }



        void add_item(char name[10], int mseq, int mframe, /*bool*/int magic)
        {
                if (magic == /*false*/0)
                {
                        //add reg item
		  int i;
                        for (i = 1; i < 17; i ++)
                        {
                                if (play.item[i].active == /*false*/0)
                                {
                                        if (debug_mode)
                                                Msg("Weapon/item %s added to inventory.",name);
                                        play.item[i].seq = mseq;
                                        play.item[i].frame = mframe;
                                        strcpy(play.item[i].name, name);
                                        play.item[i].active = /*true*/1;

                                        int crap1 = load_script(play.item[i].name, 1000, /*false*/0);
                                        if (locate(crap1, "PICKUP")) run_script(crap1);

                                        return;
                                }
                        }

                } else
                {
                        //add magic item
		  int i;
                        for (i = 1; i < 9; i ++)
                        {
                                if (play.mitem[i].active == /*false*/0)
                                {
                                        if (debug_mode)
                                                Msg("Magic %s added to inventory.",name);
                                        play.mitem[i].seq = mseq;
                                        play.mitem[i].frame = mframe;
                                        strcpy(play.mitem[i].name, name);

                                        play.mitem[i].active = /*true*/1;

                                        int crap = load_script(play.mitem[i].name, 1000, /*false*/0);
                                        if (locate(crap, "PICKUP")) run_script(crap);

                                        return;
                                }
                        }


                }
        }

        void fill_screen(int num)
        {
/*                 int crap;    */
/*                 DDBLTFX     ddbltfx; */
/*                 ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*                 ddbltfx.dwSize = sizeof( ddbltfx); */
/*                 ddbltfx.dwFillColor = num; */
/*                 crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
                // GFX
		{
		  /* Warning: palette indexes 0 and 255 are hard-coded
		     to black and white (change_screen_palette). */
 		  SDL_FillRect(GFX_lpDDSTwo, NULL, num);
		  // TODO: when using fill_screen() while the main palette was changed, which color index is used?
		}
        }


        void fill_hard_sprites(void )
        {
                /*BOOL*/int bs[MAX_SPRITES_AT_ONCE];
                int rank[MAX_SPRITES_AT_ONCE];
                int highest_sprite;
                int h;
		int j;
                memset(&bs,0,sizeof(bs));

                //Msg("filling sprite hardness...");


                int max_s = last_sprite_created;

                int height;
                int r1;
                for (r1 = 1; r1 <= max_s; r1++)
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



                for ( j = 1; j <= max_s; j++)
                {
                        h = rank[j];
                        if (h > 0)
                                if (spr[h].active)
                                {
                                        //                      Msg("proccesing sprite %d", h);
                                        if (spr[h].sp_index != 0)
                                        {
                                                //Msg("has spindex of %d prop is %d",spr[h].sp_index,pam.sprite[spr[h].sp_index].prop);
                                                if (pam.sprite[spr[h].sp_index].hard == 0)
                                                {

                                                        add_hardness(h,100+spr[h].sp_index);
                                                        //Msg("added warp hardness for %d", spr[h].sp_index);
                                                }



                                        } else
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




/**
 * Return values:
 * - 0: continue processing script
 * - 2: stop processing script (may come back later via callbacks)
 * - 3 & 4: have something to do with if/else ?
 **/
        int process_line (int script, char *s, /*bool*/int doelse)
        {
                char * h, *p;
                int i;
                char line[200];
                char ev[15][100];
                char temp[100];
                char first[2];
                int sprite = 0;
                int kk;

                if (rinfo[script]->level < 1) rinfo[script]->level = 1;


                for (kk = 1; kk < 15; kk++)
		  ev[kk][0] = 0;
                h = s;
                if (h[0] == 0) return(0);
                if (  (h[0] == '/') && (h[1] == '/'))

                {
                        //Msg("It was a comment!");
                        goto bad;
                }



                for ( i=1; i <= 14; i++)
                {
                        if (separate_string(h, i,' ',ev[i]) == /*false*/0) goto pass;
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
                        return(2);
                }
                //replace("\n","",ev[1]);
                if (ev[1][strlen(ev[1]) -1] == ':') if (strlen(ev[2]) < 2)

                {
                        //      Msg("Found label %s..",ev[1]);
                        return(0); //its a label
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
                         * multi-character cosntants should be
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

                                strcpy(s, h);

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

    sprintf(first, "%c",ev[1][0]);



        if (compare(first, "{"))
        {


                rinfo[script]->level++;
                //Msg("Went up level, now at %d.", rinfo[script]->level);
                h = &h[1];
                if (rinfo[script]->skipnext)
                {
                        rinfo[script]->skipnext = /*false*/0;
                        rinfo[script]->onlevel = ( rinfo[script]->level - 1);
                        //Msg("Skipping until level %d is met..", rinfo[script]->onlevel);

                }
                goto good;
    }

        if (compare(first, "}"))
        {
                rinfo[script]->level--;
                //Msg("Went down a level, now at %d.", rinfo[script]->level);
                h = &h[1];

                if (rinfo[script]->onlevel > 0) if (rinfo[script]->level == rinfo[script]->onlevel)
                {
                        strip_beginning_spaces(h);

                        strcpy(s, h);
                        return(4);
                }
                goto good;
        }




        if (rinfo[script]->level < 0)
        {
                rinfo[script]->level = 0;
        }

        if (compare(ev[1], "void"))
        {
                //     Msg("Next procedure starting, lets quit");
                strcpy(s, h);
                if (rinfo[script]->proc_return != 0)
                {
                        run_script(rinfo[script]->proc_return);
                        kill_script(script);
                }

                return(2);
        }


        { //used to be an if..


                if (rinfo[script]->onlevel > 0)
                {
                        if (rinfo[script]->level > rinfo[script]->onlevel) return(0);

                }
                rinfo[script]->onlevel = 0;

                if (rinfo[script]->skipnext)
                {
                        //sorry, can't do it, you were told to skip the next thing
                        rinfo[script]->skipnext = /*false*/0;
                        strcpy(s, h);
                        return(3);
        }

                //if (debug_mode) Msg("%s",s);


                if (compare(ev[1], "void"))
                {
                        Msg("ERROR: Missing } in %s, offset %d.", rinfo[script]->name,rinfo[script]->current);
                        strcpy(s, h);
                        return(2);
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
                        strcpy(s, h);
                        return(1);

                }

                if (compare(ev[1], "unfreeze"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                //Msg("UnFreeze called for %d.", nlist[0]);
                                if (spr[nlist[0]].active) spr[nlist[0]].freeze = 0; else
                                        Msg("Couldn't unfreeze sprite %d in script %d, it doesn't exist.", nlist[0], script);

                        }

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "freeze"))
                {
                        //Msg("Freeze called (%s)", h);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                if (spr[nlist[0]].active) spr[nlist[0]].freeze = script; else
                                        Msg("Couldn't freeze sprite %d in script %d, it doesn't exist.", nlist[0], script);

                        }

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "set_callback_random"))
                {
                        Msg("setting callback random");
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,1,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                add_callback(slist[0],nlist[1],nlist[2],script);
                                //got all parms, let do it
                        }

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "set_dink_speed"))
                {
                        Msg("setting callback random");
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                dinkspeed = nlist[0];

                        }

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "reset_timer"))
                {
                        h = &h[strlen(ev[1])];
                        time(&time_start);
                        play.minutes = 0;
                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "set_keep_mouse"))
                {
                        Msg("setting callback random");
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                keep_mouse = nlist[0];

                        }

                        strcpy(s, h);
                        return(0);
                }




                if (compare(ev[1], "add_item"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,1,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                add_item(slist[0], nlist[1], nlist[2], /*false*/0);
                        }

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "add_exp"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                add_exp(nlist[0], nlist[1]);
                        }

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "add_magic"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,1,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                add_item(slist[0], nlist[1], nlist[2], /*true*/1);
                        }

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "kill_this_item"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                kill_cur_item_script(slist[0]);
                        }

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "kill_this_magic"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                kill_cur_magic_script(slist[0]);
                        }

                        strcpy(s, h);
                        return(0);
                }



                if (compare(ev[1], "show_bmp"))
                {
                        Msg("showing BMP");
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,1,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                wait.active = /*false*/0;
                                show_bmp(slist[0], nlist[1], nlist[2], script);
                        }

                        strcpy(s, h);
                        return(2);
                }


                if (compare(ev[1], "wait_for_button"))
                {
                        Msg("waiting for button with script %d", script);
                        h = &h[strlen(ev[1])];
                        strcpy(s, h);
                        wait.script = script;
                        wait.active = /*true*/1;
                        wait.button = 0;
                        return(2);
                }

                if (compare(ev[1], "stop_wait_for_button"))
                {
                        wait.active = /*false*/0;

                        return(0);
                }


                if (compare(ev[1], "copy_bmp_to_screen"))
                {
                        Msg("copying BMP");
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                copy_bmp(slist[0]);

                        }

                        strcpy(s, h);
                        return(0);
                }




                if (compare(ev[1], "say"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                if (nlist[1] == 0)
                                {
                                        Msg("Say_stop error:  Sprite 0 can talk? Yeah, didn't think so.");
                                        return(0);
                                }

                                if (nlist[1] != 1000)
                                        kill_text_owned_by(nlist[1]);
                                decipher_string(slist[0], script);
                                returnint = say_text(slist[0], nlist[1], script);
                                //Msg("Just said %s.", slist[0]);
                        }

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "draw_screen"))
                {

                        if (rinfo[script]->sprite == 1000)
                        {
                                draw_map_game();
                                return(0);
                        }
                        draw_map_game();
                        return(2);
                }


                if (compare(ev[1], "free_items"))
                {
		  int i;
                        returnint = 0;
                        for (i = 1; i < 17; i ++)
                        {
                                if (play.item[i].active == /*false*/0)
                                {
                                        returnint += 1;
                                }
                        }
                        return(0);
                }


                if (compare(ev[1], "kill_cur_item"))
                {
                        returnint = 0;
                        kill_cur_item();
                        return(2);
                }



                if (compare(ev[1], "kill_cur_magic"))
                {
                        returnint = 0;
                        kill_cur_magic();
                        return(2);
                }



                if (compare(ev[1], "free_magic"))
                {
		  int i;
                        returnint = 0;

                        for (i = 1; i < 9; i ++)
                        {
                                if (play.mitem[i].active == /*false*/0)
                                {
                                        returnint += 1;
                                }
                        }
                        return(0);
                }




                if (compare(ev[1], "draw_status"))
                {
                        draw_status_all();
                        return(0);
                }


                if (compare(ev[1], "arm_weapon"))
                {

                        if (weapon_script != 0) if (locate(weapon_script, "DISARM")) run_script(weapon_script);
                        weapon_script = load_script(play.item[*pcur_weapon].name, 1000, /*false*/0);
                        if (locate(weapon_script, "ARM")) run_script(weapon_script);


                        return(0);
                }

                if (compare(ev[1], "arm_magic"))
                {


                        if (magic_script != 0) if (locate(magic_script, "DISARM")) run_script(magic_script);
                        magic_script = load_script(play.mitem[*pcur_magic].name, 1000, /*false*/0);
                        if (locate(magic_script, "ARM")) run_script(magic_script);

                        return(0);
                }


                if (compare(ev[1], "load_screen"))
                {
                        //Msg("Loading map %d..",*pmap);
                        update_screen_time();
                        load_map(map.loc[*pmap]);
                        return(0);
                }


                if (compare(ev[1], "choice_start"))
                {

                        kill_text_owned_by(1);
                        if (talk_get(script))
                        {

                                //              Msg("Question gathered successfully.");
                                return(2);
                        }

                        return(0);
                }


                if (compare(ev[1], "say_stop"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                if (nlist[1] == 0)
                                {
                                        Msg("Say_stop error:  Sprite 0 can talk? Yeah, didn't think so.");
                                        return(0);
                                }

                                kill_text_owned_by(nlist[1]);
                                kill_text_owned_by(1);
                                kill_returning_stuff(script);

                                decipher_string(slist[0], script);
                                sprite = say_text(slist[0], nlist[1], script);
                                returnint = sprite;
                                spr[sprite].callback = script;
                                play.last_talk = script;
                                //Msg("Sprite %d marked callback true.", sprite);

                                strcpy(s, h);

                                return(2);

                        }

                        strcpy(s, h);
                        return(0);
                }




                if (compare(ev[1], "say_stop_npc"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                if (text_owned_by(nlist[1]))
                                {
                                        returnint = 0;
                                        return(0);
                                }

                                kill_returning_stuff(script);
                                decipher_string(slist[0], script);
                                sprite = say_text(slist[0], nlist[1], script);
                                returnint = sprite;
                                spr[sprite].callback = script;
                                strcpy(s, h);

                                return(2);

                        }

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "say_stop_xy"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,1,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                Msg("Say_stop_xy: Adding %s", slist[0]);
                                kill_returning_stuff(script);
                                decipher_string(slist[0], script);
                                sprite = say_text_xy(slist[0], nlist[1], nlist[2], script);
                                spr[sprite].callback = script;
                                spr[sprite].live = /*true*/1;
                                play.last_talk = script;
                                strcpy(s, h);

                                return(2);

                        }

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "say_xy"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,1,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                kill_returning_stuff(script);
                                decipher_string(slist[0], script);
                                sprite = say_text_xy(slist[0], nlist[1], nlist[2], script);
                                returnint = sprite;
                                strcpy(s, h);
                                return(0);

                        }

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "restart_game"))
                {
		  int u;
		  int script;
                        while (kill_last_sprite());
                        kill_repeat_sounds_all();
                        kill_all_scripts_for_real();
                        mode = 0;
                        kill_all_vars();
                        memset(&hm, 0, sizeof(hm));
                        for (u = 1; u <= 10; u++)
                                play.button[u] = u;
                        script = load_script("main", 0, /*true*/1);

                        locate(script, "main");
                        run_script(script);
                        //lets attach our vars to the scripts
                        attach();
                        return(2);
                }

                if (compare(ev[1], "wait"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                //               Msg("Wait called for %d.", nlist[0]);
                                strcpy(s, h);
                                kill_returning_stuff(script);
                                add_callback("",nlist[0],0,script);

                                return(2);
                        }

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "preload_seq"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                check_seq_status(nlist[0]);
                        }

                        strcpy(s, h);
                        return(0);
                }




                if (compare(ev[1], "script_attach"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                rinfo[script]->sprite = nlist[0];
                        }
                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "draw_hard_sprite"))
                {

                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                update_play_changes();
                                int l = nlist[0];
                                rect mhard;
                                rect_copy(&mhard, &k[seq[spr[l].pseq].frame[spr[l].pframe]].hardbox);
                                rect_offset(&mhard, (spr[l].x- 20), spr[l].y);

                                fill_hardxy(mhard);
                                fill_back_sprites();
                                fill_hard_sprites();


                        }
                        strcpy(s, h);
                        return(0);
                }


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

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "sp_script"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,2,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                if (nlist[0] == 0)
                                {
                                        Msg("Error: sp_script cannot process sprite 0??");
                                        return(0);
                                }
                                kill_scripts_owned_by(nlist[0]);
                                if (load_script(slist[1], nlist[0], /*true*/1) == 0)
                                {
                                        returnint = 0;
                                        return(0);
                                }
                                if (no_running_main == /*true*/1) Msg("Not running %s until later..", rinfo[spr[nlist[0]].script]->name);

                                if (no_running_main == /*false*/0)
                                        locate(spr[nlist[0]].script, "MAIN");


                                int tempreturn = spr[nlist[0]].script;

                                if (no_running_main == /*false*/0)
                                        run_script(spr[nlist[0]].script);


                                returnint = tempreturn;
                        }

                        strcpy(s, h);
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

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "run_script_by_number"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,2,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                locate(nlist[0], slist[1]);

                                run_script(nlist[0]);

                        }

                        strcpy(s, h);
                        return(0);
                }



                if (compare(ev[1], "draw_hard_map"))
                {
                        // (sprite, direction, until, nohard);
                        Msg("Drawing hard map..");
                        update_play_changes();
                        fill_whole_hard();
                        fill_hard_sprites();
                        fill_back_sprites();
                        strcpy(s, h);
                        return(0);
                }




                if (compare(ev[1], "draw_background"))
                {
                        // (sprite, direction, until, nohard);
                        draw_map_game_background();
                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "fade_down"))
                {
                        // (sprite, direction, until, nohard);
                        process_downcycle = /*true*/1;
                        cycle_clock = thisTickCount+1000;
                        cycle_script = script;
                        return(2);
                }
                if (compare(ev[1], "fade_up"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        process_upcycle = /*true*/1;
                        cycle_script = script;

                        strcpy(s, h);
                        return(2);
                }


                if (compare(ev[1], "kill_this_task"))
                {
                        // (sprite, direction, until, nohard);
                        if (rinfo[script]->proc_return != 0)
                        {
                                run_script(rinfo[script]->proc_return);
                        }
                        kill_script(script);
                        return(2);
                }

                if (compare(ev[1], "kill_game"))
                {
                        // (sprite, direction, until, nohard);

                        Msg("Was told to kill game, so doing it like a good boy.");
                //      PostMessage(hWndMain, WM_CLOSE, 0, 0);
                        finiObjects();
                        return(2);
                }


                if (compare(ev[1], "playavi"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                //DSDisable();


                                //ShowWindow(hWndMain, SW_MINIMIZE);

			  /* TODO: portable replacement */
/*                                 int     myreturn =      _spawnl(_P_WAIT, "..\\dplay.exe","..\\dplay.exe",slist[0], NULL ); */

//                                 Msg("Return is %d", myreturn);
                                //ShowWindow(hWndMain, SW_MAXIMIZE);

                                //SetFocus(hWndMain);
                        }
                        //InitSound(hWndMain);
                        strcpy(s, h);
                        return(2);

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
                                      return(2);

                                    Msg("Playing CD track %d.", cd_track);
                                    if (PlayCD(cd_track) >= 0)
                                      {
                                        strcpy(s, h);
                                        return(0);
                                      }
                                  }
                              }
                            Msg("Playing midi %s.", slist[0]);
                            PlayMidi(slist[0]);
                          }
                        strcpy(s, h);
                        return(0);
                }
                if (compare(ev[1], "stopmidi"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        StopMidi();
                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "kill_all_sounds"))
                {
            kill_repeat_sounds_all();
                        strcpy(s, h);
                        return(0);

                }

                if (compare(ev[1], "turn_midi_off"))
                {
                        midi_active = /*false*/0;
                        strcpy(s, h);
                        return(0);

                }
                if (compare(ev[1], "turn_midi_on"))
                {
                        midi_active = /*true*/1;
                        strcpy(s, h);
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

                        strcpy(s, h);
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

                        strcpy(s, h);
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

                  strcpy(s, h);
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

                  strcpy(s, h);
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

                        strcpy(s, h);
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

                        strcpy(s, h);
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

                        strcpy(s, h);
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
                                return(2);
                        }

                        strcpy(s, h);
                        return(0);
                }

                if (compare(ev[1], "game_exist"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                sprintf(temp, "save%ld.dat",nlist[0]);
                                if (exist(temp)) returnint = 1; else returnint = 0;

                        }

                        strcpy(s, h);
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
                                strcpy(s, h);
                                if (debug_mode) Msg("Move_stop: Sprite %d, dir %d, num %d", nlist[0],nlist[1], nlist[2]);
                                return(2);

                        }

                        strcpy(s, h);
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

                        strcpy(s, h);
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

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "goto"))
                {

                        locate_goto(ev[2], script);
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

                        strcpy(s, h);
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
                                strcpy(s, h);
                                return(4);
                        }

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

                        strcpy(s, h);
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

                        strcpy(s, h);
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

                        strcpy(s, h);
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

                        strcpy(s, h);
                        return(0);
                }


                if (compare(ev[1], "get_version"))
                {
                        h = &h[strlen(ev[1])];
                        returnint = dversion;
                        strcpy(s, h);
                        return(0);
                }

                /* Used in the original game to choose between 2 CD
                   tracks/sounds. "This command is included for
                   compatibility" (TM) */
                if (compare(ev[1], "get_burn"))
                {
                        h = &h[strlen(ev[1])];
                        returnint = 1;
                        strcpy(s, h);
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

                        strcpy(s, h);
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

                        strcpy(s, h);
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



                if (compare(ev[1], "sp_speed"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].speed);

                                if (nlist[1] != -1) changedir(spr[nlist[0]].dir, nlist[0], spr[nlist[0]].base_walk);

                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_range"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].range);

                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_nocontrol"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].nocontrol);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_nodraw"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].nodraw);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_picfreeze"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].picfreeze);
                                return(0);
                        }
                        returnint =  -1;
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



                if (compare(ev[1], "sp_sound"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].sound);

                                if (nlist[1] != 0)
                                {
                                        SoundPlayEffect( spr[nlist[0]].sound,22050, 0,nlist[0], 1);

                                }
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_attack_wait"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                returnint = change_sprite(nlist[0], nlist[1]+thisTickCount, &spr[nlist[0]].attack_wait);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_active"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].active);


                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_disabled"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {

                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].disabled);


                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_size"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].size);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }
                if (compare(ev[1], "activate_bow"))
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

                        return(2);
                }

                if (compare(ev[1], "get_last_bow_power"))
                {


                        returnint = bow.last_power;
                        return(0);
                }


                if (compare(ev[1], "sp_que"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].que);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

        if (compare(ev[1], "sp_gold"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].gold);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }



                if (compare(ev[1], "sp_base_walk"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite_noreturn(nlist[0], nlist[1], &spr[nlist[0]].base_walk);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_target"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].target);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "stopcd"))
                {
                        // mciSendCommand(CD_ID, MCI_CLOSE, 0, NULL);
                        Msg("Stopped cd");
                        killcd();
                        return(0);
                }


                if (compare(ev[1], "sp_base_hit"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite_noreturn(nlist[0], nlist[1], &spr[nlist[0]].base_hit);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_base_attack"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite_noreturn(nlist[0], nlist[1], &spr[nlist[0]].base_attack);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_base_idle"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite_noreturn(nlist[0], nlist[1], &spr[nlist[0]].base_idle);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if ( (compare(ev[1], "sp_base_die")) || (compare(ev[1], "sp_base_death"))  )
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite_noreturn(nlist[0], nlist[1], &spr[nlist[0]].base_die);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "disable_all_sprites"))
                {
		  int jj;
                        for (jj = 1; jj < last_sprite_created; jj++)
                                if (spr[jj].active) spr[jj].disabled = /*true*/1;
                                return(0);
                }
                if (compare(ev[1], "enable_all_sprites"))
                {
		  int jj;
                        for (jj = 1; jj < last_sprite_created; jj++)
                                if (spr[jj].active) spr[jj].disabled = /*false*/0;
                                return(0);
                }


                if (compare(ev[1], "sp_pseq"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].pseq);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_pframe"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].pframe);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_seq"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].seq);
                                return(0);
                        }
                        returnint =  -1;
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



                if (compare(ev[1], "sp_editor_num"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = spr[nlist[0]].sp_index;
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_brain"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].brain);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_exp"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].exp);
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


                if (compare(ev[1], "sp_reverse"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].reverse);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_noclip"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].noclip);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_touch_damage"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite_noreturn(nlist[0], nlist[1], &spr[nlist[0]].touch_damage);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }



                if (compare(ev[1], "sp_brain_parm"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].brain_parm);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }
                if (compare(ev[1], "sp_brain_parm2"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].brain_parm2);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_follow"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].follow);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }



                if (compare(ev[1], "sp_frame"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].frame);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_frame_delay"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].frame_delay);
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

                                if (hurt_thing(nlist[0], nlist[1], 0) > 0)
                                        random_blood(spr[nlist[0]].x, spr[nlist[0]].y-40, nlist[0]);
                                if (spr[nlist[0]].nohit != 1)
                                        if (spr[nlist[0]].script != 0)

                                                if (locate(spr[nlist[0]].script, "HIT"))
                                                {

                                                        if (rinfo[script]->sprite != 1000)
                                                                *penemy_sprite = rinfo[script]->sprite;

                                                        kill_returning_stuff(spr[nlist[0]].script);
                                                        run_script(spr[nlist[0]].script);
                                                }

                                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }







                if (compare(ev[1], "sp_hard"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].hard);
                                if (spr[nlist[0]].sp_index != 0) if (nlist[1] != -1)
                                {

                                        pam.sprite[spr[nlist[0]].sp_index].hard = returnint;
                                }
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_move_nohard"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].move_nohard);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }
                if (compare(ev[1], "sp_flying"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].flying);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }




                if (compare(ev[1], "sp_kill_wait"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                spr[nlist[0]].wait = 0;
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }
                if (compare(ev[1], "sp_kill"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                spr[nlist[0]].kill = nlist[1];
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "screenlock"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                screenlock = nlist[0];
                                return(0);
                        }
                        returnint =  -1;
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


                if (compare(ev[1], "sp_x"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].x);
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


                if (compare(ev[1], "sp_mx"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].mx);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_my"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].my);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }



                if (compare(ev[1], "scripts_used"))
                {
                        h = &h[strlen(ev[1])];
                        int m = 0;
			int i;
                        for (i = 1; i < MAX_SCRIPTS; i++)
                                if (rinfo[i] != NULL) m++;
                                returnint = m;
                                return(0);
                }




                if (compare(ev[1], "sp_dir"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].dir);

                                if (nlist[1] != -1) changedir(spr[nlist[0]].dir, nlist[0], spr[nlist[0]].base_walk);
                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }

                if (compare(ev[1], "sp_hitpoints"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].hitpoints);

                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                if (compare(ev[1], "sp_attack_hit_sound"))
                {
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].attack_hit_sound);

                                return(0);
                        }
                        returnint =  -1;
                        return(0);
                }


                                                                if (compare(ev[1], "sp_attack_hit_sound_speed"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].attack_hit_sound_speed);

                                                                                return(0);
                                                                        }
                                                                        returnint =  -1;
                                                                        return(0);
                                                                }


                                                                if (compare(ev[1], "sp_strength"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].strength);

                                                                                return(0);
                                                                        }
                                                                        returnint =  -1;
                                                                        return(0);
                                                                }

                                                                if (compare(ev[1], "sp_defense"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].defense);

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

                                                                                figure_out(slist[0], 0);
                                                                                return(0);
                                                                        }
                                                                        returnint =  -1;
                                                                        return(0);
                                                                }


                                                                if (compare(ev[1], "sp_distance"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].distance);

                                                                                return(0);
                                                                        }
                                                                        returnint =  -1;
                                                                        return(0);
                                                                }


                                                                if (compare(ev[1], "sp_nohit"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].nohit);
                                                                                return(0);
                                                                        }
                                                                        returnint =  -1;
                                                                        return(0);
                                                                }


                                                                if (compare(ev[1], "sp_notouch"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].notouch);
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

                                                                                if (compare(play.item[*pcur_magic].name, slist[0]))
                                                                                {
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

                                                                                if (nlist[0] == 0)
                                                                                {
                                                                                        Msg("Error: Can't compare sprite script for sprite 0!?");
                                                                                        return(0);
                                                                                }
                                                                                if (spr[nlist[0]].active)
                                                                                {

                                                                                        if (spr[nlist[0]].script == 0)
                                                                                        {
                                                                                                Msg("Compare sprite script says: Sprite %d has no script.",nlist[0]);
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


                                                                if (compare(ev[1], "sp_y"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].y);
                                                                                return(0);
                                                                        }
                                                                        returnint =  -1;
                                                                        return(0);
                                                                }


                                                                if (compare(ev[1], "sp_timing"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        int p[20] = {1,1,0,0,0,0,0,0,0,0};
                                                                        if (get_parms(ev[1], script, h, p))
                                                                        {
                                                                                returnint = change_sprite(nlist[0], nlist[1], &spr[nlist[0]].timer);
                                                                                return(0);
                                                                        }
                                                                        returnint =  -1;
                                                                        return(0);
                                                                }


                                                                if (compare(ev[1], "return;"))
                                                                {

                                                                        if (debug_mode) Msg("Found return; statement");

                                                                        if (rinfo[script]->proc_return != 0)
                                                                        {
                                                                                run_script(rinfo[script]->proc_return);
                                                                                kill_script(script);
                                                                        }

                                                                        return(2);
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
                                                                        strcpy(s, h);
                                                                        //g("continuing to run line %s..", h);


                                                                        return(5);

                                                                }



                                                                if (compare(ev[2], "="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[1];
                                                                        strip_beginning_spaces(h);
                                                                        var_equals(ev[1], ev[3], '=', script, h);
                                                                        strcpy(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "+="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);
                                                                        var_equals(ev[1], ev[3], '+', script, h);
                                                                        strcpy(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "*="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);
                                                                        var_equals(ev[1], ev[3], '*', script, h);
                                                                        strcpy(s, h);
                                                                        return(0);
                                                                }



                                                                if (compare(ev[2], "-="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);

                                                                        var_equals(ev[1], ev[3], '-', script, h);

                                                                        strcpy(s, h);
                                                                        return(0);
                                                                }


                                                                if (compare(ev[2], "/"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[1];
                                                                        strip_beginning_spaces(h);

                                                                        var_equals(ev[1], ev[3], '/', script, h);

                                                                        strcpy(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "*"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[1];
                                                                        strip_beginning_spaces(h);

                                                                        var_equals(ev[1], ev[3], '*', script, h);

                                                                        strcpy(s, h);
                                                                        return(0);
                                                                }
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

                                                                                        return(2);
                                                                                } else
                                                                                {
                                                                                        Msg("Error:  Couldn't find procedure %s in %s.", slist[1], slist[0]);
                                                                                        kill_script(myscript1);
                                                                                }
                                                                        }
                                                                        strcpy(s, h);
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
                                                                                return(2);
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

bad:
        strcpy(s, h);
        return(0);

good:
        strcpy(s, h);
        //s = h
        //Msg("ok, continuing with running %s..",s);
        return(1);

}


void run_script(int script)
{
  int result;
  char line[200];

  returnint = 0;
  returnstring[0] = 0;

  if (rinfo[script] != NULL)
    {
      if (debug_mode)
	Msg("Script %s is entered at offset %d.", rinfo[script]->name, rinfo[script]->current);
    }
  else
    {
      Msg("Error:  Tried to run a script that doesn't exist in memory.  Nice work.");
    }

  while (read_next_line(script, line))
    {
      while(1)
	{
	  strip_beginning_spaces(line);
	  if (compare(line, "\n"))
	    break;

	  result = process_line(script, line, /*false*/0);

	  if (result == 3)
	    {
	    redo:
	      read_next_line(script, line);
	    crappa:
	      strip_beginning_spaces(line);
	      if (compare(line, "\n")) goto redo;
	      if (compare(line, "\\\\")) goto redo;
	      //           Msg("processing %s knowing we are going to skip it...", line);
	      result = process_line(script,line, /*true*/1);
	    }

	  if (result == 5)
	    goto crappa;

	  if (result == 3)
	      goto redo;

	  if (result == 2)
	    {
	      if (debug_mode)
		Msg("giving script the boot");
	      //quit script
	      return;
	    }

	  if (result == 0)
	    break;

	  if (result == 4)
	    {
	      //       Msg("Was sent %s, length %d", line, strlen(line));
	      if (strlen(line) < 2)
		{
		redo2:
		  read_next_line(script, line);
		  strip_beginning_spaces(line);
		  //Msg("Comparing to %s.", line);
		  if (compare(line, "\n"))
		    goto redo2;
		  if (compare(line, "\\\\"))
		    goto redo2;
		}
	      result = process_line(script,line, /*true*/1);
	    }

	  if (result == 2)
	    {
	      if (debug_mode)
		Msg("giving script the boot");
	      //quit script
	      return;
	    }
	  if (result == 0) break;
	}
    }

  if (rinfo[script] != NULL && rinfo[script]->proc_return != 0)
    {
      run_script(rinfo[script]->proc_return);
      kill_script(script);
    }
}


/**
 * Run callbacks, order by index. Sets the activation delay if
 * necessary. Kill obsolete callbacks along the way.
 *
 * Callbacks are set by wait() and
 * set_callback_random(). spawn()/external()/etc. use other
 * mechanisms.
 **/
void process_callbacks(void)
{
  int now = SDL_GetTicks();
  int i, k;

  for (i = 1; i < MAX_SCRIPTS; i++)
    {
      if (rinfo[i] != NULL)
	{
	  if (rinfo[i]->sprite > 0 && rinfo[i]->sprite != 1000 && spr[rinfo[i]->sprite].active == /*false*/0)
	    {
	      //kill this script, owner is dead
	      if (debug_mode)
		Msg("Killing script %s, owner sprite %d is dead.", rinfo[i]->name, rinfo[i]->sprite);
	      kill_script(i);
	      /*free(rinfo[i]);
		rinfo[i] = NULL;
		free(rbuf[i]);
		rbuf[i] = NULL;*/
	    }
	}
    }
  
  for (k = 1; k < MAX_CALLBACKS; k++)
    {
      if (callback[k].active)
	{
	  if (callback[k].owner > 0 && rinfo[callback[k].owner] == NULL)
	    {
	      //kill this process, it's owner sprite is 'effin dead.
	      if (debug_mode)
		Msg("Killed callback %s because script %d is dead.", k, callback[k].owner);
	      callback[k].active = /*false*/0;
	    }
	  else
	    {
	      if (callback[k].timer == 0)
		{
		  //set timer
		  
		  if (callback[k].max > 0)
		    callback[k].timer = now + (rand() % callback[k].max) + callback[k].min;
		  else
		    callback[k].timer = now + callback[k].min;
		}
	      else
		{
		  if (callback[k].timer < now)
		    {
		      callback[k].timer = 0;
		      
		      if (compare(callback[k].name, ""))
			{
			  //callback defined no proc name, so lets assume they want to start the script where it
			  //left off
			  //kill this callback
			  callback[k].active = /*false*/0;
			  run_script(callback[k].owner);
			  if (debug_mode)
			    Msg("Called script %d with callback %d.", callback[k].owner, k);
			}
		      else
			{
			  if (debug_mode)
			    Msg("Called proc %s with callback %d.", callback[k].name, k);
			  
			  //callback defined a proc name
			  if (locate(callback[k].owner,callback[k].name))
			    {
			      //found proc, lets run it
			      run_script(callback[k].owner);
			    }
			}
		    }
		}
	    }
	}
    }
}


void init_scripts(void)
{
  int k;
  for (k = 1; k < MAX_SCRIPTS; k++)
    {
      if (rinfo[k] != NULL && rinfo[k]->sprite != 0
	  && rinfo[k]->sprite != 1000 /* don't go out of bounds in spr[300] */
	  && spr[rinfo[k]->sprite].active)
	{
	  if (locate(k,"main"))
	    {
	      if (debug_mode) Msg("Screendraw: running main of script %s..", rinfo[k]->name);
	      run_script(k);
	    }
	}
    }
}
