/**
 * Link game engine and DinkC script engine

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

#include "dinkc_bindings.h"

#include <stdio.h>
#include <time.h>
#include <math.h>

#include "game_engine.h"
#include "dinkvar.h"
#include "dinkc.h"
#include "freedink.h"
#include "gfx.h"
#include "gfx_fonts.h"
#include "gfx_sprites.h"
#include "gfx_tiles.h"
#include "bgm.h"
#include "sfx.h"
#include "str_util.h"
#include "paths.h"
#include "log.h"

/* store current procedure arguments expanded values of type 'int' (see get_parms) */
static long nlist[10];
/* store current procedure arguments of type 'string' (idem) */
static char slist[10][200];


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


/**
 * Process DinkC dialog choice stanza
 */
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


/**
 * Utility function for 'process_line', to separate and store the current procedure arguments.
 *
 * proc_name: named of the called function
 * script: script id
 * str_params: string to parse (what was after the function name)
 * spec: describe the function's parameters:
 *    1=int
 *    2=string
 *    0=no more args (10 args max)
 *
 * Return: 0 if parse error, 1 if success
 */
int get_parms(char proc_name[20], int script, char *str_params, int spec[10])
{
  char crap[1024];

  strip_beginning_spaces(str_params);
  if (str_params[0] == '(')
    {
      //Msg("Found first (.");
      str_params++;
    }
  else
    {
      Msg("Missing ( in %s, offset %d.", rinfo[script]->name, rinfo[script]->current);
      return 0;
    }

  int i;
  for (i = 0; i < 10; i++)
    {
      strip_beginning_spaces(str_params);
      
      if (spec[i] == 1) // type=int
	{
	  // Get next parameter (until ',' or ')' is reached)
	  if (strchr(str_params, ',') != NULL)
	    separate_string(str_params, 1, ',', crap);
	  else if (strchr(str_params, ')') != NULL)
	    separate_string(str_params, 1, ')', crap);

	  // move to next param
	  str_params += strlen(crap);
	  
	  if (crap[0] == '&')
	    {
	      replace(" ", "", crap);
	      //      Msg("Found %s, 1st is %c",crap, crap[0]);
	      decipher(crap, script);
	    }
	  // store parameter of type 'int'
	  nlist[i] = atol(crap);
	}
      else if (spec[i] == 2) // type=string
	{
	  // Msg("Checking for string..");
	  separate_string(str_params, 2, '"', crap);
	  // move to next param
	  str_params += strlen(crap)+2;

	  // store parameter of type 'string'
	  strcpy(slist[i], crap);
	}

      if ((i+1) == 10 || spec[i+1] == 0) // this was the last arg
	{
	  //finish
	  strip_beginning_spaces(str_params);
	  
	  if (str_params[0] == ')')
	    {
	      str_params++;
	    }
	  else
	    {
	      Msg("Missing ')' in %s, offset %d.", rinfo[script]->name, rinfo[script]->current);
	      return 0;
	    }
	  strip_beginning_spaces(str_params);
	  return 1;
	}

      //got a parm, but there is more to get, lets make sure there is a comma there
      strip_beginning_spaces(str_params);

      if (str_params[0] == ',')
	{
	  str_params++;
	}
      else
	{
	  Msg("Procedure %s does not take %d parms in %s, offset %d. (%s?)", proc_name, i+1, rinfo[script]->name, rinfo[script]->current, str_params);
	  return 0;
	}
    }
  return 1;
}


/**
 * Return values:
 * - 0: continue processing script
 * - 2: stop processing script (may come back later via callbacks)
 * - 3 & 4: have something to do with if/else ?
 **/
