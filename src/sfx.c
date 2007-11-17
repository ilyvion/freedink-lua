/**
 * Sound effects (not music) management

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

#include "SDL.h"
#include "SDL_mixer.h"
#include "dinkvar.h"
#include "sfx.h"
#include "io_util.h"
#include "math.h"

struct soundstruct soundinfo[NUM_SOUNDBANKS+1];

#define MAX_SOUNDS 100
static struct
{
  Mix_Chunk *sound;
} registered_sounds[MAX_SOUNDS];

/**
 * Load sounds in the standard paths
 */
int CreateBufferFromWaveFile(char* filename, int dwBuf)
{
    // Open the wave file       
    char path[150];
    
    sprintf(path, "sound/%s", filename);
    if (!exist(ciconvert(path)))
      sprintf(path, "../dink/sound/%s", filename);
    
    registered_sounds[dwBuf].sound = Mix_LoadWAV(ciconvert(path));
    
    if (registered_sounds[dwBuf].sound == NULL)
      {
        fprintf(stderr, "Error loading %s - %s\n", filename, Mix_GetError());
        return 0;
      }
    
    return 1;
}

/**
 * Return the channel that plays the specified sound, or -1 if not
 * found
 */
int get_channel(int sound) {
  int i;
  /* Check all channels to see if it is playing the sound */
  for (i = 0; i < NUM_SOUNDBANKS; i++)
    {
      if (Mix_GetChunk(i) == registered_sounds[sound].sound)
        return i;
    }
  return -1;
}

/**
 * Is the specified sound currently playing?
 */
int playing(int sound)
{
  if (sound >= MAX_SOUNDS)
    {
      Msg("Attempting to get the status of sound %d (> MAX_SOUNDS=%d)",
          sound, MAX_SOUNDS);
      return 0;
    }

  return (get_channel(sound) != -1);
}
        

/**
 * Kill repeating sounds except the ones that survive
 */
void kill_repeat_sounds(void)
{
  int i;
  if (!sound_on)
    return;
  
  Msg("Killing repeating sound");

  for (i = 1; i <= NUM_SOUNDBANKS; i++)
    {
      // Msg("Bank #%d: repeat=%d, owner=%d, survive=%d", i,
      //   soundinfo[i].repeat, soundinfo[i].owner, soundinfo[i].survive);
      if (soundinfo[i].repeat && (soundinfo[i].owner == 0)
          && (soundinfo[i].survive == 0))
        {
          Mix_HaltChannel(i-1);
	  Msg("Killed repeating sound %d", i-1);
          soundinfo[i].repeat = 0;
        }
    }
}

/**
 * Kill all repeating sounds, even the ones that survive
 */
void kill_repeat_sounds_all(void)
{
  int i;
  if (!sound_on)
    return;
  
  for (i = 1; i <= NUM_SOUNDBANKS; i++)
    {
      if (soundinfo[i].repeat && (soundinfo[i].owner == 0))
        {
          Mix_HaltChannel(i-1);
          soundinfo[i].repeat = 0;
        }
    }
}


/**
 * Called by update_frame()
 *
 * If sound is active, refreshed pan&vol for 3D effect.
 *
 * If repeating and sprite.sound==0 and sprite.owner!=0 -> stop sound
 * If sprite.active==0 and sprite.owner!=0 -> stop sound
 */
