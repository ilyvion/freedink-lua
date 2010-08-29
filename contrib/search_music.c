/*
 * Search for a DinkC script usage in the world map

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2010  Sylvain Beucler

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

#define log_info printf
#define log_error printf

#define BOOL_1BYTE char /* Boolean value on 1 byte exactly, used to
			   replace the C++ bool type during the C++=>C
			   conversion. Do not change to int, else
			   player_info (among others) will have a
			   different size and savegame format will
			   change! */

typedef struct rect {
  int left, top, right, bottom;
} rect;

/* Background square in a screen */
struct tile
{
  short square_full_idx0; /* tile index */
  short althard; /* alternate hardness index, 0 = default tile hardness */
};

struct sprite_placement
{
  int x, y, seq, frame, type, size;
  BOOL_1BYTE active;
  int rotation, special, brain;
  
  char script[13+1]; /* attached DinkC script */
  int speed, base_walk, base_idle, base_attack, base_hit, timer, que;
  int hard;
  rect alt; /* trim left/top/right/bottom */
  int prop;
  int warp_map;
  int warp_x;
  int warp_y;
  int parm_seq;
  
  int base_die, gold, hitpoints, strength, defense, exp, sound, vision, nohit, touch_damage;
  int buff[5];
};

/* one screen from map.dat */
struct small_map
{
  struct tile t[12*8+1]; // 97 background tiles
  struct sprite_placement sprite[100+1];
  char script[20+1]; /* script to run when entering the script */
};

/* dink.dat */
struct map_info
{
  int loc[769];
  int music[769];
  int indoor[769];
};
extern struct map_info map;


/**
 * Read integer portably (same result with MSB and LSB
 * endianness). Source data is a file with little-endian data.
 */
int read_lsb_int(FILE *f)
{
  unsigned char buf[4];
  fread(buf, 4, 1, f);
  return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
}


/**
 * Write integer portably (same result with MSB and LSB
 * endianness). Will write little-endian data to file.
 */
void write_lsb_int(int n, FILE *f)
{
  unsigned char buf[4];
  buf[0] = n & 0xFF;
  buf[1] = (n >> (1*8)) & 0xFF;
  buf[2] = (n >> (2*8)) & 0xFF;
  buf[3] = (n >> (3*8)) & 0xFF;
  fwrite(buf, 4, 1, f);
}

/**
 * Read short portably (same result with MSB and LSB
 * endianness). Source data is a file with little-endian data.
 */
short read_lsb_short(FILE *f)
{
  unsigned char buf[2];
  fread(buf, 2, 1, f);
  return (buf[1] << 8) | (buf[0]);
}


/**
 * Write short portably (same result with MSB and LSB
 * endianness). Will write little-endian data to file.
 */
void write_lsb_short(short n, FILE *f)
{
  unsigned char buf[2];
  buf[0] = n & 0xFF;
  buf[1] = (n >> (1*8)) & 0xFF;
  fwrite(buf, 2, 1, f);
}



/**
 * Load dink.dat to specified memory buffer
 */
int load_info_to(char* path, struct map_info *mymap)
{
  FILE *f = NULL;

  f = fopen(path, "rb");
  if (!f)
    return -1;

  //log_info("World data loaded.");

  /* Portably load struct map_info from disk */
  int i = 0;
  fseek(f, 20, SEEK_CUR); // unused 'name' field
  for (i = 0; i < 769; i++)
    mymap->loc[i]    = read_lsb_int(f);
  for (i = 0; i < 769; i++)
    mymap->music[i]  = read_lsb_int(f);
  for (i = 0; i < 769; i++)
    mymap->indoor[i] = read_lsb_int(f);
  fseek(f, 2240, SEEK_CUR); // unused space

  fclose(f);

  return 0;
}

/**
 * Load 1 screen from specified map.dat in specified memory buffer
 */
