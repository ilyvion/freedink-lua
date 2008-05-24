/**
 * Common code for FreeDink and FreeDinkedit

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2003  Shawn Betts
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

#ifndef __DEMO
#define __REAL_THING
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* strncasecmp */
#include <ctype.h>
#include <time.h>

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

#include "game_engine.h"

/* #include "ddutil.h" */
#include "fastfile.h"
#include "io_util.h"


#include "freedink.h"
#include "dinkvar.h"
#include "gfx.h"
#include "gfx_tiles.h"
#include "gfx_sprites.h"
#include "gfx_utils.h"
/* for DinkC's initfonts(): */
#include "gfx_fonts.h"
#include "bgm.h"
#include "sfx.h"
#include "dinkc.h"
#include "dinkc_bindings.h"

#include "str_util.h"
#include "paths.h"
#include "log.h"

//if true, will close app as soon as the message pump is empty
int g_b_kill_app = 0;

int dinkspeed = 3;
char current_map[255] = "MAP.DAT";
/*bool*/int item_screen = /*false*/0;

void update_status_all(void);
int add_sprite(int x1, int y, int brain,int pseq, int pframe );

void add_exp(int num, int h);
void draw_status_all(void);
void check_seq_status(int h);

void add_text(char *tex ,char *filename);
void program_idata(void);
int map_vision = 0;
int realhard(int tile);
int flub_mode = -500;
int draw_map_tiny = -1;

int walk_off_screen = /*false*/0;

/* Skip flipping the double buffer for this frame only - used when
   setting up show_bmp and copy_bmp */
/*bool*/int abort_this_flip = /*false*/0;




struct show_bmp showb;

int keep_mouse = 0;


struct attackinfo_struct bow;

int screenlock = 0;

struct talk_struct talk;

int mbase_timing;
#define MAX_IDATA 600
enum idata_type { IDATA_EMPTY=0, IDATA_SPRITE_INFO, IDATA_FRAME_SPECIAL,
		  IDATA_FRAME_DELAY, IDATA_FRAME_FRAME };
struct idata
{
  enum idata_type type;
  int seq;
  int frame;
  int xoffset, yoffset;
  rect hardbox;
};
struct idata id[MAX_IDATA];

unsigned long mold;

int item_timer;
int item_pic;

int mbase_count;
/*bool*/int no_transition = /*false*/0;





int push_active = 1;


/*bool*/int turn_on_plane = /*FALSE*/0;
const int text_timer = 77;
const int text_min = 2700;
int stop_entire_game;
const int max_game = 20;
/*bool*/int in_enabled = /*false*/0;
char *in_string;




/*bool*/int first_frame = /*false*/0;

/* If true, and if the engine is executing a screen's attached script,
   and if main() loads new graphics (preload_seq()...), then
   load_sprites and load_sprite_pak will display a "Please Wait"
   animation. */
/*bool*/int no_running_main = /*false*/0;

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
int sp_speed = 0;
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

char dir[80];


//defaults




int process_line (int script, char *s, /*bool*/int doelse);




int  show_dot = /*FALSE*/0;
int  plane_process = /*TRUE*/1;
struct hit_map hm;

int last_sprite_added = 0;
unsigned long timer = 0;
char *command_line;
/*bool*/int dinkedit = /*false*/0;
int base_timing;
int weapon_script = 0;
int magic_script = 0;

int sp_mode = 0;
int fps,fps_final = 0;
int move_screen = 0;
/*bool*/int move_gonna = /*false*/0;
int move_counter = 0;
int m1x,m1y;
int m2x,m2y;
int m3x,m3y;
int playx = 620;
/*bool*/int windowed = /*false*/0;
int playl = 20;

/*bool*/int mouse1 = /*false*/0;
int playy = 400;
int cur_map,cur_tile;
struct seth_joy sjoy;

/* Number of ms since an arbitrarily fixed point */
Uint32 thisTickCount,lastTickCount;
/* SDL_gfx accurate framerate */
FPSmanager framerate_manager;

unsigned long timecrap;
rect math,box_crap,box_real;

int sz,sy,x_offset,y_offset;



int mode;

struct sequence seq[MAX_SEQUENCES];
struct small_map pam;


/*bool*/int trig_man = /*false*/0;
/*bool*/int total_trigger = /*false*/0;