void update_sound(void)
{
  /*
Just in case, here's setFrequency:
setFrequency
public void setFrequency(int frequency);


Sets the frequency at which the audio samples are played. 

Return Value: 

No return value. 

Parameter Description 
frequency  New frequency, in Hz, at which to play the audio samples. The value must be between 100 and 100,000. If the value is 0, the frequency is reset to the current buffer format. This format is specified in the createSoundBuffer method.  


Remarks: 

Increasing or decreasing the frequency changes the perceived pitch of the audio data. This method does not affect the format of the buffer. 

See Also: getFrequency, play, setFormat, com.ms.directX.DirectSound
  */
  int i;

  if (!sound_on)
    return;
  
  for (i = 1; i <= NUM_SOUNDBANKS; i++)
    {
      if (soundinfo[i].repeat && (soundinfo[i].owner != 0))
	{
	  if ((spr[soundinfo[i].owner].sound == 0)
	      || (spr[soundinfo[i].owner].active == /*false*/0) )
	    {
	      Mix_HaltChannel(i-1);
	      soundinfo[i].owner = 0;
	      soundinfo[i].repeat = 0;
	    }
	  else
	    {
	      SetPan(i-1, get_pan(soundinfo[i].owner));
	      SetVolume(i-1, get_vol(soundinfo[i].owner));
	    }
	}
      
      if (Mix_Playing(i-1))
	{
	  if (soundinfo[i].owner != 0)
	    {
	      if (spr[soundinfo[i].owner].active == /*false*/0)
		{
		  Mix_HaltChannel(i-1);
		}
	      else
		{
		  SetPan(i-1, get_pan(soundinfo[i].owner));
		  SetVolume(i-1, get_vol(soundinfo[i].owner));
		}
	    }
	}
    }
}



/**
 * Just play a sound, do not try to update sprites info or apply
 * effects
 */
int EditorSoundPlayEffect(int sound)
{
  if (Mix_PlayChannel(0, registered_sounds[sound].sound, 0) < 0)
    {
      fprintf(stderr, "Mix_PlayChannel: Error playing sound %d - %s\n",
	      sound, Mix_GetError());
      return 0;
    }
  return 1;
}


/* SoundPlayEffect */
/* TODO:
 * - min: frequency (Hz)
 * - plus: max random frequency, to add to min
 * (Check playbank in the original source code)
 **/

int SoundPlayEffect(int sound, int min, int plus, int sound3d, /*bool*/int repeat)
{
  int channel;
  int channel_index;

  channel = Mix_PlayChannel(-1, registered_sounds[sound].sound, repeat ? -1 : 0);

  if (channel < 0)
    {
      fprintf(stderr, "Mix_PlayChannel: Error playing sound %d - %s\n",
	      sound, Mix_GetError());
      return 0;
    }


/*   int bank = playbank (sound, min, plus, sound3d, repeat); */

  if (sound3d > 0)
    {
      SetPan(channel, get_pan(sound3d));
      SetVolume(channel, get_vol(sound3d));
    }

  /* TODO */
/*   if (plus == 0) */
/*     soundbank[i]->SetFrequency (min); */
/*   else */
/*     soundbank[i]->SetFrequency ((rand () % plus) + min); */

  channel_index = channel + 1;
  soundinfo[channel_index].owner = sound3d;
  soundinfo[channel_index].repeat = repeat;
  soundinfo[channel_index].survive = 0;
  soundinfo[channel_index].vol = 0;

  /* Return a non-zero channel */
  return channel_index;
}
        
/*
 * SoundStopEffect
 *
 * Stops the sound effect specified.
 * Returns TRUE if succeeded.
 */
int SoundStopEffect( int sound )
{
  int channel;

  if (sound >= MAX_SOUNDS)
    {
      fprintf(stderr, "Attempting to get stop sound %d (> MAX_SOUNDS=%d)",
	      sound, MAX_SOUNDS);
      return 0;
    }

  channel = get_channel(sound);
  if (channel < 0)
    return 0;
  else
    return Mix_HaltChannel(channel); /* always returns 0 */
}


/*
 * InitSound
 *
 * Sets up the DirectSound object and loads all sounds into secondary
 * DirectSound buffers.  Returns FALSE on error, or TRUE if successful
 */
