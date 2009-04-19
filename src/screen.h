/**
 * Screen sprites and hardness

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

#ifndef _SCREEN_H
#define _SCREEN_H

#define MAX_SPRITES_EDITOR 99

// sub struct for hardness map
struct mega_y
{
	unsigned char y[401];
};

// struct for hardness map
struct hit_map
{
	struct mega_y x[601];
};
extern struct hit_map hm;

extern void screen_rank_map_sprites(int rank[]);
extern void screen_rank_game_sprites(int rank[]);
extern void place_sprites_game(void);
extern void place_sprites_game_background(void);
extern void fill_back_sprites(void);
extern void fill_hard_sprites(void);

#endif
