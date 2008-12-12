/**
 * DinkC script engine

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2006  Dan Walma
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* compare */

#include "gettext.h"
#define _(String) gettext (String)

#include "dinkc.h"
#include "dinkc_bindings.h"
#include "game_engine.h"
#include "paths.h"
#include "str_util.h"
#include "log.h"

int returnint = 0;
int bKeepReturnInt = 0;
char returnstring[200];
/* Used to tell decipher_string about the currently selected savegame
   in a choice menu; also abuse to tell which key is selected in
   joystick remapping */
unsigned short decipher_savegame = 0;

#define MAX_CALLBACKS 100
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
static struct call_back callback[MAX_CALLBACKS];
/* TODO: Used 1->100 in the game, should it be MAX_CALLBACKS+1 ? */

/* DinkC script buffer */
static char cbuf[64000]; /* TODO: don't use a fixed limit, vulnerable to
		     buffer overflow */
static char *rbuf[MAX_SCRIPTS]; //pointers to buffers we may need

/* Number of reserved ASCII indexes in .d BPE compression format */
#define NB_PAIRS_MAX 128


struct refinfo *rinfo[MAX_SCRIPTS];


/**
 * Decompress a .d DinkC script; also clean newlines. Check
 * contrib/d2c.c for more explanation about the decompression process.
 */
void decompress(FILE *in)
{
  unsigned char stack[NB_PAIRS_MAX+1], pairs[NB_PAIRS_MAX][2];
  short c, top = -1;
  int nb_pairs = 0;
  
  /* Check for optional pair count and pair table */
  if ((c = fgetc(in)) > 127)
    {
      /* Read pairs table */
      nb_pairs = c - 128;
      int i, j;
      for (i = 0; i < nb_pairs; i++)
	{
	  for (j = 0; j < 2; j++)
	    {
	      int c = fgetc(in);
	      if (c == EOF)
		{
		  fprintf(stderr, "decompress: invalid header: truncated pair table\n");
		  return;
		}
	      if (c > i+128)
		{
		  fprintf(stderr, "decompress: invalid header: reference to a pair that is not registered yet\n");
		  return;
		}
	      pairs[i][j] = c;
	    }
	}
    }
  else
    {
      /* Non-compressed file, put back the character we read */
      ungetc(c, in);
    }
  
  char* pc = cbuf + strlen(cbuf);
  for (;;)
    {
      /* Pop byte from stack or read byte from file */
      if (top >= 0)
	c = stack[top--];
      else if ((c = fgetc(in)) == EOF)
	break;
    
      /* Push pair on stack or output byte to file */
      if (c > 127)
	{
	  if ((c-128) >= nb_pairs)
	    {
	      fprintf(stderr, "decompress: invalid body: references non-existent pair\n");
	      break;
	    }
	  stack[++top] = pairs[c-128][1];
	  stack[++top] = pairs[c-128][0];
	}
      else
	{
	  if (c == '\r') c = '\n';
	  if (c == '\t') c = ' ';
	  *pc = c;
	  pc++;
	}
    }
  *pc = '\0';
}

void decompress_nocomp(FILE *in)
{
  int c;
  char* pc = cbuf + strlen(cbuf);
  while ((c = getc(in)) != EOF)
    {
      if (c == '\r') c = '\n';
      if (c == '\t') c = ' ';
      *pc = c;
      pc++;
    }
  *pc = '\0';
}


/**
 * Only load game metadata (timetime). Used when displaying the list
 * of saved games (see decipher_string).
 */
