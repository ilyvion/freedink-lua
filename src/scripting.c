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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <xalloc.h>

#include "io_util.h"
#include "game_engine.h"
#include "str_util.h"
#include "input.h"
#include "paths.h"
#include "log.h"

#include "gettext.h"
#define _(String) gettext (String)

#include "scripting.h"
#include "dinkc.h"
#include "dinklua.h"

int returnint = 0;

/* Used to tell decipher_string about the currently selected savegame
   in a choice menu; also abuse to tell which key is selected in
   joystick remapping */
unsigned short decipher_savegame = 0;

struct scriptinfo *sinfo[MAX_SCRIPTS];
struct call_back callback[MAX_CALLBACKS];

/**
 * Only load game metadata (timetime). Used when displaying the list
 * of saved games (see decipher_string).
 */
/*bool*/int load_game_small(int num, char line[196], int *mytime)
{
  FILE *f = paths_savegame_fopen(num, "rb");
  if (f == NULL)
    {
      log_info("Couldn't quickload save game %d", num);
      return /*false*/0;
    }
  else
    {
      //int version = read_lsb_int(f);
      fseek(f, 4, SEEK_CUR);

      fread(line, 196, 1, f);
      line[195] = '\0';
      *mytime = read_lsb_int(f);
      fclose(f);

      return /*true*/1;
    }
}

/**
 * v1.07-style scope. This function is buggy: the first memory slot
 * has precedence (independently of local/global scope).
 * 
 * Return -1 if not found, slot index >1 if found. Slot 0 isn't
 * currently used by the engine.
 */
int search_var_with_this_scope_107(char* variable, int var_scope)
{
  int i;
  for (i = 1; i < MAX_VARS; i ++)
    if (play.var[i].active == 1
	&& ((play.var[i].scope == VAR_GLOBAL_SCOPE) || (play.var[i].scope == var_scope))
	&& (compare(play.var[i].name, variable)))
      return i;
  return -1; /* not found */
}

/**
 * v1.08-style scope: local variables are search before global
 * variables.
 *
 * Return -1 if not found, slot index >1 if found. Slot 0 isn't
 * currently used by the engine.
 */
int search_var_with_this_scope_108(char* variable, int var_scope)
{
  int search_scope[2];
  search_scope[0] = var_scope; /* first local scope */
  search_scope[1] = VAR_GLOBAL_SCOPE; /* then global scope */

  int i;
  for (i = 0; i < 2; i++)
    {
      //We'll start going through every var, starting at one
      int v;
      for (v = 1; v < MAX_VARS; v++)
	{
	  //Okay... make sure the var is active,
	  //The scope should match the script,
	  //Then make sure the name is the same.
	  if (play.var[v].active
	      && play.var[v].scope == search_scope[i]
	      && compare (play.var[v].name, variable))
	    return v;
	}
    }
  return -1;
}

/**
 * 
 */
int search_var_with_this_scope(char* variable, int scope)
{
  if (dversion >= 108)
    return search_var_with_this_scope_108(variable, scope);
  return search_var_with_this_scope_107(variable, scope);      
}

int var_exists(const char name[20], int scope)
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
          return i;
        }
      }
    }
  }
  return 0;
}

char scripting_error[255] = {0};

int scripting_make_int(const char name[80], int value, int scope)
{
  int dupe;
  int i;
  if (strlen(name) > 19)
  {
    sprintf(scripting_error, "varname %s is too long", name);
    return 0;
  }
  dupe = var_exists(name, scope);

  if (dupe > 0)
  {
    if (scope != VAR_GLOBAL_SCOPE)
    {
      sprintf(scripting_error, "Local var %s already used in this procedure",
              name);

      play.var[dupe].var = value;
    }
    else
    {
      sprintf(scripting_error, "var %s is already a global, not changing value",
      name);
    }
    return 0;
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
      return 1;
    }
  }

  sprintf(scripting_error, "out of var space, all %d used",
          MAX_VARS);
  return 0;
}

