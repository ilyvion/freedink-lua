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
static int buttons_map[10];


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
    int x, x1;
    for (x = 0; x < 256; x++)
      GetKeyboard(x);
	
    for (x1 = 1; x1 <= 10; x1++) 
      sjoy.letgo[x1] = /*TRUE*/1;
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
  int i = 1;
  for (; i <= NB_BUTTONS; i++)
    input_set_button_action(i, i);
#ifdef _PSP
  /* Alternate mapping, more consistent with other apps on PSP */
  /* static const enum PspCtrlButtons button_map[] = { */
  /* 	PSP_CTRL_TRIANGLE, PSP_CTRL_CIRCLE, PSP_CTRL_CROSS, PSP_CTRL_SQUARE, */
  /* 	PSP_CTRL_LTRIGGER, PSP_CTRL_RTRIGGER, */
  /* 	PSP_CTRL_DOWN, PSP_CTRL_LEFT, PSP_CTRL_UP, PSP_CTRL_RIGHT, */
  /* 	PSP_CTRL_SELECT, PSP_CTRL_START, PSP_CTRL_HOME, PSP_CTRL_HOLD };   */
  input_set_button_action(1,  ACTION_INVENTORY); // triangle
  input_set_button_action(2,  ACTION_MAGIC);     // circle
  input_set_button_action(3,  ACTION_ATTACK);    // cross
  input_set_button_action(4,  ACTION_TALK);      // square
  input_set_button_action(5,  ACTION_MENU);      // ltrigger
  input_set_button_action(6,  ACTION_MAP);       // rtrigger
  // TODO: make these also work like d/l/u/r:
  input_set_button_action(7,  ACTION_NOOP);      // down
  input_set_button_action(8,  ACTION_NOOP);      // left
  input_set_button_action(9,  ACTION_NOOP);      // up
  input_set_button_action(10, ACTION_NOOP);      // right
#endif
}

/* BIG FAT WARNING: in DinkC, buttons are in [1, 10] (not [0, 9]) */
/* Current return range: [1-10] inclusive */
int input_get_button_action(int button_index)
{
  button_index--;
  if (button_index >= 0 && button_index < NB_BUTTONS)
    {
      int action = buttons_map[button_index];
      return action;
    }
  return -1; /* error */
}

/**
 * Set what action will be triggered when button 'button_index' is
 * pressed. Action '0' currently means 'do nothing'.
 */
void input_set_button_action(int button_index, int action_index)
{
  button_index--;
  if (button_index >= 0 && button_index < NB_BUTTONS)
    {
      if (action_index >= ACTION_FIRST && action_index < ACTION_LAST)
	buttons_map[button_index] = action_index;
      else
	fprintf(stderr, "Attempted to set invalid action %d\n", action_index);
    }
  else
    {
      fprintf(stderr, "Attempted to set invalid button %d\n", button_index+1);
    }
}
