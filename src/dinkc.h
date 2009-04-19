/**
 * DinkC script engine

 * Copyright (C) 2008, 2009  Sylvain Beucler

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

#ifndef _DINKC_H
#define _DINKC_H

#include <stdio.h>
#include "io_util.h"

#define MAX_SCRIPTS 200
#define MAX_VARS 250

/* Part of the savegame data dump, before of size change! */
struct varman
{
  int var;
  char name[20];
  int scope; /* script number holding the local variable */
  BOOL_1BYTE active;
};
/* Named index for varman.scope */
#define DINKC_GLOBAL_SCOPE 0

// global functions (v1.08)
struct global_function
{
  char file[10];
  char func[20];
};

struct refinfo
{
  char* name;
  long location;
  long current; // current offset
  int cur_line; // current line
  int cur_col;  // current column (position within ligne)
  int debug_line; // last parsed line (whereas cur_line == next-to-be-read)
  int level;
  long end; // size of the text, == strlen(rbuf[i])
  int sprite; //if more than 0, it was spawned and is owned by a sprite, if 1000 doesn't die
  /*bool*/int skipnext;
  int onlevel;
  int proc_return;
  /* v1.08 arguments for user-defined functions */
  int arg1;
  int arg2;
  int arg3;
  int arg4;
  int arg5;
  int arg6;
  int arg7;
  int arg8;
  int arg9;
};
extern struct refinfo *rinfo[];

extern void dinkc_init();
extern void dinkc_quit();
extern int load_script(char filename[15], int sprite, /*bool*/int set_sprite);
extern int dinkc_execute_one_liner(char* line);
extern void strip_beginning_spaces(char *str);
extern /*bool*/int locate(int script, char* proc_lookup);
extern /*bool*/int locate_goto(char* expr, int script);
extern long decipher(char* variable, int script);
extern void decipher_string(char** line_p, int script);
extern int add_callback(char name[20], int n1, int n2, int script);
extern void kill_callback(int cb);
extern void kill_callbacks_owned_by_script(int script);
extern void kill_script(int k);
extern void kill_all_scripts_for_real(void);
extern char* read_next_line(int script);
extern void process_callbacks(void);
extern int var_exists(char name[20], int scope);
extern void make_int(char name[80], int value, int scope, int script);
extern void var_equals(char name[20], char newname[20], char math, int script, char rest[200]);
extern int var_figure(char h[200], int script);
extern void kill_scripts_owned_by(int sprite);
extern void kill_returning_stuff(int script);
extern void run_script(int script);
extern void attach(void);
extern void int_prepare(char line[100], int script);
extern void make_function(char file[10], char func[20]);

/* Used by gfx_tiles.c only */
extern void kill_all_scripts(void);
extern void init_scripts(void);

extern int returnint;
extern int bKeepReturnInt; /* v1.08 */
extern char returnstring[];
extern unsigned short decipher_savegame;

#endif