/* TODO: get rid of either k or GFX_k */
struct pic_info     k[MAX_SPRITES];       // Sprite data
struct GFX_pic_info GFX_k[MAX_SPRITES];   // Sprite data (SDL)

struct player_info play;




/* LPDIRECTDRAWSURFACE     game[max_game];       // Game pieces */
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
struct hardness hmap;

void clear_talk(void)
{
        memset(&talk, 0, sizeof(talk));
        play.mouse = 0;
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


//add hardness from a sprite

int getpic(int h)
{
        if (spr[h].pseq == 0) return(0);
        if (spr[h].pseq >= MAX_SEQUENCES)
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


/**
 * Load 1 screen from map.dat, which contains all 768 game screens
 */
void load_map(const int num)
{
  FILE *fp;
  long holdme,lsize;
  fp = paths_dmodfile_fopen(current_map, "rb");
  if (!fp)
    {
      Msg("Cannot find %s file!!!",current_map);
      return;
    }
  lsize = sizeof(struct small_map);
  holdme = (lsize * (num-1));
  fseek(fp, holdme, SEEK_SET);
  //Msg("Trying to read %d bytes with offset of %d",lsize,holdme);
  int shit = fread(&pam, lsize, 1, fp);       /* current player */
  //              Msg("Read %d bytes.",shit);
  if (shit == 0)
    Msg("ERROR:  Couldn't read map %d?!?", num);
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
  FILE *fp;
  long holdme,lsize;

  Msg("Saving map data..");
  if (num > 0)
    {
      fp = paths_dmodfile_fopen(current_map, "r+b");
      if (fp == NULL)
	{
	  perror("Cannot save map");
	  return;
	}
      lsize = sizeof(struct small_map);
      holdme = (lsize * (num-1));
      fseek(fp, holdme, SEEK_SET);
      fwrite(&pam, lsize, 1, fp);       /* current player */
      fclose(fp);
    }

  Msg("Done saving map data..");
}




void save_info(void)
{
  FILE *fp = paths_dmodfile_fopen("dink.dat", "wb");
  if (fp != NULL)
    {
      fwrite(&map, sizeof(struct map_info), 1, fp);
      fclose(fp);
    }
  else
    {
      perror("Cannot save dink.dat");
    }
}



/*bool*/int load_game(int num)
{
  FILE *fp;
  
  //lets get rid of our magic and weapon scripts
  if (weapon_script != 0)
    {
      if (locate(weapon_script, "DISARM"))
	{
	  run_script(weapon_script);
	}
    }

  if (magic_script != 0 && locate(magic_script, "DISARM"))
    run_script(magic_script);

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
  
  fp = paths_savegame_fopen(num, "rb");
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
	    }
	  else
	    {
	      weapon_script = load_script(play.item[*pcur_weapon].name, 1000, /*false*/0);
	      if (locate(weapon_script, "DISARM"))
		run_script(weapon_script);
	      weapon_script = load_script(play.item[*pcur_weapon].name, 1000, /*false*/0);
	      if (locate(weapon_script, "ARM"))
		run_script(weapon_script);
	    }
	}
      if (*pcur_magic != 0)
	{
	  if (play.item[*pcur_magic].active == /*false*/0)
	    {
	      *pcur_magic = 0;
	      magic_script = 0;
	      Msg("Loadgame error: Player doesn't have armed magic - changed to 0.");
	    }
	  else
	    {
	      
	      magic_script = load_script(play.mitem[*pcur_magic].name, 1000, /*false*/0);
	      if (locate(magic_script, "DISARM"))
		run_script(magic_script);
	      magic_script = load_script(play.mitem[*pcur_magic].name, 1000, /*false*/0);
	      if (locate(magic_script, "ARM"))
		run_script(magic_script);
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

/*bool*/int add_time_to_saved_game(int num)
{
  FILE *fp;

  fp = paths_savegame_fopen(num, "rb");
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
  
  fp = paths_savegame_fopen(num, "wb");
  if (fp)
    {
      fwrite(&play,sizeof(play),1,fp);
      fclose(fp);
    }
  Msg("Wrote it.(%d of time)", play.minutes);
  
  return(/*true*/1);
}

void save_game(int num)
{
  FILE *fp;

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
  fp = paths_savegame_fopen(num, "wb");
  if (fp != NULL)
    {
      fwrite(&play,sizeof(play),1,fp);
      fclose(fp);
    }
  else
    {
      perror("Cannot save game");
    }
}



void kill_all_vars(void)
{
  memset(&play, 0, sizeof(play));
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


void load_info(void)
{
  FILE *fp;

  fp = paths_dmodfile_fopen("dink.dat", "rb");
  if (!fp)
    {
      //make new data file
      fp = paths_dmodfile_fopen("dink.dat", "wb");
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
  FILE *fp = paths_dmodfile_fopen("hard.dat", "wb");
  if (!fp)
    {
      perror("Couldn't save hard.dat");
      return;
    }
  fwrite(&hmap, sizeof(struct hardness), 1, fp);
  fclose(fp);
}


/**
 * Load hard.dat which contains tile hardness information
 */
void load_hard(void)
{
  FILE *fp = NULL;

  /* Try loading the D-Mod hard.dat */
  fp = paths_dmodfile_fopen("hard.dat", "rb");

  /* If running the game, fallback to the default hard.dat, but if
     running the editor, recreate it in all cases. */
  if (fp == NULL && !dinkedit)
    fp = paths_fallbackfile_fopen("hard.dat", "rb");


  if (fp != NULL)
    {
      fread(&hmap,sizeof(struct hardness), 1, fp);
      fclose(fp);
    }
  else
    {
      //make new data file
      memset(&hmap, 0, sizeof(struct hardness));
      fp = paths_dmodfile_fopen("hard.dat", "wb");
      if (fp != NULL)
	{
	  fwrite(&hmap, sizeof(struct hardness), 1, fp);
	  fclose(fp);
	}
    }
}


/**
 * Parse a sprite .ini data or init() line, and act immediately
 */
void figure_out(char line[255], int load_seq)
{
  int myseq = 0,myframe = 0; int special = 0;
  int special2 = 0;
  int i;
  char ev[15][100];
  memset(&ev, 0, sizeof(ev));
  for (i = 1; i <= 14; i++)
    separate_string(line, i,' ',ev[i]);
  char *command = ev[1];
  
  // LOAD_SEQUENCE_NOW  path  seq  BLACK
  // LOAD_SEQUENCE_NOW  path  seq  LEFTALIGN
  // LOAD_SEQUENCE_NOW  path  seq  NOTANIM
  // LOAD_SEQUENCE_NOW  path  seq  speed  offsetx offsety  hard.left hard.top hard.right hard.bottom
  if (compare(command, "LOAD_SEQUENCE_NOW") ||
      compare(command, "LOAD_SEQUENCE"))
    {
      rect hardbox;
      memset(&hardbox, 0, sizeof(rect));

      int myseq = atol(ev[3]);
      seq[myseq].is_active = 1;
      seq_set_ini(myseq, line);

      if (compare(ev[4], "BLACK"))
	{
	  load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]),
		       hardbox, /*true*/1, /*true*/1, /*false*/0);
	}
      else if (compare(ev[4], "LEFTALIGN"))
	{
	  load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]),
		       hardbox, /*false*/0, /*false*/0, /*true*/1);
	}
      else if (compare(ev[4], "NOTANIM"))
	{
	  //not an animation!
	  load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]),
		       hardbox, /*false*/0, /*false*/0, /*false*/0);
	}
      else
	{
	  //yes, an animation!
	  hardbox.left = atol(ev[7]);
	  hardbox.top = atol(ev[8]);
	  hardbox.right = atol(ev[9]);
	  hardbox.bottom = atol(ev[10]);
	  
	  load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]),
		       hardbox, /*true*/1, /*false*/0, /*false*/0); //Crap
	}
      
      program_idata();
      return;
    }

  if (compare(command, "SET_SPRITE_INFO"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      myseq = atol(ev[2]);
      myframe = atol(ev[3]);
      k[seq[myseq].frame[myframe]].xoffset = atol(ev[4]);
      k[seq[myseq].frame[myframe]].yoffset = atol(ev[5]);
      k[seq[myseq].frame[myframe]].hardbox.left = atol(ev[6]);
      k[seq[myseq].frame[myframe]].hardbox.top = atol(ev[7]);
      k[seq[myseq].frame[myframe]].hardbox.right = atol(ev[8]);
      k[seq[myseq].frame[myframe]].hardbox.bottom = atol(ev[9]);
    }
  
  if (compare(command, "SET_FRAME_SPECIAL"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      myseq = atol(ev[2]);
      myframe = atol(ev[3]);
      special = atol(ev[4]);
      
      seq[myseq].special[myframe] = special;
      Msg("Set special.  %d %d %d", myseq, myframe, special);
    }

  if (compare(command, "SET_FRAME_DELAY"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      myseq = atol(ev[2]);
      myframe = atol(ev[3]);
      special = atol(ev[4]);
      
      seq[myseq].delay[myframe] = special;
      Msg("Set delay.  %d %d %d",myseq, myframe, special);
    }

  if (compare(command, "STARTING_DINK_X"))
    {
      myseq = atol(ev[2]);
      play.x = myseq;
    }

  if (compare(command, "STARTING_DINK_Y"))
    {
      myseq = atol(ev[2]);
      play.y = myseq;
    }
  
  if (compare(command, "SET_FRAME_FRAME"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      myseq = atol(ev[2]);
      myframe = atol(ev[3]);
      special = atol(ev[4]);
      special2 = atol(ev[5]);
      
      if (special == -1)
	seq[myseq].frame[myframe] = special;
      else
	seq[myseq].frame[myframe] = seq[special].frame[special2];
      Msg("Set frame.  %d %d %d",myseq, myframe, special);
    }
}

/* Interpret/execute sprite instructions */
void program_idata(void)
{
  int i;
  for (i = 1; i < MAX_IDATA; i++)
    {
      if (id[i].type == IDATA_EMPTY)
	return;

      if (id[i].type == IDATA_SPRITE_INFO)
	{
	  k[seq[id[i].seq].frame[id[i].frame]].xoffset = id[i].xoffset;
	  k[seq[id[i].seq].frame[id[i].frame]].yoffset = id[i].yoffset;
	  rect_copy(&k[seq[id[i].seq].frame[id[i].frame]].hardbox, &id[i].hardbox);
	  
	  // Msg("Programming idata type %d in %d...Seq %d Frame %d (Hardbox is %d %d %d %d)", id[i].type, i,
	  //     id[i].seq, id[i].frame, id[i].hardbox.left,id[i].hardbox.right, id[i].hardbox.top, id[i].hardbox.bottom);
	}
      
      if (id[i].type == IDATA_FRAME_SPECIAL)
	{
	  //set special
	  seq[id[i].seq].special[id[i].frame] = id[i].xoffset;
	}
      if (id[i].type == IDATA_FRAME_DELAY)
	{
	  //set delay
	  seq[id[i].seq].delay[id[i].frame] = id[i].xoffset;
	}
      
      if (id[i].type == IDATA_FRAME_FRAME)
	{
	  if (id[i].xoffset == -1)
	    seq[id[i].seq].frame[id[i].frame] = -1;
	  else
	    seq[id[i].seq].frame[id[i].frame] = seq[id[i].xoffset].frame[id[i].yoffset];
	}
    }
}

/* Store sprite instructions from dink.ini */
void make_idata(enum idata_type type, int myseq, int myframe, int xoffset, int yoffset, rect crect)
{
  int i;
  for (i = 1; i < MAX_IDATA; i++)
    {
      if (id[i].type == IDATA_EMPTY)
	{
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

/**
 * Parse a dink.ini line, and store instructions for later processing
 * (used in game initialization through 'load_batch')
 */
void pre_figure_out(char line[255], int load_seq)
{
  int i;
  char ev[15][100];
  memset(&ev, 0, sizeof(ev));
  for (i = 1; i <= 14; i++)
    separate_string(line, i, ' ', ev[i]);
  char *command = ev[1];


  // PLAYMIDI  filename
  if (compare(command, "playmidi"))
    {
      char* midi_filename = ev[2];
      if (!dinkedit)
	PlayMidi(midi_filename);
    }

  // LOAD_SEQUENCE_NOW  path  seq  BLACK
  // LOAD_SEQUENCE_NOW  path  seq  LEFTALIGN
  // LOAD_SEQUENCE_NOW  path  seq  NOTANIM
  // LOAD_SEQUENCE_NOW  path  seq  speed
  // LOAD_SEQUENCE_NOW  path  seq  speed  offsetx offsety  hard.left hard.top hard.right hard.bottom
  if (compare(command, "LOAD_SEQUENCE_NOW"))
    {
      rect hardbox;
      memset(&hardbox, 0, sizeof(rect));

      int myseq = atol(ev[3]);
      seq[myseq].is_active = 1;
      seq_set_ini(myseq, line);

      if (compare(ev[4], "BLACK"))
	{
	  load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]),
		       hardbox, /*not_anim=*/1, /*black=*/1, /*leftalign=*/0);
	}
      else if (compare(ev[4], "LEFTALIGN"))
	{
	  load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]),
		       hardbox, /*not_anim=*/0, /*black=*/0, /*leftalign=*/1);
	}
      else if (compare(ev[4], "NOTANIM"))
	{
	  //not an animation!
	  load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]),
		       hardbox, /*not_anim=*/0, /*black=*/0, /*leftalign=*/0);
	}
      else
	{
	  //yes, an animation!
	  hardbox.left = atol(ev[7]);
	  hardbox.top = atol(ev[8]);
	  hardbox.right = atol(ev[9]);
	  hardbox.bottom = atol(ev[10]);
	  
	  load_sprites(ev[2],atol(ev[3]),atol(ev[4]),atol(ev[5]),atol(ev[6]),
		       hardbox, /*not_anim=*/1, /*black=*/0, /*leftalign=*/0);
	}
      
      int myseq_TODO = atol(ev[2]);   // isn't it #3???
      int myframe = atol(ev[3]); // isn't it #4???
      rect_set(&hardbox, atol(ev[6]), atol(ev[7]), atol(ev[8]), atol(ev[9]));
      make_idata(IDATA_SPRITE_INFO, myseq_TODO, myframe,atol(ev[4]), atol(ev[5]),hardbox);
      return;
    }

  // LOAD_SEQUENCE  path  seq  BLACK
  // LOAD_SEQUENCE  path  seq  LEFTALIGN
  // LOAD_SEQUENCE  path  seq  NOTANIM
  // LOAD_SEQUENCE  path  seq  speed
  // LOAD_SEQUENCE  path  seq  speed  offsetx offsety  hard.left hard.top hard.right hard.bottom
  if (compare(command, "LOAD_SEQUENCE"))
    {
      int myseq = atol(ev[3]);
      seq_set_ini(myseq, line);
      seq[myseq].is_active = 1;
      return;
    }
  
  if (compare(command, "SET_SPRITE_INFO"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      //if (k[seq[myseq].frame[myframe]].frame = 0) Msg("Changing sprite that doesn't exist...");
      
      rect hardbox;
      int myseq = atol(ev[2]);
      int myframe = atol(ev[3]);
      rect_set(&hardbox, atol(ev[6]), atol(ev[7]), atol(ev[8]), atol(ev[9]));
      make_idata(IDATA_SPRITE_INFO, myseq, myframe,atol(ev[4]), atol(ev[5]),hardbox);
      return;
    }
  
  if (compare(command, "SET_FRAME_SPECIAL"))
    {
      rect hardbox;
      int myseq = atol(ev[2]);
      int myframe = atol(ev[3]);
      int special = atol(ev[4]);
      make_idata(IDATA_FRAME_SPECIAL, myseq, myframe, special, 0, hardbox);
      return;
    }
  
  if (compare(command, "SET_FRAME_DELAY"))
    {
      rect hardbox;
      int myseq = atol(ev[2]);
      int myframe = atol(ev[3]);
      int delay = atol(ev[4]);
      make_idata(IDATA_FRAME_DELAY, myseq, myframe, delay, 0, hardbox);
      return;
    }
  
  // SET_FRAME_FRAME  seq frame  new_seq new_frame
  // SET_FRAME_FRAME  seq frame  -1
  if (compare(command, "SET_FRAME_FRAME"))
    {
      rect hardbox;
      int myseq = atol(ev[2]);
      int myframe = atol(ev[3]);
      int new_seq = atol(ev[4]);
      int new_frame = atol(ev[5]);
      
      make_idata(IDATA_FRAME_FRAME, myseq, myframe, new_seq, new_frame, hardbox);
    }

  // Obsolete?
  if (compare(command, "STARTING_DINK_X"))
    {
      int x = atol(ev[2]);
      play.x = x;
    }
  if (compare(command, "STARTING_DINK_Y"))
    {
      int y  = atol(ev[2]);
      play.y = y;
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
                        figure_out(seq[spr[h].pseq].ini, 0);
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

        if (!seq[h].is_active) return;

        if (h > 0)
        {
                // Msg("Smartload: Loading seq %d..", spr[h].seq);
                if (seq[h].frame[1] == 0)
                {
                        first_frame = /*true*/1;
                        figure_out(seq[h].ini, 0);
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

        if (!seq[h].is_active) return;
        if (h > 0) if (h < MAX_SEQUENCES)
        {
                // Msg("Smartload: Loading seq %d..", spr[h].seq);

                if (seq[h].frame[1] == 0)
                {
                        figure_out(seq[h].ini, 0);
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
    if (seq[base+i].is_active)
      check_seq_status(base+i);
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


void kill_sprite_all (int sprite)
{
        spr[sprite].active = /*false*/0;

        kill_text_owned_by(sprite);
        kill_scripts_owned_by(sprite);

}


void get_right(char line[200], char thing[100], char *ret)
        {
                char *dumb;
                int pos = strcspn(line, thing );


                if (pos == 0){ strcpy(ret, ""); return; }


                dumb = &ret[pos+1];
                strcpy(ret, dumb);
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
                                if (!seq[spr[k].seq].is_active)
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

                        if (!seq[spr[k].seq].is_active && seq[base+3].is_active)
			  spr[k].seq = base + 3;
                        if (!seq[spr[k].seq].is_active && seq[base+1].is_active)
			  spr[k].seq = base + 1;


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
                                if (!seq[spr[k].seq].is_active)
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
                        if (!seq[spr[k].seq].is_active && seq[base+7].is_active)
			  spr[k].seq = base + 7;
                        if (!seq[spr[k].seq].is_active && seq[base+1].is_active)
			  spr[k].seq = base + 1;
                }

                if (dir1 == 6)
                {
                        spr[k].mx = spr[k].speed;
                        spr[k].my = 0;
                        if (base != -1)
                                spr[k].seq = base + 6;

                        if (!seq[spr[k].seq].is_active && seq[base+3].is_active)
			  spr[k].seq = base + 3;
                        if (!seq[spr[k].seq].is_active && seq[base+9].is_active)
			  spr[k].seq = base + 9;

                }

                if (dir1 == 7)
                {
                        spr[k].mx = (0 - spr[k].speed) + (spr[k].speed / 3);
                        spr[k].my = (0 - spr[k].speed)+ (spr[k].speed / 3);
                        if (base != -1)
                        {
                                spr[k].seq = base + 7;


                                if (!seq[spr[k].seq].is_active)
				  spr[k].seq = base + 3;
                        }

                }
                if (dir1 == 8)
                {
                        spr[k].mx = 0;
                        spr[k].my = (0 - spr[k].speed);
                        if (base != -1)
                                spr[k].seq = base + 8;

                        if (!seq[spr[k].seq].is_active && seq[base+7].is_active)
			  spr[k].seq = base + 7;
                        if (!seq[spr[k].seq].is_active && seq[base+9].is_active)
			  spr[k].seq = base + 9;

                }


                if (dir1 == 9)
                {
                        spr[k].mx = spr[k].speed- (spr[k].speed / 3);
                        spr[k].my = (0 - spr[k].speed)+ (spr[k].speed / 3);
                        if (base != -1)
                        {
                                spr[k].seq = base + 9;
                                if (!seq[spr[k].seq].is_active)
                                        spr[k].seq = base + 1;
                        }
                }



                if (old_seq != spr[k].seq)
                {
                        spr[k].frame = 0;
                        spr[k].delay = 0;
                }


                if (!seq[spr[k].seq].is_active)
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


void show_bmp(char name[80], int showdot, int reserved, int script)
{
  SDL_Surface *image = NULL;
  FILE* in = NULL;

  in = paths_dmodfile_fopen(name, "rb");
  if (in == NULL)
    {
      Msg("Error: Can't open bitmap '%s'.", name);
      return;
    }

  image = load_bmp_setpal(in);
  if (image == NULL)
    {
      fprintf(stderr, "Couldn't load '%s'.\n", name);
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
  FILE* in = NULL;

  in = paths_dmodfile_fopen(name, "rb");
  if (in == NULL)
    {
      Msg("Error: Can't open bitmap '%s'.", name);
      return;
    }

  // memory leak?
  //lpDDSTrick = DDLoadBitmap(lpDD, name, 0, 0);

/*   lpDDPal = DDLoadPalette(lpDD, name); */

/*   if (lpDDPal) */
/*     lpDDSPrimary->SetPalette(lpDDPal); */

  image = load_bmp_setpal(in);
  if (image == NULL)
    {
      fprintf(stderr, "Couldn't load '%s'.\n", name);
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
