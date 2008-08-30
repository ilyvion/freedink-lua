/**
 * Graphics - sprites management

 * Copyright (C) 2007  Sylvain Beucler

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

#ifndef _GFX_SPRITES_H
#define _GFX_SPRITES_H

#include "SDL.h"
#include "rect.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Max number of sprites, minus 1 (GFX_k is indexed from 1) */
#define MAX_SPRITES 4000
#define MAX_FRAMES_PER_SEQUENCE 50
/* Max number of sequences the engine could be abused to load */
#define MAX_FRAMES_PER_ABUSED_SEQUENCE 1000
  
  /* Store sprites info */
  struct pic_info
  {
    /*   LPDIRECTDRAWSURFACE k; // Sprites */
    
    rect box;     // Dimensions (0,0,width,height)
    rect hardbox; // Square where Dink can't block if sprite is hard
    
    int yoffset;  // Center of the picture
    int xoffset;
  };
  
  struct GFX_pic_info
  {
    SDL_Surface *k; // Sprites
    /* TODO: move pic_info to GFX_pic_info; if possible, replace 'box'
       with k->h and k->w in the code */
  };
  
  /* Sequence description */
  struct sequence
  {
    int is_active;  // does it contain something
    char* ini;      // matching dink.ini (or init()) line
    int len;        // number of initial frames in this sequence
                    // - inaccurate if the sequence is modified by 'set_frame_frame'
    /* frame: index in GFX_k for the each frame, indexed from 1, ended
       by '0' if -1, loop from beginning. Now this one is tricky: the
       original engine's load_sprite() can load non-animated sequences
       of more than MAX_FRAMES_PER_SEQUENCE (up to 1000) at the
       expense of a non-critical buffer overflow in 'delay' and
       'special'.  */
    int frame[MAX_FRAMES_PER_ABUSED_SEQUENCE+1+1]; 
    int delay[MAX_FRAMES_PER_ABUSED_SEQUENCE+1]; // frame duration, indexed from 1
    unsigned char special[MAX_FRAMES_PER_SEQUENCE+1]; // does this frame 'hit' enemies, indexed from 1
  };

  extern struct pic_info k[];
  extern struct GFX_pic_info GFX_k[];
  extern struct sequence seq[];

  extern void sprites_unload(void);
  extern void load_sprite_pak(char seq_path_prefix[100], int seq_no, int speed, int xoffset, int yoffset,
			      rect hardbox, int flags, int samedir);
  extern void load_sprites(char seq_path_prefix[100], int seq_no, int speed, int xoffset, int yoffset,
			   rect hardbox, int flags);
  extern void seq_set_ini(int seq_no, char *line);

#ifdef __cplusplus
}
#endif

#endif
