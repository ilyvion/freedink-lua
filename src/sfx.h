/**
 * Sound (not music) management

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2003  Shawn Betts
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

#ifndef _SFX_H
#define _SFX_H

#include "SDL_mixer.h"

extern int InitSound();
extern int CreateBufferFromWaveFile(char* filename, int dwBuf);
extern int CreateBufferFromWaveFile_RW(SDL_RWops* rwops, int rwfreesrc, int index);
extern void EditorSoundPlayEffect(int sound);
extern int SoundPlayEffect(int sound, int min, int plus, int sound3d, /*bool*/int repeat);
extern int playing(int sound);
extern int SoundStopEffect(int sound);
extern void kill_repeat_sounds(void);
extern void kill_repeat_sounds_all(void);
extern void QuitSound(void);
extern void sfx_log_meminfo(void);
extern void sfx_cleanup_finished_channels(void);
extern void update_sound(void);

/* DinkC procedures */
extern int playsound(int sound, int min, int plus, int sound3d, int repeat);
extern void sound_set_kill(int soundbank);
extern void sound_set_survive(int soundbank, int survive);
extern void sound_set_vol(int soundbank, int volume);

#endif
