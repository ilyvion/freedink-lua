/**
 * Sound (not music) management

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2003  Shawn Betts
 * Copyright (C) 2005  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with program; see the file COPYING. If not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef _SFX_H
#define _SFX_H

#include "SDL_mixer.h"

const int num_soundbanks = 20;
/* Channel metadata */
struct soundstruct
{
  bool repeat;
  int owner;
  int survive;
  int vol;
};

extern soundstruct soundinfo[num_soundbanks+1];

extern int InitSound();
extern int CreateBufferFromWaveFile(char* filename, DWORD dwBuf);
extern int EditorSoundPlayEffect(int sound);
extern int SoundPlayEffect(int sound, int min, int plus, int sound3d, bool repeat);
extern int playing(int sound);
extern int SoundStopEffect(int sound);
extern void kill_repeat_sounds(void);
extern void kill_repeat_sounds_all(void);
extern void DestroySound( void );

#endif
