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

struct script_engine
{
  /*
   * Sentinel entry indicator (should always be true on non-sentinel entries)
   */
  int active;

  /*
   * Name of the script.
   */
  char *name;

  /*
   * Array of file extensions supported by engine, don't forget NULL sentinel
   * This array and its entires should all be allocated in such a way that they
   * can be free'd safely. In other words, use malloc or strdup or other
   * functions that return free-able pointers.
   */
  char **extensions;

  /*
   * Create pointer to engine-specific data storage, accessible through
   * sinfo[#]->data later. Returning 0 indicates an error condition. Setting
   * *data to NULL in this function results in free_data() never being called.
   */
  int (*allocate_data)(void **data);

  /*
   * Free data previously created by allocate_data(); If you don't use the data
   * field (i.e. *data is set to NULL in allocate_data()), this pointer can be
   * left unset.
   */
  void (*free_data)(void *data);

  /*
   * Called to load a script, receives the path to the script, and the script
   * number this script is assigned to. Returning 0 indicates that an error
   * occured during the script loading.
   * 
   * At the point this function is called, sinfo[script] is intialized, as is
   * sinfo[script]->data, using the allocate_data() function.
   */
  int (*load_script)(const char *path, int script);

  /*
   * Nothing has to be done in this function, but it's a way for you to clean
   * up things you hold on to associated with the script that is not in
   * sinfo[script]->data (which is cleaned up automatically, using the
   * free_data() function)
   */
  void (*kill_script)(int script);

  /*
   * Return 0 if the procedure does not exist in the script, otherwise,
   * return 1. Not that this function is not necessarily called before
   * run_script_proc() is called, and only exists because the Dink engine
   * occasionally makes decisions depending on whether a script function
   * exists or not before actually running the function.
   */
  int (*script_proc_exists)(int script, const char *proc);

  /*
   * Run the procedure in the given script. The return value of this
   * function should be as with script_proc_exists. In addition to
   * checking for existence, however, this function also runs the procedure.
   */
  int (*run_script_proc)(int script, const char *proc);

  /*
   * Resumes a script after the script has yielded.
   */
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

//extern void decipher_string(char** line_p, int script);
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
extern int scripting_add_callback(const char name[20], int n1, int n2, int script);
extern void scripting_kill_callbacks(int script);
extern void scripting_kill_callback(int cb);
extern void scripting_kill_callbacks_owned_by_script(int script);
extern void scripting_process_callbacks(void);
extern void scripting_kill_script(int k);
extern void scripting_init_scripts();

#endif