static /*bool*/int load_game_small(int num, char *line, int *mytime)
{
  FILE *f = paths_savegame_fopen(num, "rb");
  if (f == NULL)
    {
      Msg("Couldn't quickload save game %d", num);
      return(/*false*/0);
    }
  else
    {
      //int version = read_lsb_int(f);
      read_lsb_int(f); // avoid compiler warning

      char gameinfo[196];
      fread(gameinfo, 196, 1, f);
      int minutes = read_lsb_int(f);
      fclose(f);

      *mytime = minutes;
      strcpy(line, gameinfo);
      return(/*true*/1);
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
  FILE *in;
  /*bool*/int comp = /*false*/0;
  char tab[10];
  
  Msg("LOADING %s",filename);
  sprintf(tab, "%c",9);
  
  sprintf(temp, "story/%s.d", filename);
  in = paths_dmodfile_fopen(temp, "rb");
  if (in == NULL)
    {
      sprintf(temp, "story/%s.c", filename);
      in = paths_dmodfile_fopen(temp, "rb");
      if (in == NULL)
	{
	  sprintf(temp, "story/%s.d", filename);
	  in = paths_fallbackfile_fopen(temp, "rb");
	  if (in == NULL)
	    {
	      sprintf(temp, "story/%s.c", filename);
	      in = paths_fallbackfile_fopen(temp, "rb");
	      if (in == NULL)
		{
		  Msg("Script %s not found. (checked for .C and .D) (requested by %d?)", temp, sprite);
		  return 0;
		}
	    }
	}
    }
  
  strtoupper(temp);
  Msg("Temp thingie is %c", temp[strlen(temp)-1]);
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
  fclose(in);
  return 0;
  

 found:
  Msg("Loading script %s..", temp);

  script = k;
  rinfo[script] = (struct refinfo *) malloc( sizeof(struct refinfo));
  memset(rinfo[script], 0, sizeof(struct refinfo));
  
  //if compiled
  {
    //load compiled script
    cbuf[0] = 0;

    //Msg("decompressing!");
    if (comp)
      decompress(in);
    else
      decompress_nocomp(in);
    
    fclose(in);
    
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

  return script;
}


int dinkc_execute_one_liner(char* line)
{
  /* Find available script slot */
  int k = 1;
  for (k = 1; k < MAX_SCRIPTS; k++)
    if (rbuf[k] == NULL)
      break;

  if (k < MAX_SCRIPTS)
    {
      rinfo[k] = (struct refinfo*) calloc(1, sizeof(struct refinfo));
      rinfo[k]->sprite = 1000; /* survice screen change */
      rinfo[k]->level = 1; /* skip 'void main(void) {' parsing */
      rbuf[k] = (char*) malloc(255);
      strcpy(rbuf[k], line);
      process_line(k, rbuf[k], 0);
      return returnint;
    }
  else
    return -1;
}


/**
 * Remove leading spaces by shifting 'str' to the left, as much as
 * there is leading spaces.
 */
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
                                        spr[rinfo[script]->sprite].move_active = 0;
                                        if (dversion >= 108)
					  spr[rinfo[script]->sprite].move_nohard = 0;
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
	&& ((play.var[i].scope == DINKC_GLOBAL_SCOPE) || (play.var[i].scope == var_scope))
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
  search_scope[1] = DINKC_GLOBAL_SCOPE; /* then global scope */

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

/**
 * Expand 'variable' in the scope of 'script'; the result is placed
 * back in 'variable'. Only used in function 'get_parms'.
 */
void decipher(char *variable, int script)
{
  // Special vars: &current_sprite and &current_script
  if (compare(variable, "&current_sprite"))
    {
      sprintf(variable, "%d",rinfo[script]->sprite);
      return;
    }
  if (compare(variable, "&current_script"))
    {
      sprintf(variable, "%d",script);
      return;
    }

  //v1.08 special variables.
  if (dversion >= 108)
    {
      if (compare(variable, "&return")) 
	{
	  sprintf(variable, "%d", returnint);
	  return;
	}
      if (compare(variable, "&arg1")) 
	{
	  sprintf(variable, "%d", rinfo[script]->arg1);
	  return;
	}
      if (compare(variable, "&arg2")) 
	{
	  sprintf(variable, "%d", rinfo[script]->arg2);
	  return;
	}
      if (compare(variable, "&arg3")) 
	{
	  sprintf(variable, "%d", rinfo[script]->arg3);
	  return;
	}
      if (compare(variable, "&arg4")) 
	{
	  sprintf(variable, "%d", rinfo[script]->arg4);
	  return;
	}
      if (compare(variable, "&arg5"))
	{
	  sprintf(variable, "%d", rinfo[script]->arg5);
	  return;
	}
      if (compare(variable, "&arg6")) 
	{
	  sprintf(variable, "%d", rinfo[script]->arg6);
	  return;
	}
      if (compare(variable, "&arg7")) 
	{
	  sprintf(variable, "%d", rinfo[script]->arg7);
	  return;
	}
      if (compare(variable, "&arg8"))
	{
	  sprintf(variable, "%d", rinfo[script]->arg8);
	  return;
	}
      if (compare(variable, "&arg9")) 
	{
	  sprintf(variable, "%d", rinfo[script]->arg9);
	  return;
	}
    }

  // Check in local and global variables
  int i = search_var_with_this_scope(variable, script);
  if (i != -1)
    sprintf(variable, "%d", play.var[i].var);
}


/**
 * Replace all variables in a string; try longest variables
 * first. Known bug: may replace shorter variables (e.g. &gold instead
 * of &golden).
 */
void var_replace_107(char* line, int scope)
{
  char crap[20];
  int i;
  for (i = 1; i < MAX_VARS; i ++)
    if ((play.var[i].active == 1)
	&& ((play.var[i].scope == DINKC_GLOBAL_SCOPE) || (play.var[i].scope == scope)))
      {
	sprintf(crap, "%d", play.var[i].var);
	replace(play.var[i].name, crap, line);
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
void var_replace_108(int i, int script, char *line, char *prevar)
{
  while (i < MAX_VARS)
    {
      //First, make sure the variable is active.
      //Then, make sure it is in scope,
      //Then, see if the variable name is in the line
      //Then, prevar is null, or if prevar isn't null, see if current variable starts with prevar
      if (play.var[i].active
	  && i == search_var_with_this_scope_108(play.var[i].name, script)
	  && strstr (line, play.var[i].name)
	  && (prevar == NULL || (prevar != NULL && strstr (play.var[i].name, prevar))))
	{
	  //Look for shorter variables
	  var_replace_108(i + 1, script, line, play.var[i].name);
	  //we didn't find any, so we replace!
	  char crap[20];
	  sprintf (crap, "%d", play.var[i].var);
	  replace (play.var[i].name, crap, line);
	}
      i++;
    }
}

/**
 * Replace all variables (&something) in 'line', with scope 'scope'
 */
void var_replace(char* line, int scope)
{
  if (dversion >= 108)
    var_replace_108(1, scope, line, NULL);
  else
    var_replace_107(line, scope);
}


/**
 * Similar to decipher, plus:
 * - expand special choice variables &savegameinfo and &buttoninfo
 * - it can replace several variables in the same string
 * - with v1.07 it has a prefix bug (see var_replace_107)
 */
void decipher_string(char line[200], int script)
{
  char buffer[20];
  char crab[100];
  int mytime;
  
  /* Replace all valid variables in 'line' */
  var_replace(line, script);
  
  if ((strchr(line, '&') != NULL) && (script != 0))
    {
      sprintf(buffer, "%d", rinfo[script]->sprite); replace("&current_sprite", buffer, line);
      sprintf(buffer, "%d", script);                replace("&current_script", buffer, line);

      if (dversion >= 108)
	{
	  //v1.08 special variables.
	  sprintf(buffer, "%d", returnint);           replace("&return", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg1); replace("&arg1", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg2); replace("&arg2", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg3); replace("&arg3", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg4); replace("&arg4", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg5); replace("&arg5", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg6); replace("&arg6", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg7); replace("&arg7", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg8); replace("&arg8", buffer, line);
	  sprintf(buffer, "%d", rinfo[script]->arg9); replace("&arg9", buffer, line);
	}

      if (decipher_savegame != 0)
	{
	  if      (play.button[decipher_savegame] == 1)   replace("&buttoninfo", "Attack", line);
	  else if (play.button[decipher_savegame] == 2)   replace("&buttoninfo", "Talk/Examine", line);
	  else if (play.button[decipher_savegame] == 3)   replace("&buttoninfo", "Magic", line);
	  else if (play.button[decipher_savegame] == 4)   replace("&buttoninfo", "Item Screen", line);
	  else if (play.button[decipher_savegame] == 5)   replace("&buttoninfo", "Main Menu", line);
	  else if (play.button[decipher_savegame] == 6)   replace("&buttoninfo", "Map", line);
	  else if (play.button[decipher_savegame] == 7)   replace("&buttoninfo", "Unused", line);
	  else if (play.button[decipher_savegame] == 8)   replace("&buttoninfo", "Unused", line);
	  else if (play.button[decipher_savegame] == 9)   replace("&buttoninfo", "Unused", line);
	  else if (play.button[decipher_savegame] == 10)  replace("&buttoninfo", "Unused", line);
	  else replace("&buttoninfo", _("Error: not mapped"), line);
	}
    }

  if ((decipher_savegame != 0) && compare(line, "&savegameinfo"))
    {
      char crap[20];
      sprintf(crap, "save%d.dat", decipher_savegame);
      if (load_game_small(decipher_savegame, crab, &mytime) == 1)
	{
	  sprintf(line, _("Slot %d - %d:%02d - %s"), decipher_savegame, (mytime / 60),
		  mytime - ((mytime / 60) * 60) , crab);
	  //sprintf(line, "In Use");
	}
      else
	{
	  sprintf(line, _("Slot %d - Empty"), decipher_savegame);
	}
    }
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

void kill_callback(int cb)
{
  if (cb >= 0 && cb <= 99)
    callback[cb].active = /*false*/0;
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
  if (rinfo[script] == NULL || rbuf == NULL)
    {
      Msg("  ERROR:  Tried to read script %d, it doesn't exist.", script);
      return /*false*/0;
    }

  if (rinfo[script]->current >= rinfo[script]->end)
    {
      //at end of buffer
      return /*false*/0;
    }

  line[0] = '\0';
  char *pc = line;

  int k;
  for (k = rinfo[script]->current; k < rinfo[script]->end; k++)
    {
      *pc = rbuf[script][k];
      pc++;
      rinfo[script]->current++;
      
      if (rbuf[script][k] == '\n') // \r were replaced in decompress()
	{
	  *pc = '\0';
	  return /*true*/1;
	}
    }

  //at end of buffer
  *pc = '\0';
  return /*false*/0;
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
	  /* don't go out of bounds in spr[300], e.g. when sprite == 1000: */
	  && rinfo[k]->sprite < MAX_SPRITES_AT_ONCE
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


void run_script(int script)
{
  int result;
  char line[200];

  /* keep 'return' value? */
  if (dversion >= 108)
    {
      if (bKeepReturnInt == 1)
	{
	  bKeepReturnInt = 0;
	}
      else
	{
	  returnint = 0;
	}
    }
  else
    {
      returnint = 0;
    }
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

  int doelse_once = 0;
  while (read_next_line(script, line))
    {
      while (1)
	{
	  strip_beginning_spaces(line);
	  if (strcmp(line, "\n") == 0)
	    break;

	  
	  int doelse = 0;
	  if (doelse_once == 1)
	    {
	      doelse = 1;
	      doelse_once = 0;
	    }
	  result = process_line(script, line, doelse);
	  

	  if (result == DCPS_DOELSE_ONCE)
	    {
	      doelse_once = 1;
	      /* now process the rest of the line */
	    }

	  if (result == DCPS_YIELD)
	    {
	      /* Quit script: */
	      if (debug_mode)
		Msg("giving script the boot");
	      return;
	    }
	  
	  if (result == DCPS_GOTO_NEXTLINE)
	    break;

	  /* else result == DCPS_CONTINUE */
	}
    }

  if (rinfo[script] != NULL && rinfo[script]->proc_return != 0)
    {
      run_script(rinfo[script]->proc_return);
      kill_script(script);
    }
}

/**
 * Copy the word number 'word' present in 'line' to the 'result'
 * string. Words are separated by one or more spaces and count from 1
 * (i.e. not 0).
 */
void get_word(char line[300], int word, char *result)
{
  int cur_word = 1;
  result[0] = '\0';

  /* find word */
  char* pc = line;
  while (*pc != '\0')
    {
      if (cur_word == word)
	break;
      if (*pc == ' ')
	{
	  cur_word++;
	  while(*pc == ' ' && *pc != '\0')
	    pc++;
	}
      else
	{
	  while(*pc != ' ' && *pc != '\0')
	    pc++;
	}
    }
  /* copy word - either we're on the right word and will copy it,
     either we're at the end of string and will copy an empty word */
  char* pcr = result;
  while(*pc != '\0' && *pc != ' ')
    {
      *pcr = *pc;
      pcr++;
      pc++;
    }
  *pcr = '\0';
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

/**
 * Make new global functions (v1.08)
 */
void make_function (char file[10], char func[20])
{
  //See if it already exists

  int exists = 0;
  int i;
  for (i = 0; strlen (play.func[i].func) > 0 && i < 100; i++)
    {
      if (compare (func, play.func[i].func))
	{
	  exists = 1;
	  break;
	}
    }
  if (exists == 1)
    {
      strncpy (play.func[i].file, file, 10);
    }
  else
    {
      strncpy (play.func[0].file, file, 10);
      strncpy (play.func[0].func, func, 20);
    }
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
                if (scope != DINKC_GLOBAL_SCOPE)
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

/**
 * (re)Define variable
 *
 * name: variable name
 * newname: new value (unless that's a function call, cf. 'rest')
 * math: operator (one of '=', '+', '-', '*', '/')
 * script: in-memory script identifier
 * rest: text of the script after the operator (left-trimmed)
 */
void var_equals(char name[20], char newname[20], char math, int script, char rest[200])
{
  int newval = 0;
  struct varman *lhs_var = NULL;
  
  /** Ensure left-hand side is an existing variable **/
  if (name[0] != '&')
    {
      Msg("ERROR (var equals): Unknown var %s in %s offset %d.",
	  name, rinfo[script]->name, rinfo[script]->current);
      return;
    }
  /* Find the variable slot */
  {
    int k = search_var_with_this_scope(name, script);
    if (k != -1)
      lhs_var = &(play.var[k]);
    
    if (lhs_var == NULL) /* not found */
      {
	Msg("ERROR: (var equals2) Unknown var %s in %s offset %d.",
	    name, rinfo[script]->name, rinfo[script]->current);
	return;
      }
  }

  /** Analyse right-hand side **/
  /* check if right-hand side is a function */
  if (strchr(rest, '(') != NULL)
    {
      process_line(script, rest, /*false*/0);
      newval = returnint;
      goto next2;
    }

  /* check if right-hand side is a variable to copy */
  /* remove trailing ';' */
  if (strchr(newname, ';') != NULL)
    replace(";", "", newname);
  /* look for existing variable */
  {
    int k2 = search_var_with_this_scope(newname, script);
    if (k2 != -1)
      {
	newval = play.var[k2].var;
	//found var
	goto next2;
      }
  }
  /* also check special variables */
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
  if (dversion >= 108)
    {
      //v1.08 special variables.
      if (compare (newname, "&return"))
	{
	  newval = returnint;
	  goto next2;
	}
      if (compare (newname, "&arg1"))
	{
	  newval = rinfo[script]->arg1;
	  goto next2;
	}
      if (compare (newname, "&arg2"))
	{
	  newval = rinfo[script]->arg2;
	  goto next2;
	}
      if (compare (newname, "&arg3"))
	{
	  newval = rinfo[script]->arg3;
	  goto next2;
	}
      if (compare (newname, "&arg4"))
	{
	  newval = rinfo[script]->arg4;
	  goto next2;
	}
      if (compare (newname, "&arg5"))
	{
	  newval = rinfo[script]->arg5;
	  goto next2;
	}
      if (compare (newname, "&arg6"))
	{
	  newval = rinfo[script]->arg6;
	  goto next2;
	}
      if (compare (newname, "&arg7"))
	{
	  newval = rinfo[script]->arg7;
	  goto next2;
	}
      if (compare (newname, "&arg8"))
	{
	  newval = rinfo[script]->arg8;
	  goto next2;
	}
      if (compare (newname, "&arg9"))
	{
	  newval = rinfo[script]->arg9;
	  goto next2;
	}
    }
  /* otherwise, assume right-hand side is an integer */
  newval = atol(newname);


next2:
  /* Apply the right operation */
  if (math == '=')
    lhs_var->var = newval;
  if (math == '+')
    lhs_var->var += newval;
  if (math == '-')
    lhs_var->var -= newval;
  if (math == '/')
    lhs_var->var = lhs_var->var / newval;
  if (math == '*')
    lhs_var->var = lhs_var->var * newval;
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

void dinkc_init()
{
  dinkc_bindings_init();
}

void dinkc_quit()
{
  dinkc_bindings_quit();
}