/**
 * Replace all variables in a string; try longest variables
 * first. Known bug: may replace shorter variables (e.g. &gold instead
 * of &golden).
 */
static void var_replace_107(char** line_p, int scope)
{
  char crap[20];
  int i;
  for (i = 1; i < MAX_VARS; i ++)
    if ((play.var[i].active == 1)
	&& ((play.var[i].scope == VAR_GLOBAL_SCOPE) || (play.var[i].scope == scope)))
      {
	sprintf(crap, "%d", play.var[i].var);
	replace(play.var[i].name, crap, line_p);
      }
}

/**
 * Replace all variables in a string; try longest variables first.
 *
 * Possible improvements:
 * 
 * - Copy play.var[] and sort it by variable length (and avoid the
 *   recursion)
 *
 * - find vars in the string and replace them as-needed (requires
 *   understanding what exactly is an end-of-variable delimiter, if
 *   such a thing exists)
 */
static void var_replace_108(int i, int script, char** line_p, char *prevar)
{
  while (i < MAX_VARS)
    {
      //First, make sure the variable is active.
      //Then, make sure it is in scope,
      //Then, see if the variable name is in the line
      //Then, prevar is null, or if prevar isn't null, see if current variable starts with prevar
      if (play.var[i].active
	  && i == search_var_with_this_scope_108(play.var[i].name, script)
	  && strstr (*line_p, play.var[i].name)
	  && (prevar == NULL || (prevar != NULL && strstr (play.var[i].name, prevar))))
	{
	  //Look for shorter variables
	  var_replace_108(i + 1, script, line_p, play.var[i].name);
	  //we didn't find any, so we replace!
	  char crap[20];
	  sprintf(crap, "%d", play.var[i].var);
	  replace(play.var[i].name, crap, line_p);
	}
      i++;
    }
}

/**
 * Replace all variables (&something) in 'line', with scope 'scope'
 */
void var_replace(char** line_p, int scope)
{
  if (dversion >= 108)
    var_replace_108(1, scope, line_p, NULL);
  else
    var_replace_107(line_p, scope);
}

// XXX: Always set to one number higher than possible simultaneous engines,
// thus leaving room for one sentinel entry (where engine->active == 0)
//
// Current possible engines:
//   - DinkC
//   - DinkLua
struct script_engine script_engines[3];

void scripting_init()
{
  log_debug("[Scripting] scripting_init()");

  int cur_engine_num = 0;
  struct script_engine *script_engine = &script_engines[cur_engine_num];

  if (dversion >= 108)
  {
    if (dinklua_enabled)
    {
      dinklua_initialize(script_engine);
      
      cur_engine_num++;
      script_engine = &script_engines[cur_engine_num];
    }
  }
  else if (dinklua_enabled)
  {
    log_warn("Lua support is disabled when running in 1.07 compatibility mode");
  }

  if (dinkc_enabled)
    dinkc_init(script_engine);

  // Configure engine sentinel entry
  cur_engine_num++;
  script_engine = &script_engines[cur_engine_num];
  script_engine->active = 0;
}

void scripting_quit()
{
  log_debug("[Scripting] scripting_quit()");

  if (dinklua_enabled)
    dinklua_quit();

  if (dinkc_enabled)
    dinkc_quit();

  // Clean up engine name and file extensions
  struct script_engine *script_engine;
  for (int i = 0; (script_engine = &script_engines[i])->active; i++)
  {
    for (int j = 0; script_engine->extensions[j]; j++)
    {
      free(script_engine->extensions[j]);
    }
    free(script_engine->extensions);
    free(script_engine->name);
  }
}