int InitSound()
{
  Msg("initting sound");

  if (SDL_Init(SDL_INIT_CDROM | SDL_INIT_AUDIO) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return 0;
    }
  
  // Initialize CD-ROM
  {
    SDL_CD *cdrom;
    
    /* Check for CD drives */
    if(!SDL_CDNumDrives()){
      /* None found */
      Msg (("No CDROM devices available\n"));
    }
    
    /* Open the default drive */
    cdrom = SDL_CDOpen(0);
    
    /* Did if open? Check if cdrom is NULL */
    if (!cdrom)
      Msg("Couldn't open drive: %s\n", SDL_GetError());
    
    if (CD_INDRIVE(SDL_CDStatus(cdrom))) {
      /* TODO: do some test about the presence of audio tracks in the
         CD - though fortunately SDL_Mixer does not read data track */
      cd_inserted = /*true*/1;
    } 
    
    /* This newly opened CD-ROM becomes the default CD used when other
       CD functions are passed a NULL CD-ROM handle. */
  }
  
  /* MIX_DEFAULT_FREQUENCY is ~22kHz are considered a good default,
     44kHz is considered too CPU-intensive on older computers */
  /* MIX_DEFAULT_FORMAT is 16bit adapted to current architecture
     (little/big endian) */
  /* MIX_DEFAULT_CHANNELS is 2 => stereo, allowing panning effects */
  /* 1024 (chunk on which effects are applied) seems a good default,
     4096 is considered too big for SFX */
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1)
    {
      Msg("Mix_OpenAudio: %s", Mix_GetError());
      return 0;
    }

  /* Allocate channels to play effects to */
  Mix_AllocateChannels(NUM_SOUNDBANKS);

  /* Done with initialization */
  /* Avoid calling SDL_PauseAudio when using SDL_Mixer */
  /* SDL_PauseAudio(0); */
  
  
  /* Dump audio info */
  {
    int numtimesopened, frequency, channels;
    Uint16 format;
    numtimesopened = Mix_QuerySpec(&frequency, &format, &channels);
    if (!numtimesopened)
      {
	printf("Mix_QuerySpec: %s\n", Mix_GetError());
      }
    else
      {
	char *format_str = "Unknown";
	switch (format)
	  {
	  case AUDIO_U8: format_str = "U8"; break;
	  case AUDIO_S8: format_str = "S8"; break;
	  case AUDIO_U16LSB: format_str = "U16LSB"; break;
	  case AUDIO_S16LSB: format_str = "S16LSB"; break;
	  case AUDIO_U16MSB: format_str = "U16MSB"; break;
	  case AUDIO_S16MSB: format_str = "S16MSB"; break;
	  }
	printf("Audio info: opened=%d times  frequency=%dHz  format=%s  channels=%d\n",
	       numtimesopened, frequency, format_str, channels);
      }
  }

  return 1;
}

/**
 * Undoes everything that was done in a InitSound call
 */
void DestroySound(void)
{
  int idxKill;
  
  /* Stops all SFX channels */
  Mix_HaltChannel(-1);
  
  /**
   * Frees up resources associated with a sound effect
   */
  for (idxKill = 0; idxKill < MAX_SOUNDS; idxKill++)
    {
      if (registered_sounds[idxKill].sound != NULL)
        {
          Mix_FreeChunk(registered_sounds[idxKill].sound);
          registered_sounds[idxKill].sound = NULL;
        }
    }

  Mix_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO | SDL_INIT_CDROM);
}


/* Set volume; dx_volume is [-10000;10000] in hundredth of dB */
int SetVolume(int channel, int dx_volume)
{
/*   soundbank[i]->SetVolume(get_vol(soundinfo[i].owner)); */
  // SFX
  /* See doc/sound.txt for details */
  return Mix_Volume(channel, MIX_MAX_VOLUME * pow(10, ((double)dx_volume / 100) / 20));
}

/* Set left/right balance; dx_panning is [-10000;10000] in hundredth
   of dB, -ive is right channel attenuation, +ive is left channel
   attenuation */
int SetPan(int channel, int dx_panning)
{
/*   soundbank[i]->SetPan(get_pan(soundinfo[i].owner)); */
  // SFX
  /* See doc/sound.txt for details */
  if (dx_panning > 0)
    return Mix_SetPanning(channel, 255 * pow(10, ((double)-dx_panning / 100) / 20), 255);
  else
    return Mix_SetPanning(channel, 255, 255 * pow(10, ((double)dx_panning / 100) / 20));
}
