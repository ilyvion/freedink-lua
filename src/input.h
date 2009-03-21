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

#define NB_BUTTONS 10

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
    ACTION_LAST // max+1 constant for loops
  };

  extern int GetKeyboard(int key);
  extern void input_init(void);
  extern void input_quit(void);
  extern void input_set_default_buttons(void);
  extern int input_get_button_action(int button_index);
  extern void input_set_button_action(int button_index, int action_index);

#ifdef __cplusplus
}
#endif

#endif