int load_map_to(char* path, const int num, struct small_map* screen)
{
  /* Instead of using 'fseek(...)' when we want to skip a little bit
     of data, we read it to this buffer - this is much faster on PSP
     (1000ms -> 60ms), probably related to cache validation. No
     noticeable change on PC (<1ms). */
  char skipbuf[10000]; // more than any fseek we do

  FILE *f = NULL;
  long holdme,lsize;
  f = fopen(path, "rb");
  if (!f)
    {
      log_error("Cannot find %s file!!!", path);
      return -1;
    }
  lsize = 31280; // sizeof(struct small_map); // under ia32, not portable
  holdme = (lsize * (num-1));
  fseek(f, holdme, SEEK_SET);
  //Msg("Trying to read %d bytes with offset of %d",lsize,holdme);

  /* Portably load map structure from disk */
  int i = 0;
  fread(skipbuf, 20, 1, f); // unused 'name' field
  for (i = 0; i < 97; i++)
    {
      screen->t[i].square_full_idx0 = read_lsb_int(f);
      fread(skipbuf, 4, 1, f); // unused 'property' field
      screen->t[i].althard = read_lsb_int(f);
      fread(skipbuf, 6, 1, f); // unused 'more2', 'more3', 'more4' fields
      fread(skipbuf, 2, 1, f); // reproduce memory alignment
      fread(skipbuf, 60, 1, f); // unused 'buff' field
    }
  // offset 7780
  
  fread(skipbuf, 160, 1, f); // unused 'v' field
  fread(skipbuf, 80, 1, f);  // unused 's' field
  // offset 8020
  
  /* struct sprite_placement sprite[101]; */
  /* size = 220 */
  for (i = 0; i < 101; i++)
    {
      screen->sprite[i].x = read_lsb_int(f);
      screen->sprite[i].y = read_lsb_int(f);
      screen->sprite[i].seq = read_lsb_int(f);
      screen->sprite[i].frame = read_lsb_int(f);
      screen->sprite[i].type = read_lsb_int(f);
      screen->sprite[i].size = read_lsb_int(f);
      
      screen->sprite[i].active = fgetc(f);
      fread(skipbuf, 3, 1, f); // reproduce memory alignment
      // offset 28
      
      screen->sprite[i].rotation = read_lsb_int(f);
      screen->sprite[i].special = read_lsb_int(f);
      screen->sprite[i].brain = read_lsb_int(f);
      
      fread(screen->sprite[i].script, 14, 1, f);
      screen->sprite[i].script[14-1] = '\0'; // safety
      fread(skipbuf, 38, 1, f); // unused hit/die/talk fields
      // offset 92
      
      screen->sprite[i].speed = read_lsb_int(f);
      screen->sprite[i].base_walk = read_lsb_int(f);
      screen->sprite[i].base_idle = read_lsb_int(f);
      screen->sprite[i].base_attack = read_lsb_int(f);
      screen->sprite[i].base_hit = read_lsb_int(f);
      screen->sprite[i].timer = read_lsb_int(f);
      screen->sprite[i].que = read_lsb_int(f);
      screen->sprite[i].hard = read_lsb_int(f);
      // offset 124
      
      screen->sprite[i].alt.left = read_lsb_int(f);
      screen->sprite[i].alt.top = read_lsb_int(f);
      screen->sprite[i].alt.right = read_lsb_int(f);
      screen->sprite[i].alt.bottom = read_lsb_int(f);
      // offset 140
      
      screen->sprite[i].prop = read_lsb_int(f);
      screen->sprite[i].warp_map = read_lsb_int(f);
      screen->sprite[i].warp_x = read_lsb_int(f);
      screen->sprite[i].warp_y = read_lsb_int(f);
      screen->sprite[i].parm_seq = read_lsb_int(f);
      // offset 160
      
      screen->sprite[i].base_die = read_lsb_int(f);
      screen->sprite[i].gold = read_lsb_int(f);
      screen->sprite[i].hitpoints = read_lsb_int(f);
      screen->sprite[i].strength = read_lsb_int(f);
      screen->sprite[i].defense = read_lsb_int(f);
      screen->sprite[i].exp = read_lsb_int(f);
      screen->sprite[i].sound = read_lsb_int(f);
      screen->sprite[i].vision = read_lsb_int(f);
      screen->sprite[i].nohit = read_lsb_int(f);
      screen->sprite[i].touch_damage = read_lsb_int(f);
      // offset 200
      
      int j = 0;
      for (j = 0; j < 5; j++)
	screen->sprite[i].buff[j] = read_lsb_int(f);
    }
  // offset 30204
  
  fread(screen->script, 21, 1, f);
  screen->script[21-1] = '\0'; // safety
  fread(skipbuf, 1018, 1, f); // unused hit/die/talk fields
  fread(skipbuf, 1, 1, f); // reproduce memory alignment
  // offset 31280
  
  fclose(f);
  return 0;
}

int main(int argc, char* argv[])
{
  struct map_info dinkdat;
  if (load_info_to("DINK.DAT", &dinkdat) < 0)
    {
      fprintf(stderr, "Cannot open DINK.DAT\n");
      exit(1);
    }
  int r = 0;
  for (r = 0; r < 769; r++)
    {
      // printf("%d: %d\n", r, dinkdat.loc[r]);
      if (dinkdat.music[r] != 0)
	{
	  printf("#%d: %d\n", r, dinkdat.music[r]);
	}
    }
}

/**
 * Local Variables:
 * compile-command: "gcc -Wall -std=c99 -pedantic -g search_music.c -o search_music"
 * End:
 */
