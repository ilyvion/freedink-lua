/**
 * DinkC script engine

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
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

#include "dinkc.h"
#include "dinkc_bindings.h"
#include "game_engine.h"
#include "paths.h"
#include "str_util.h"
#include "log.h"

int returnint = 0;
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
struct call_back callback[MAX_CALLBACKS];
/* TODO: Used 1->100 in the game, should it be MAX_CALLBACKS+1 ? */

/* DinkC script buffer */
char cbuf[64000]; /* TODO: don't use a fixed limit, vulnerable to
		     buffer overflow */
char *rbuf[MAX_SCRIPTS]; //pointers to buffers we may need

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
	      return;
	    }
	  stack[++top] = pairs[c-128][1];
	  stack[++top] = pairs[c-128][0];
	}
      else
	{
	  if (c == '\r') c = '\n';
	  if (c == 9) c = ' ';
	  strchar(cbuf,c);
	}
    }
}


void decompress_nocomp(FILE *in)
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



struct player_short_info
{
  int version;
  char gameinfo[196];
  int minutes;
};
struct player_short_info short_play;

/**
 * Only load game metadata (timetime). Used when displaying the list
 * of saved games (see decipher_string).
 */
static /*bool*/int load_game_small(int num, char *line, int *mytime)
{
  FILE *fp = paths_savegame_fopen(num, "rb");
  if (fp == NULL)
    {
      Msg("Couldn't quickload save game %d", num);
      return(/*false*/0);
    }
  else
    {
      fread(&short_play, sizeof(struct player_short_info), 1, fp);
      fclose(fp);
      *mytime = short_play.minutes;
      strcpy(line, short_play.gameinfo);
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


/**
 * Expand 'variable' in the scope of 'script'
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

  // Check in local and global variables
  int i;
  for (i = 1; i < MAX_VARS; i ++)
    {
      if (play.var[i].active == 1
	  && ((play.var[i].scope == DINKC_GLOBAL_SCOPE) || (play.var[i].scope == script))
	  && (compare(play.var[i].name, variable)))
	{
	  sprintf(variable, "%d",play.var[i].var);
	  return;
	}
    }
}

/**
 * Similar to decipher, plus expand special choice variables
 * &savegameinfo and &buttoninfo
 */
void decipher_string(char line[200], int script)
{
  char crap[20];
  char buffer[20];
  char crab[100];
  int mytime;
  int i;
  
  for (i = 1; i < MAX_VARS; i ++)
    {
      if ((play.var[i].active == 1)
	  && ((play.var[i].scope == DINKC_GLOBAL_SCOPE) || (play.var[i].scope == script)))
	{
	  sprintf(crap, "%d", play.var[i].var);
	  replace(play.var[i].name, crap, line);
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
	}
    }

  if ((decipher_savegame != 0) && compare(line, "&savegameinfo"))
    {
      sprintf(crap, "save%d.dat", decipher_savegame);
      if (load_game_small(decipher_savegame, crab, &mytime) == 1)
	{
	  sprintf(line, "Slot %d - %d:%d - %s",decipher_savegame, (mytime / 60),
		  mytime - ((mytime / 60) * 60) , crab);
	  //sprintf(line, "In Use");
	}
      else
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
