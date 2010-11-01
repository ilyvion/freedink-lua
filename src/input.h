/**
 * Keyboard and joystick

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

#ifndef _INPUT_H
#define _INPUT_H

#ifdef __cplusplus
extern "C"
{
#endif

#define NB_BUTTONS 12

  /* Actions that can be dynamically mapped on joystick buttons (and
     statically mapped to keyboard keys). The indices are important:
     they are used in the savegame format and in DinkC's
     wait_for_button() - note that indices 12/14/16/18 are reserved
     for down/left/right/up in that function. */
  enum buttons_actions {
    ACTION_FIRST  = 0, // min constant for loops, like SDLKey
    ACTION_NOOP = 0,
    ACTION_ATTACK = 1,
    ACTION_TALK,
    ACTION_MAGIC,
    ACTION_INVENTORY,
    ACTION_MENU,
    ACTION_MAP,
    // These execute the 'buttonX.c' DinkC script:
    ACTION_BUTTON7,
    ACTION_BUTTON8,
    ACTION_BUTTON9,
    ACTION_BUTTON10,
    // To map buttons to arrow keys
    // Using the same keys than in wait_for_button()
    ACTION_DOWN  = 12,
    ACTION_LEFT  = 14,
    ACTION_RIGHT = 16,
    ACTION_UP    = 18,
    // max+1 constant for loops
    ACTION_LAST
  };

  struct seth_joy
  {
    int joybit[ACTION_LAST]; // is this action currently pressed?
    int button[ACTION_LAST]; // was this action just pressed (not maintained pressed)?
    int joybitold[ACTION_LAST];  // copy of previous joybit state
                                 // (used to compute .button and to release an arrow with the bow)
    // Note: also used by Dinkedit, but with fewer different actions
    
    /* Only used in the editor (for now): */
    /* State of the keyboard, SDL-supported keys */
#if SDL_VERSION_ATLEAST(1, 3, 0)
    /* SDL 1.3 TODO */
#else
    int keystate[SDLK_LAST]; /* current GetAsyncKeyState value, in
    				cache */
    int keyjustpressed[SDLK_LAST]; /* true if key was just pressed, false
    				      if kept pressed or released */
#endif

    /* Idem, but with unicode characters - layout-independant */
    char charstate[65536];
    char charjustpressed[65536];
    char key2char[65536]; /* to retrieve matching unicode on SDL_KEYUP,
			     if possible */
    Uint16 last_unicode; /* last character typed by the user, used for
			    text input */
    Uint16 last_nokey_unicode; /* char with no key match, so no KEYUP
				  support - reset it next time */
    
    int right,left,up,down; // is this arrow currently pressed?
    int rightd,leftd,upd,downd; // was this arrow just pressed (not maintained pressed)?
    int rightold,leftold,upold,downold; // copy of previous state (used to compute *d)
  };
  extern struct seth_joy sjoy;

  extern int GetKeyboard(int key);
  extern void input_init(void);
  extern void input_quit(void);
  extern void input_set_default_buttons(void);
  extern enum buttons_actions input_get_button_action(int button_index);
  extern void input_set_button_action(int button_index, enum buttons_actions action_index);

#ifdef __cplusplus
}
#endif

#endif