int scripting_load_script(const char filename[15], int sprite, /*bool*/int set_sprite)
{
  log_debug("[Scripting] scripting_load_script(%s, %d, %d)", filename, sprite, set_sprite);
  log_info("[Scripting] LOADING %s", filename);

  char script_path[100];
  char *script_full_path;
  int script_found = 0;
  struct script_engine *script_engine;
  for (int i = 0; (script_engine = &script_engines[i])->active; i++)
  {
    for (int j = 0; script_engine->extensions[j]; j++)
    {
      sprintf(script_path, "story/%s.%s", filename, script_engine->extensions[j]);
      script_full_path = paths_dmodfile(script_path);
      log_debug("[Scripting] Checking %s", script_full_path);

      if (access(script_full_path, F_OK) == 0)
      {
        script_found = 1;
        break;
      }
      
      free(script_full_path);
    }

    if (script_found)
      break;
    
    for (int j = 0; script_engine->extensions[j]; j++)
    {
      sprintf(script_path, "story/%s.%s", filename, script_engine->extensions[j]);
      script_full_path = paths_fallbackfile(script_path);
      log_debug("[Scripting] Checking %s", script_full_path);

      if (access(script_full_path, F_OK) == 0)
      {
        script_found = 1;
        break;
      }
      
      free(script_full_path);
    }

    if (script_found)
      break;
  }

  if (!script_found)
  {
    log_error("Could not load script %s, no candidate file(s) found for available script engines.", filename);

    if (!script_engines[0].active)
    {
      log_error("No script engines enabled!");
    }
    else
    {
      log_debug("Available script engines:");

      for (int i = 0; (script_engine = &script_engines[i])->active; i++)
      {
        char exts[50] = "";
        for (int j = 0; script_engine->extensions[j]; j++)
        {
          if (j == 0)
            sprintf(exts, ".%s", script_engine->extensions[j]);
          else
            sprintf(exts, "%s, .%s", exts, script_engine->extensions[j]);
        }
        log_debug("\t- %s, supported extensions: %s", script_engine->name, exts);
      }
    }
    return 0;
  }

  int script = 0;
  {
    int k = 1;
    for (; k < MAX_SCRIPTS; k++)
      if (sinfo[k] == NULL)
	break;
    script = k;
  }

  if (script == MAX_SCRIPTS)
  {
    log_error("Couldn't find unused buffer for script.");
    return 0;
  }

  log_debug("Found script %s, using engine %s, script slot %d", script_full_path,
           script_engine->name, script);

  sinfo[script] = XZALLOC(struct scriptinfo);
  if (sinfo[script] == NULL)
  {
    log_error("Couldn't allocate script %d.", script);
    free(script_full_path);
    return 0;
  }
  memset(sinfo[script], 0, sizeof(struct scriptinfo));
  
  {
    void *data;
    int allocate_retval = script_engine->allocate_data(&data);

    if (allocate_retval == 0)
    {
      free(sinfo[script]);
      sinfo[script] = NULL;
      free(script_full_path);
      return 0;
    }
    sinfo[script]->data = data;
  }

  sinfo[script]->engine = script_engine;
  sinfo[script]->name = strdup(filename); strtolower(sinfo[script]->name);
  sinfo[script]->sprite = sprite;
  {
    int load_retval = script_engine->load_script(script_full_path, script);
    free(script_full_path);

    if (load_retval == 0)
    {
      free(sinfo[script]->name);
      if (sinfo[script]->data != NULL)
        script_engine->free_data(sinfo[script]->data);
      free(sinfo[script]);
      sinfo[script] = NULL;
      return 0;
    }
  }
  
  if (set_sprite && sprite != 0 && sprite != 1000)
    spr[sprite].script = script;
  
  return script;
}

/**
 * Run main() for all active sprites on screen
 */
