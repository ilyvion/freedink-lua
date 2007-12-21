/**
 * Keyboard and joystick

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007  Sylvain Beucler

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

#include "SDL.h"
#include "dinkvar.h"
#include "log.h"
#include "input.h"

int GetKeyboard(int key)
{
  // returns 0 if the key has been depressed, else returns 1 and sets key to code recd.
  int keystate_size;
  Uint8 *keystate;
  SDL_PumpEvents();
  keystate = SDL_GetKeyState(&keystate_size);
  return keystate[key];
}

int keypressed(void)
{
  int x;
  for (x=0; x<256; x++)
    if (GetKeyboard(x))
      return 1;
  return 0;
}


void input_init(void)
{
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
		 manually. */
	      /* SDL_JoystickEventState(SDL_ENABLE); */
	      
	      if (jinfo) {
		printf("Name: %s\n", SDL_JoystickName (0));
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
