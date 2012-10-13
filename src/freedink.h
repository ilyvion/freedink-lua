/**
 * Header for FreeDink-specific code (not FreeDinkedit)

 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2012  Sylvain Beucler

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

/**
 * I made this header because I though update_frame had to be compiled
 * separately - actually it was also included in dink.cpp. Might still
 * come in handy.
 */

#ifndef _FREEDINK_H
#define _FREEDINK_H

#include "SDL.h"

extern void CyclePalette();
extern void Scrawl_OnMouseInput(void);
extern void button_brain(int h );
extern void bounce_brain(int h);
extern int check_if_move_is_legal(int u);
extern void check_joystick(void);
extern void did_player_cross_screen(/*bool*/int real, int h);
extern void dragon_brain(int h);
extern void drawscreenlock( void );
extern void flip_it(void);
extern void get_last_sprite(void);
extern void human_brain(int h);
extern /*bool*/int in_this_base(int seq, int base);
extern int initFail(char mess[200]);
extern void missile_brain( int h, /*bool*/int repeat);
extern void missile_brain_expire(int h);
extern void mouse_brain(int h);
extern void process_item( void );
extern void process_show_bmp( void );
extern void process_talk();
extern void process_warp_man(void);
extern void no_brain(int h);
extern void one_time_brain(int h);
extern void one_time_brain_for_real(int h);
extern void people_brain(int h);
extern void pig_brain(int h);
extern void pill_brain(int h);
extern void repeat_brain(int h);
extern void run_through_tag_list(int h, int strength);
extern /*bool*/int run_through_tag_list_talk(int h);
extern void run_through_touch_damage_list(int h);
extern void scale_brain(int h);
extern void shadow_brain(int h);
extern int special_block(int block);
extern void text_brain(int h);
/* extern void text_draw(int h, HDC hdc); */
extern void text_draw(int h);
extern /*bool*/int transition(void);
extern void up_cycle(void);

#endif