void scripting_init_scripts()
{
  log_debug("[Scripting] scripting_init_scripts()");
  
  int k = 1;
  for (; k < MAX_SCRIPTS; k++)
  {
    if (sinfo[k] != NULL && sinfo[k]->sprite != 0
        /* don't go out of bounds in spr[300], e.g. when sprite == 1000: */
        && sinfo[k]->sprite < MAX_SPRITES_AT_ONCE
        && spr[sinfo[k]->sprite].active)
	{
	  if (scripting_proc_exists(k, "main"))
      {
        log_debug("Screendraw: running main of script %s..", sinfo[k]->name);
        scripting_run_proc(k, "main");
      }
	}
  }
}

int scripting_proc_exists(int script, const char* proc)
{
  log_debug("[Scripting] scripting_proc_exists(%d, %s)", script, proc);
  
  if (sinfo[script] == NULL)
    return 0;

  return sinfo[script]->engine->script_proc_exists(script, proc);
}

int scripting_run_proc(int script, const char* proc)
{
  log_debug("[Scripting] scripting_run_proc(%d, %s)", script, proc);
  
  if (sinfo[script] == NULL)
    return 0;

  return sinfo[script]->engine->run_script_proc(script, proc);
}

void scripting_resume_script(int script)
{
  log_debug("[Scripting] scripting_resume_script(%d)", script);
  
  if (sinfo[script] == NULL)
    return;

  sinfo[script]->engine->resume_script(script);
}

void scripting_kill_scripts_owned_by(int sprite)
{
  log_debug("[Scripting] scripting_kill_scripts_owned_by(%d)", sprite);

  int i;
  for (i = 1; i < MAX_SCRIPTS; i++)
  {
    if (sinfo[i] != NULL)
    {
      if (sinfo[i]->sprite == sprite)
      {
        scripting_kill_script(i);
      }
    }
  }
}

/**
 * Kill all scripts except those attached to pseudo-sprite 1000, which
 * is meant to survive across screen changes
 * (kill_all_scripts_for_real(...) is more brutal)
 *
 * Used by gfx_tiles only
 */
void scripting_kill_all_scripts(void)
{
  log_debug("[Scripting] scripting_kill_all_scripts()");

  /* Kill scripts (except if attached to pseudo-sprite 1000) */
  int k = 1;
  for (; k < MAX_SCRIPTS; k++)
  {
    if (sinfo[k] != NULL)
      if (sinfo[k]->sprite != 1000)
        scripting_kill_script(k);
  }
  
  /* Kill pending callbacks (except if attached to pseudo-sprite 1000) */
  for (k = 1; k < MAX_CALLBACKS; k++)
  {
    if (callback[k].active
	    && (!(sinfo[callback[k].owner] != NULL)
        && (sinfo[callback[k].owner]->sprite == 1000)))
	{
	  log_debug("Killed callback %d.  (was attached to script %d)",
		        k, callback[k].owner);
	  callback[k].active = 0;
	}
  }
}

void scripting_kill_all_scripts_for_real(void)
{
  log_debug("[Scripting] scripting_kill_all_scripts_for_real()");
  
  int k = 1;
  for (; k < MAX_SCRIPTS; k++)
  {
    if (sinfo[k] != NULL)
      scripting_kill_script(k);
  }
  
  for (k = 1; k <= MAX_CALLBACKS; k++)
  {
    callback[k].active = 0;
  }
}

void scripting_kill_callbacks(int script)
{
  log_debug("[Scripting] scripting_kill_callbacks(%d)", script);
  //Msg("Checking callbacks..");
  //check callbacks

  int i;
  // callbacks from wait() and run_script_by_number()
  for (i = 1; i < MAX_CALLBACKS; i++)
  {
    if (callback[i].active && callback[i].owner == script)
    //      if (compare(callback[i].name, ""))
    {
      log_debug("killed a returning callback, ha!");
      callback[i].active = /*false*/0;
    }
  }

  // callbacks from say_*()
  for (i = 1; i <= last_sprite_created; i++)
  {
    if (spr[i].active && spr[i].brain == 8 && spr[i].callback == script)
    {
      log_debug("Killed sprites callback command");
      spr[i].callback = 0;
    }
  }
}