int process_line (int script, char *s, /*bool*/int doelse)
{
  char *h, *p;
  int i;
  char line[200];
  char ev[15][100];
  char temp[100];
  char first[2];
  int sprite = 0;
  int kk;
  
  if (rinfo[script]->level < 1)
    rinfo[script]->level = 1;


  for (kk = 1; kk < 15; kk++)
    ev[kk][0] = 0;
  h = s;
  if (h[0] == '\0')
    return 0;
  
  if ((h[0] == '/') && (h[1] == '/'))
    {
      //Msg("It was a comment!");
      goto bad;
    }


  for (i = 1; i <= 14; i++)
    {
      if (separate_string(h, i, ' ', ev[i]) == /*false*/0)
	goto pass;
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
                if (ev[1][strlen(ev[1]) -1] == ':' && strlen(ev[2]) < 2)
                {
		  if (dversion >= 108)
		    {
		      /* Attempt to avoid considering:
			   say("bonus: 5 points", 1); // would not display any text at all!
			 as a label */
		      if (strncmp (ev[1], "say", 3) != 0)
			return(0); //its a label
		    }
		  else
		    {
		      return(0); //its a label
		    }
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
                         * multi-character constants should be
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

                                strcpy_nooverlap(s, h);

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

                        strcpy_nooverlap(s, h);
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
                strcpy_nooverlap(s, h);
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
                        strcpy_nooverlap(s, h);
                        return(3);
        }

                //if (debug_mode) Msg("%s",s);


                if (compare(ev[1], "void"))
                {
                        Msg("ERROR: Missing } in %s, offset %d.", rinfo[script]->name,rinfo[script]->current);
                        strcpy_nooverlap(s, h);
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
                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "reset_timer"))
                {
                        h = &h[strlen(ev[1])];
                        time(&time_start);
                        play.minutes = 0;
                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
                        return(0);
                }



                if (compare(ev[1], "show_bmp"))
                {
                        Msg("showing BMP");
                        h = &h[strlen(ev[1])];
                        int p[20] = {2,1,1,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
                                wait4b.active = /*false*/0;
                                show_bmp(slist[0], nlist[1], nlist[2], script);
                        }

                        strcpy_nooverlap(s, h);
                        return(2);
                }


                if (compare(ev[1], "wait_for_button"))
                {
                        Msg("waiting for button with script %d", script);
                        h = &h[strlen(ev[1])];
                        strcpy_nooverlap(s, h);
                        wait4b.script = script;
                        wait4b.active = /*true*/1;
                        wait4b.button = 0;
                        return(2);
                }

                if (compare(ev[1], "stop_wait_for_button"))
                {
                        wait4b.active = /*false*/0;

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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                                strcpy_nooverlap(s, h);

                                return(2);

                        }

                        strcpy_nooverlap(s, h);
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
                                strcpy_nooverlap(s, h);

                                return(2);

                        }

                        strcpy_nooverlap(s, h);
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
                                strcpy_nooverlap(s, h);

                                return(2);

                        }

                        strcpy_nooverlap(s, h);
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
                                strcpy_nooverlap(s, h);
                                return(0);

                        }

                        strcpy_nooverlap(s, h);
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
                        screenlock = 0;
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
                                strcpy_nooverlap(s, h);
                                kill_returning_stuff(script);
                                add_callback("",nlist[0],0,script);

                                return(2);
                        }

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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
                        strcpy_nooverlap(s, h);
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
                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "run_script_by_number"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,2,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  if (locate (nlist[0], slist[1]))
			    run_script (nlist[0]);
                        }

                        strcpy_nooverlap(s, h);
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
                        strcpy_nooverlap(s, h);
                        return(0);
                }




                if (compare(ev[1], "draw_background"))
                {
                        // (sprite, direction, until, nohard);
                        draw_map_game_background();
                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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
                                        strcpy_nooverlap(s, h);
                                        return(0);
                                      }
                                  }
                              }
                            Msg("Playing midi %s.", slist[0]);
                            PlayMidi(slist[0]);
                          }
                        strcpy_nooverlap(s, h);
                        return(0);
                }
                if (compare(ev[1], "stopmidi"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        StopMidi();
                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "kill_all_sounds"))
                {
            kill_repeat_sounds_all();
                        strcpy_nooverlap(s, h);
                        return(0);

                }

                if (compare(ev[1], "turn_midi_off"))
                {
                        midi_active = /*false*/0;
                        strcpy_nooverlap(s, h);
                        return(0);

                }
                if (compare(ev[1], "turn_midi_on"))
                {
                        midi_active = /*true*/1;
                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                  strcpy_nooverlap(s, h);
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

                  strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
                        return(0);
                }

                if (compare(ev[1], "game_exist"))
                {
                        // (sprite, direction, until, nohard);
                        h = &h[strlen(ev[1])];
                        int p[20] = {1,0,0,0,0,0,0,0,0,0};
                        if (get_parms(ev[1], script, h, p))
                        {
			  FILE *fp;
			  if ((fp = paths_savegame_fopen(nlist[0], "rb")) != NULL)
			    {
			      fclose(fp);
			      returnint = 1;
			    }
			  else
			    {
			      returnint = 0;
			    }
                        }

                        strcpy_nooverlap(s, h);
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
                                strcpy_nooverlap(s, h);
                                if (debug_mode) Msg("Move_stop: Sprite %d, dir %d, num %d", nlist[0],nlist[1], nlist[2]);
                                return(2);

                        }

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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
                                strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
                        return(0);
                }


                if (compare(ev[1], "get_version"))
                {
                        h = &h[strlen(ev[1])];
                        returnint = dversion;
                        strcpy_nooverlap(s, h);
                        return(0);
                }

                /* Used in the original game to choose between 2 CD
                   tracks/sounds. "This command is included for
                   compatibility" (TM) */
                if (compare(ev[1], "get_burn"))
                {
                        h = &h[strlen(ev[1])];
                        returnint = 1;
                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                        strcpy_nooverlap(s, h);
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

                                if (nlist[1] > 0)
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
                                                                                        if (rinfo[spr[nlist[0]].script] == NULL)
                                                                                        {
											        Msg("Compare sprite script says: script %d for sprite %d was already killed!.", nlist[0], spr[nlist[0]].script);
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
                                                                                bKeepReturnInt = 1; /* v1.08 */
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
                                                                        strcpy_nooverlap(s, h);
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
                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "+="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);
                                                                        var_equals(ev[1], ev[3], '+', script, h);
                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "*="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);
                                                                        var_equals(ev[1], ev[3], '*', script, h);
                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }



                                                                if (compare(ev[2], "-="))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[2];
                                                                        strip_beginning_spaces(h);

                                                                        var_equals(ev[1], ev[3], '-', script, h);

                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }


                                                                if (compare(ev[2], "/")
								    || (dversion >= 108 && compare(ev[2], "/=")))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[1];
                                                                        strip_beginning_spaces(h);

                                                                        var_equals(ev[1], ev[3], '/', script, h);

                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }

                                                                if (compare(ev[2], "*"))
                                                                {
                                                                        h = &h[strlen(ev[1])];
                                                                        strip_beginning_spaces(h);
                                                                        h = &h[1];
                                                                        strip_beginning_spaces(h);

                                                                        var_equals(ev[1], ev[3], '*', script, h);

                                                                        strcpy_nooverlap(s, h);
                                                                        return(0);
                                                                }


  /*********************************/
  /** New DinkC commands in v1.08 **/
  /**                             **/
  /*********************************/

  if (dversion >= 108)
    {
    //redink1 added
    if (compare (ev[1], "sp_freeze"))

      {
	h = &h[strlen (ev[1])];

	// Msg("Running busy, h is %s", h);    
	int
	p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {

	    // Set the value
	    if (nlist[1] == 0)

	      {
		spr[nlist[0]].freeze = 0;
	      }

	    else if (nlist[1] == 1)

	      {
		spr[nlist[0]].freeze = script;
	      }

	    // Return the value
	    if (spr[nlist[0]].freeze > 0)

	      {
		returnint = 1;
	      }

	    else

	      {
		returnint = 0;
	      }
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "get_time_game"))

      {
	h = &h[strlen (ev[1])];
	time_t ct;
	time (&ct);
	returnint = play.minutes + (difftime (ct, time_start) / 60);
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "get_time_real"))

      {
	h = &h[strlen (ev[1])];
	char
	  mytime[5];
	time_t ct;
	struct tm *
	  time_now;
	time (&ct);
	time_now = localtime (&ct);
	strftime (mytime, 5, "%M", time_now);
	returnint = atoi (mytime);
	strftime (mytime, 5, "%H", time_now);
	returnint += 60 * atoi (mytime);
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "get_date_year"))

      {
	h = &h[strlen (ev[1])];
	char
	  mytime[5];
	time_t ct;
	struct tm *
	  time_now;
	time (&ct);
	time_now = localtime (&ct);
	strftime (mytime, 5, "%Y", time_now);
	returnint = atoi (mytime);
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "get_date_month"))

      {
	h = &h[strlen (ev[1])];
	char
	  mytime[5];
	time_t ct;
	struct tm *
	  time_now;
	time (&ct);
	time_now = localtime (&ct);
	strftime (mytime, 5, "%m", time_now);
	returnint = atoi (mytime);
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "get_date_day"))

      {
	h = &h[strlen (ev[1])];
	char
	  mytime[5];
	time_t ct;
	struct tm *
	  time_now;
	time (&ct);
	time_now = localtime (&ct);
	strftime (mytime, 5, "%d", time_now);
	returnint = atoi (mytime);
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "math_abs"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    returnint = abs (nlist[0]);
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    /*if (compare(ev[1], "math_sin"))
       {
       h = &h[strlen(ev[1])];
       int p[20] = {1,0,0,0,0,0,0,0,0,0};  
       if (get_parms(ev[1], script, h, p))
       {
       returnint = sin((double)nlist[0]);
       }
       strcpy(s, h);  
       return(0);
       }

       //redink1 added this function
       if (compare(ev[1], "math_cos"))
       {
       h = &h[strlen(ev[1])];
       int p[20] = {1,0,0,0,0,0,0,0,0,0};  
       if (get_parms(ev[1], script, h, p))
       {
       returnint = cos((double)nlist[0]);
       }
       strcpy(s, h);  
       return(0);
       }

       //redink1 added this function
       if (compare(ev[1], "math_tan"))
       {
       h = &h[strlen(ev[1])];
       int p[20] = {1,0,0,0,0,0,0,0,0,0};  
       if (get_parms(ev[1], script, h, p))
       {
       returnint = tan((double)nlist[0]);
       }
       strcpy(s, h);  
       return(0);
       } */

    //redink1 added this function
    if (compare (ev[1], "math_sqrt"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    returnint = sqrt ((double) abs (nlist[0]));
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added this function
    if (compare (ev[1], "math_mod"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    returnint = (nlist[0] % nlist[1]);
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added for global functions
    if (compare (ev[1], "make_global_function"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    make_function (slist[0], slist[1]);

	    //Msg(slist[0]);
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1
    if (compare (ev[1], "sp_blood_seq"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    change_sprite (nlist[0], nlist[1], &spr[nlist[0]].bloodseq);
	    returnint = spr[nlist[0]].bloodseq;
	    return (0);
	  }
	returnint = -1;
	return (0);
      }

    //redink1
    if (compare (ev[1], "sp_blood_num"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    change_sprite (nlist[0], nlist[1], &spr[nlist[0]].bloodnum);
	    returnint = spr[nlist[0]].bloodseq;
	    return (0);
	  }
	returnint = -1;
	return (0);
      }

    //redink1 added this function to change the save game 'info'
    if (compare (ev[1], "set_save_game_info"))

      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))

	  {
	    strcpy (save_game_info, slist[0]);
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 - clears the editor information, useful for save games and such
    if (compare (ev[1], "clear_editor_info"))
      {
	h = &h[strlen (ev[1])];
	for (int i = 0; i < 769; i++)
	  {
	    for (int j = 0; j < 100; j++)
	      {
		play.spmap[i].seq[j] = 0;
		play.spmap[i].frame[j] = 0;
		play.spmap[i].type[j] = 0;
		play.spmap[i].last_time = 0;
	      }
	  }
	returnint = 1;
	return(0);
      }

    //redink1 added this function to load a new map/dink.dat
    if (compare (ev[1], "load_map"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = { 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms(ev[1], script, h, p))
	  {
	    strcpy(current_map, slist[0]);
	    strcpy(current_dat, slist[1]);
	    load_info();
	  }
	strcpy(s, h);
	return(0);
      }

    //redink1 added this function to load new tiles, because he is a l33t guy
    if (compare (ev[1], "load_tile"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 2, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    if (nlist[1] >= 1 && nlist[1] <= NB_TILE_SCREENS)
	      {
		//Load in the new tiles...
		tiles_load_slot(slist[0], nlist[1]);

		//Store in save game
		strncpy(play.tile[nlist[1]].file, slist[0], 50);
	      }
	  }
	strcpy (s, h);
	return (0);
      }

    //redink1 added so developers can change or see what tile is at any given position
    if (compare (ev[1], "map_tile"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    //Yeah... they can only modify valid tiles
	    if (nlist[0] > 0 && nlist[0] <= 96)
	      {
		//Only change the value if it is greater than 0...
		if (nlist[1] > 0)
		  {
		    pam.t[nlist[0] - 1].num = nlist[1];
		  }
		returnint = pam.t[nlist[0] - 1].num;
		return (0);
	      }
	  }
	returnint = -1;
	return (0);
      }

    //redink1 added so a developer can retrieve/modify a hard tile
    if (compare (ev[1], "map_hard_tile"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    //Yeah... they can only modify valid tiles
	    if (nlist[0] > 0 && nlist[0] <= 96)
	      {
		//Only change the value if it is greater than 0...
		if (nlist[1] > 0)
		  {
		    pam.t[nlist[0] - 1].althard = nlist[1];
		  }
		returnint = pam.t[nlist[0] - 1].althard;
		return (0);
	      }
	  }
	returnint = -1;
	return (0);
      }

    //redink1 added this function to load a pallete from any bmp
    if (compare (ev[1], "load_palette"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    char *name = slist[0];
	    SDL_Surface* image = NULL;
	    FILE *in = paths_dmodfile_fopen(name, "rb");
	    if (in == NULL)
	      fprintf(stderr, "Error: Can't open palette '%s'.", name);
	    else
	      /* Set palette */
	      image = load_bmp_setpal(in);
	    
	    if (image == NULL)
	      fprintf(stderr, "Couldn't load palette from '%s'.\n", name);
	    else
	      {
		//Store in save game
		strncpy(play.palette, slist[0], 50);
		SDL_FreeSurface(image);
	      }
	  }
	strcpy(s, h);
	return(0);
      }

    //redink1 added to get index of specified item
    if (compare (ev[1], "get_item"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms(ev[1], script, h, p))
	  {
	    returnint = 0;
	    for (int i = 1; i < 17; i++)
	      {
		if (play.item[i].active)
		  {
		    if (compare(play.item[i].name, slist[0]))
		      {
			returnint = i;
			break;
		      }
		  }
	      }
	    return(0);
	  }
	returnint = -1;
	return(0);
      }

    //redink1 added to get index of specified magic spell
    if (compare (ev[1], "get_magic"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms(ev[1], script, h, p))
	  {
	    returnint = 0;
	    for (int i = 1; i < 9; i++)
	      {
		if (play.mitem[i].active)
		  {
		    if (compare(play.mitem[i].name, slist[0]))
		      {
			returnint = i;
			break;
		      }
		  }
	      }
	    return(0);
	  }
	returnint = -1;
	return(0);
      }

    //redink1 - sets font color
    if (compare (ev[1], "set_font_color"))
      {
	h = &h[strlen(ev[1])];
	int p[20] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 };
	if (get_parms(ev[1], script, h, p))
	  {
	    set_font_color(nlist[0],
			   nlist[1], nlist[2], nlist[3]);
	  }
	strcpy(s, h);
	return(0);
      }

    //redink1 added this to make Paul Pliska's life more fulfilling
    if (compare (ev[1], "get_next_sprite_with_this_brain"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    for (int i = nlist[2]; i <= last_sprite_created; i++)
	      {
		if ((spr[i].brain == nlist[0]) && (i != nlist[1]))
		  if (spr[i].active == 1)
		    {
		      Msg ("Ok, sprite with brain %d is %d", nlist[0], i);
		      returnint = i;
		      return (0);
		    }
	      }
	  }
	Msg ("Ok, sprite with brain %d is 0", nlist[0], i);
	returnint = 0;
	return (0);
      }

    //redink1 clip stuff
    if (compare (ev[1], "sp_clip_left"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    change_sprite (nlist[0], nlist[1], &spr[nlist[0]].alt.left);
	    returnint = spr[nlist[0]].alt.left;
	    return (0);
	  }
	returnint = -1;
	return (0);
      }

    //redink1 clip stuff
    if (compare (ev[1], "sp_clip_top"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    change_sprite (nlist[0], nlist[1], &spr[nlist[0]].alt.top);
	    returnint = spr[nlist[0]].alt.top;
	    return (0);
	  }
	returnint = -1;
	return (0);
      }

    //redink1 clip stuff
    if (compare (ev[1], "sp_clip_right"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    change_sprite (nlist[0], nlist[1], &spr[nlist[0]].alt.right);
	    returnint = spr[nlist[0]].alt.right;
	    return (0);
	  }
	returnint = -1;
	return (0);
      }

    //redink1 clip stuff
    if (compare (ev[1], "sp_clip_bottom"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    change_sprite (nlist[0], nlist[1], &spr[nlist[0]].alt.bottom);
	    returnint = spr[nlist[0]].alt.bottom;
	    return (0);
	  }
	returnint = -1;
	return (0);
      }

    if (compare (ev[1], "set_smooth_follow"))
      {
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (get_parms (ev[1], script, h, p))
	  {
	    if (nlist[0] == 0)
	      {
		smooth_follow = 0;
	      }
	    else if (nlist[0] == 1)
	      {
		smooth_follow = 1;
	      }
	    return (0);
	  }
	returnint = -1;
	return (0);
      }
    
    //redink1 added so users can check truecolor or not
    if (compare (ev[1], "get_truecolor"))
      {
	h = &h[strlen (ev[1])];
	returnint = truecolor;
	strcpy (s, h);
	return (0);
      }

    }


  /***************************************/
  /** New DinkC user-defined procedures **/
  /**                                   **/
  /***************************************/
  if (dversion >= 108)
    {

    /* added so we can have return values and crap. */
    /* see also "return;" above */
    if (compare (ev[1], "return"))

      {
	if (debug_mode)
	  Msg ("Found return; statement");
	h = &h[strlen (ev[1])];
	strip_beginning_spaces (h);
	process_line (script, h, 0);
	if (rinfo[script]->proc_return != 0)

	  {
	    bKeepReturnInt = 1;
	    run_script (rinfo[script]->proc_return);
	    kill_script (script);
	  }
	return (2);
      }

    if (compare (ev[1], "external"))
      {
	h = &h[strlen (ev[1])];
	int
	p[20] = { 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 };
	memset (slist, 0, 10 * 200);
	get_parms (ev[1], script, h, p);
	if (strlen (slist[0]) > 0 && strlen (slist[1]) > 0)
	  {
	    int
	      myscript1 =
	      load_script (slist[0], rinfo[script]->sprite, 0);
	    if (myscript1 == 0)
	      {
		Msg ("Error:  Couldn't find %s.c (for procedure %s)",
		     slist[0], slist[1]);
		return (0);
	      }
	    rinfo[myscript1]->arg1 = nlist[2];
	    rinfo[myscript1]->arg2 = nlist[3];
	    rinfo[myscript1]->arg3 = nlist[4];
	    rinfo[myscript1]->arg4 = nlist[5];
	    rinfo[myscript1]->arg5 = nlist[6];
	    rinfo[myscript1]->arg6 = nlist[7];
	    rinfo[myscript1]->arg7 = nlist[8];
	    rinfo[myscript1]->arg8 = nlist[9];
	    if (locate (myscript1, slist[1]))
	      {
		rinfo[myscript1]->proc_return = script;
		run_script (myscript1);
		return (2);
	      }
	    else
	      {
		Msg ("Error:  Couldn't find procedure %s in %s.", slist[1],
		     slist[0]);
		kill_script (myscript1);
	      }
	  }
	strcpy (s, h);
	return (0);
      }
    if (strchr (h, '(') != NULL)
      {
	//lets attempt to run a procedure
	int myscript = load_script (rinfo[script]->name, rinfo[script]->sprite, 0);
	h = &h[strlen (ev[1])];
	int p[20] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	get_parms (ev[1], script, h, p);
	if (locate(myscript, ev[1]))
	  {
	    /* Custom procedure in the current script */
	    rinfo[myscript]->arg1 = nlist[0];
	    rinfo[myscript]->arg2 = nlist[1];
	    rinfo[myscript]->arg3 = nlist[2];
	    rinfo[myscript]->arg4 = nlist[3];
	    rinfo[myscript]->arg5 = nlist[4];
	    rinfo[myscript]->arg6 = nlist[5];
	    rinfo[myscript]->arg7 = nlist[6];
	    rinfo[myscript]->arg8 = nlist[7];
	    rinfo[myscript]->arg9 = nlist[8];
	    rinfo[myscript]->proc_return = script;
	    run_script(myscript);
	    return(2);
	  }
	else
	  {
	    /* Try custom global procedure */
	    for (int i = 0; i < 100; i++)
	      {
		/* Skip empty slots */
		if (strlen (play.func[i].func) == 0)
		  continue;

		if (compare(play.func[i].func, ev[1]))
		  {
		    myscript = load_script(play.func[i].file, rinfo[script]->sprite, 0);
		    rinfo[myscript]->arg1 = nlist[0];
		    rinfo[myscript]->arg2 = nlist[1];
		    rinfo[myscript]->arg3 = nlist[2];
		    rinfo[myscript]->arg4 = nlist[3];
		    rinfo[myscript]->arg5 = nlist[4];
		    rinfo[myscript]->arg6 = nlist[5];
		    rinfo[myscript]->arg7 = nlist[6];
		    rinfo[myscript]->arg8 = nlist[7];
		    rinfo[myscript]->arg9 = nlist[8];
		    if (locate(myscript, ev[1]))
		      {
			rinfo[myscript]->proc_return = script;
			run_script (myscript);
			return(2);
		      }
		    break;
		  }
	      }
	    Msg
	      ("ERROR:  Procedure void %s( void ); not found in script %s. (word 2 was %s) ",
	       ev[1], ev[2], rinfo[myscript]->name);
	    kill_script (myscript);
	  }

	/*seperate_string(h, 1,'(',line);

	   int myscript = load_script(rinfo[script]->name, rinfo[script]->sprite, false);

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
	   } */
	return (0);
      }
    }
  else
    {
      /* v1.07 function that are implemented differently than in v1.08 */

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
                                                                        strcpy_nooverlap(s, h);
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
        }

bad:
	strcpy_nooverlap(s, h);
        return 0;

good:
	strcpy_nooverlap(s, h);
        //s = h
        //Msg("ok, continuing with running %s..",s);
        return 1;
}
