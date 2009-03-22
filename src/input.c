/**
 * Keyboard and joystick

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009  Sylvain Beucler

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

#include <string.h>

#include "SDL.h"
#include "game_engine.h"
#include "log.h"
#include "input.h"

/* maps joystick buttons to action IDs (attack/attack/map/...). */
/* 10 buttons (indices), 6 different actions + 4 static buttons (values) */
static enum buttons_actions buttons_map[NB_BUTTONS];


/* TODO: maybe it's not necessary to SDL_PumpEvents every time, since
   it's usually done before this function is called */
int GetKeyboard(int key)
{
  // returns 0 if the key has been depressed, else returns 1 and sets key to code recd.
  int keystate_size;
  Uint8 *keystate;
  //SDL_PumpEvents();
  keystate = SDL_GetKeyState(&keystate_size);
  return keystate[key];
}

void input_init(void)
{
  /* Enable Unicode to be able to grab what letter the user actually
     typed, taking the keyboard layout/language into account. Used for
     the console (game) and the input dialogs (editor). */
  SDL_EnableUNICODE(1);

  /* Clear keyboard/joystick buffer */
  memset(&sjoy,0,sizeof(sjoy));
  {
    int k = 0;
    for (k = 0; k < 256; k++)
      GetKeyboard(k);
    
    int a = ACTION_FIRST;
    for (a = ACTION_FIRST; a < ACTION_LAST; a++) 
      sjoy.letgo[a] = /*TRUE*/1;
  }

  /* Define default button->action mapping */
  input_set_default_buttons();

  /* JOY */
  /* Joystick initialization never makes Dink fail for now. */
  /* Note: joystick is originaly only used by the game, not the
     editor. */
  if (joystick == 1)
    {
      if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == -1)
	{
	  Msg("Error initializing joystick, skipping: %s\n", SDL_GetError());
	  joystick = 0;
	}
      else
	{
	  /* first tests if a joystick driver is present */
	  /* if TRUE it makes certain that a joystick is plugged in */
	  if (SDL_NumJoysticks() > 0)
	    {
	      int i;
	      printf("%i joysticks were found.\n", SDL_NumJoysticks());
	      printf("The names of the joysticks are:\n");
	      for (i=0; i < SDL_NumJoysticks(); i++)
		printf("    %s\n", SDL_JoystickName(i));
	      printf("Picking the first one...\n");
	      jinfo = SDL_JoystickOpen(0);
	      /* Don't activate joystick events, Dink polls joystick
		 manually.  Plus events would pile up in the queue. */
	      SDL_JoystickEventState(SDL_IGNORE);
	      
	      if (jinfo) {
		printf("Name: %s\n", SDL_JoystickName(0));
		printf("Number of axes: %d\n", SDL_JoystickNumAxes(jinfo));
		printf("Number of buttons: %d\n", SDL_JoystickNumButtons(jinfo));
		printf("Number of balls: %d\n", SDL_JoystickNumBalls(jinfo));
		printf("Number of hats: %d\n", SDL_JoystickNumHats(jinfo));
		
		/* Flush stacked joystick events */
		{
		  SDL_Event event;
		  while (SDL_PollEvent(&event));
		}
		
		joystick = 1;
	      } else {
		printf("Couldn't open Joystick 0\n");
		joystick = 0;
	      }
	    }
	}
    }
}

void input_quit(void)
{
  if (joystick == 1)
    {
      if (jinfo != NULL)
	SDL_JoystickClose(jinfo);
      SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
}

void input_set_default_buttons(void)
{
  /* Set default button->action mapping */
  int i = 0;
  for (i = 0; i < NB_BUTTONS; i++)
    input_set_button_action(i, ACTION_NOOP);

  input_set_button_action( 1-1, ACTION_ATTACK);
  input_set_button_action( 2-1, ACTION_TALK);
  input_set_button_action( 3-1, ACTION_MAGIC);
  input_set_button_action( 4-1, ACTION_INVENTORY);
  input_set_button_action( 5-1, ACTION_MENU);
  input_set_button_action( 6-1, ACTION_MAP);
  input_set_button_action( 7-1, ACTION_BUTTON7);
  input_set_button_action( 8-1, ACTION_BUTTON8);
  input_set_button_action( 9-1, ACTION_BUTTON9);
  input_set_button_action(10-1, ACTION_BUTTON10);

#ifdef _PSP
  /* Alternate mapping, more consistent with other apps on PSP; in
     addition, the buttons numbering/placement is different than on
     PC, so it needs to be redefined anyway. */
  /* Here are names for the button indices returned by SDL, 12 buttons
     in [0,11], plus (apparently non-usable) HOME and HOLD [can 'note'
     be mapped too?] */
  enum buttons_psp {
    BUTTON_TRIANGLE=0, BUTTON_CIRCLE, BUTTON_CROSS, BUTTON_SQUARE,
    BUTTON_LTRIGGER, BUTTON_RTRIGGER,
    BUTTON_DOWN, BUTTON_LEFT, BUTTON_UP, BUTTON_RIGHT,
    BUTTON_SELECT, BUTTON_START, BUTTON_HOME, BUTTON_HOLD };
  input_set_button_action(BUTTON_TRIANGLE,  ACTION_INVENTORY);
  input_set_button_action(BUTTON_CIRCLE,    ACTION_MAGIC);
  input_set_button_action(BUTTON_CROSS,     ACTION_ATTACK);
  input_set_button_action(BUTTON_SQUARE,    ACTION_TALK);
  input_set_button_action(BUTTON_LTRIGGER,  ACTION_MENU);
  input_set_button_action(BUTTON_RTRIGGER,  ACTION_MAP);
  // TODO: make these also work like d/l/u/r:
  input_set_button_action(BUTTON_DOWN,      ACTION_NOOP);
  input_set_button_action(BUTTON_LEFT,      ACTION_NOOP);
  input_set_button_action(BUTTON_UP,        ACTION_NOOP);
  input_set_button_action(BUTTON_RIGHT,     ACTION_NOOP);
  /* TODO: we could also map:
     - debug (Alt+D),
     - pause/resume midi (Alt+N/B)
     - fast-quit (Alt+Q) - fast-quit is somewhat already available
     through the classic Home key, although handled differently. */
  /* Let's also try to get a free key to possibly implement a
     zooming/switch-view function for small screens, as well as a
     virtual keyboard feature (like ScummVM)... */
  /* Maybe also map inventory to start instead of Triangle. */
  input_set_button_action(BUTTON_START,     ACTION_INVENTORY);
#endif
}

enum buttons_actions input_get_button_action(int button_index)
{
  if (button_index >= 0 && button_index < NB_BUTTONS)
    {
      return buttons_map[button_index];
    }
  return -1; /* error */
}

/**
 * Set what action will be triggered when button 'button_index' is
 * pressed. Action '0' currently means 'do nothing'.
 */
void input_set_button_action(int button_index, enum buttons_actions action_index)
{
  if (button_index >= 0 && button_index < NB_BUTTONS)
    {
      if (action_index >= ACTION_FIRST && action_index < ACTION_LAST)
	buttons_map[button_index] = action_index;
      else
	fprintf(stderr, "Attempted to set invalid action %d\n", action_index);
    }
  else
    {
      fprintf(stderr, "Attempted to set invalid button %d (internal index %d)\n",
	      button_index+1, button_index);
    }
}
