/**
 * Dink Scripting

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

#ifndef _SCRIPTING_H
#define _SCRIPTING_H

#define MAX_VARS 250
#define MAX_SCRIPTS 200
#define MAX_CALLBACKS 100

/*struct refinfo
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
  *bool*int skipnext;
  int onlevel;
  int proc_return;
  * v1.08 arguments for user-defined functions *
  int arg1;
  int arg2;
  int arg3;
  int arg4;
  int arg5;
  int arg6;
  int arg7;
  int arg8;
  int arg9;

#ifdef HAVE_LUA
  int islua;
  char* luaproc;
  int lua_script_loaded;
  int lua_script_index;
#endif
};*/

struct script_engine
{
  int active;
  char *name;
  char **extensions; // Array of file extensions supported by engine, don't forget NULL sentinel

  int (*allocate_data)(void **data); // Create pointer to engine-specific data storage,
                                     // accessible through sinfo[#]->data later. Returning 0
                                     // indicates an error condition. Setting *data to NULL
                                     // results in free_data() never being called.

  void (*free_data)(void *data); // Free data previously created by allocate_data(); If you don't
                                 // use the data field (i.e. data is NULL), this can be left unset.
  
  int (*load_script)(const char *path, int script);
  void (*kill_script)(int script);
  
  int (*script_proc_exists)(int script, const char *proc);
  int (*run_script_proc)(int script, const char *proc);
  void (*resume_script)(int script);
};

struct scriptinfo
{
  char *name;
  int sprite;
  struct script_engine *engine;
  void *data; // Script-engine specific data
};

extern struct scriptinfo *sinfo[];

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
extern struct call_back callback[MAX_CALLBACKS];
/* TODO: Used 1->100 in the game, should it be MAX_CALLBACKS+1 ? */

/* Named index for varman.scope */
#define VAR_GLOBAL_SCOPE 0

/* Part of the savegame data dump, before of size change! */
struct varman
{
  int var;
  char name[20];
  int scope; /* script number holding the local variable */
  BOOL_1BYTE active;
};

// global functions (v1.08)
struct global_function
{
  char file[10];
  char func[20];
};

extern char scripting_error[255];

extern void decipher_string(char** line_p, int script);
extern int var_exists(const char name[20], int scope);
extern int scripting_make_int(const char name[80], int value, int scope);
extern int search_var_with_this_scope(char* variable, int scope);
extern int search_var_with_this_scope_108(char* variable, int var_scope);
extern /*bool*/int load_game_small(int num, char line[196], int *mytime);
extern void var_replace(char** line_p, int scope);

extern void scripting_init();
extern void scripting_quit();
extern int scripting_load_script(const char filename[15], int sprite, /*bool*/int set_sprite);
extern int scripting_proc_exists(int script, const char* proc);
extern int scripting_run_proc(int script, const char* proc);
extern void scripting_resume_script(int script);
extern void scripting_kill_scripts_owned_by(int sprite);
extern void scripting_kill_all_scripts(void);
extern void scripting_kill_all_scripts_for_real(void);
extern void scripting_kill_callbacks(int script);
extern void scripting_kill_callback(int cb);
extern void scripting_kill_callbacks_owned_by_script(int script);
extern void scripting_process_callbacks(void);
extern void scripting_kill_script(int k);
extern void scripting_init_scripts();

#endif