void scripting_kill_callback(int cb)
{
  log_debug("[Scripting] scripting_kill_callback(%d)", cb);
  
  if (cb >= 0 && cb <= 99)
    callback[cb].active = /*false*/0;
}

void scripting_kill_callbacks_owned_by_script(int script)
{
  log_debug("[Scripting] scripting_kill_callbacks_owned_by_script(%d)", script);
  
  int i = 1;
  for (; i < MAX_CALLBACKS; i++)
    {
      if (callback[i].owner == script)
	{
	  log_debug("Kill_all_callbacks just killed %d for script %d", i, script);
	  //killed callback
	  callback[i].active = /*false*/0;
	}
    }
}

/**
 * 
 * name: name of the procedure() to call
 * n1: wait at least n1 milliseconds before callback
 * n2: wait at most n1+n2 milliseconds before callback
 * script: number of the script currently running
 **/
int scripting_add_callback(const char name[20], int n1, int n2, int script)
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
	  
	  log_debug("Callback added to %d.", k);
	  return k;
	}
  }
  
  log_error("Couldn't add callback, all out of space");
  return 0;
}

/**
 * Run callbacks, order by index. Sets the activation delay if
 * necessary. Kill obsolete callbacks along the way.
 *
 * Callbacks are set by wait() and set_callback_random().
 * 
 * spawn()/external()/etc. use other mechanisms. say_stop*() also use
 * callbacks, but implemented differently (spr[x].callback, processed
 * in updateFrame()).
 **/
void scripting_process_callbacks(void)
{
  //log_debug("[Scripting] scripting_process_callbacks()");
  
  int now = game_GetTicks();
  int i, k;

  for (i = 1; i < MAX_SCRIPTS; i++)
    {
      if (sinfo[i] != NULL)
	{
	  if (sinfo[i]->sprite > 0 && sinfo[i]->sprite != 1000 && spr[sinfo[i]->sprite].active == /*false*/0)
	    {
	      //kill this script, owner is dead
	      log_debug("Killing script %s, owner sprite %d is dead.", sinfo[i]->name, sinfo[i]->sprite);
	      scripting_kill_script(i);
	    }
	}
    }
  
  for (k = 1; k < MAX_CALLBACKS; k++)
    {
      if (callback[k].active)
	{
	  if (callback[k].owner > 0 && sinfo[callback[k].owner] == NULL)
	    {
	      //kill this process, it's owner sprite is 'effin dead.
	      log_debug("Killed callback %s because script %d is dead.",
			k, callback[k].owner);
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
			  scripting_resume_script(callback[k].owner);
			  log_debug("Called script %d from callback %d.",
				    callback[k].owner, k);
			}
		      else
			{
			  log_debug("Called proc %s from callback %d.", callback[k].name, k);
			  
			  //callback defined a proc name
			  scripting_run_proc(callback[k].owner,callback[k].name);
			}
		    }
		}
	    }
	}
    }
}

void scripting_kill_script(int k)
{
  log_debug("[Scripting] scripting_kill_script(%d)", k);

  if (sinfo[k] != NULL)
  {
    int i;

    scripting_kill_callbacks_owned_by_script(k);
    
    // Now let's kill all local vars associated with this script
    for (i = 1; i < MAX_VARS; i++)
    {
      if (play.var[i].active && play.var[i].scope == k)
        play.var[i].active = /*false*/0;
    }
    log_debug("Killed script %s. (num %d)", sinfo[k]->name, k);

    sinfo[k]->engine->kill_script(k);

    if (sinfo[k]->data != NULL)
      sinfo[k]->engine->free_data(sinfo[k]->data);
    if (sinfo[k]->name != NULL)
      free(sinfo[k]->name);
    if (sinfo[k] != NULL)
      free(sinfo[k]);
    sinfo[k] = NULL;
  }
}