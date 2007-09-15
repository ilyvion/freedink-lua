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

soundstruct soundinfo[num_soundbanks+1];

const int max_sounds = 100;
static struct
{
  Mix_Chunk *sound;
} registered_sounds[max_sounds];

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
        fprintf(stderr, "Error loading %s - %s", filename, Mix_GetError());
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
  for (i = 0; i < num_soundbanks; i++)
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
  if (sound >= max_sounds)
    {
      Msg("Attempting to get the status of sound %d (> MAX_SOUNDS=%d)",
          sound, max_sounds);
      return 0;
    }

  return (get_channel(sound) != -1);
}
        

/**
 * Kill repeating sounds except the ones that survive
 */
void kill_repeat_sounds(void)
{
  if (!sound_on)
    return;
  
  Msg("Killing repeating sound");

  for (int i=1; i <= num_soundbanks; i++)
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
  if (!sound_on)
    return;
  
  for (int i = 1; i <= num_soundbanks; i++)
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
  /* Note about volume in DirectSound:
setVolume
public void setVolume(int volume);


Changes the volume of a sound buffer. 

Return Value: 

No return value. 

Parameter Description 
volume  New volume requested for this sound buffer. Values range from 0 (0 dB, no volume adjustment) to -10,000 (-100 dB, essentially silent). DirectSound does not currently support amplification.  


Remarks: 

Volume units of are in hundredths of decibels, where 0 is the original volume of the stream. 

Positive decibels correspond to amplification and negative decibels correspond to attenuation. The decibel scale corresponds to the logarithmic hearing characteristics of the ear. An attenuation of 10 dB makes a buffer sound half as loud; an attenuation of 20 dB makes a buffer sound one quarter as loud. DirectSound does not currently support amplification. 

The pan control is cumulative with the volume control. 

See Also: getPan, getVolume, setPan

CONCLUSION: we need some conversion [0-128] -> [-10000, 0](log)

=====

Same for PAN:
setPan
public void setPan(int pan);


Specifies the relative volume between the left and right channels. 

Return Value: 

No return value. 

Parameter Description 
pan  Relative volume between the left and right channels. This value has a range of -10,000 to 10,000 and is measured in hundredths of a decibel.  


Remarks: 

0 is the neutral value for pan and indicates that both channels are at full volume (attenuated by 0 decibels). At any other setting, one of the channels is at full volume and the other is attenuated. For example, a pan of -2173 means that the left channel is at full volume and the right channel is attenuated by 21.73 dB. Similarly, a pan of 870 means that the left channel is attenuated by 8.7 dB and the right channel is at full volume. 

A pan of -10,000 means that the right channel is silent and the sound is "all the way to the left," while a pan of 10,000 means that the left channel is silent and the sound is "all the way to the right." 

The pan control is cumulative with the volume control. 

See Also: getPan, getVolume, setVolume 

=====

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

  if (!sound_on)
    return;
  
  for (int i = 1; i <= num_soundbanks; i++)
    {
      if (soundinfo[i].repeat && (soundinfo[i].owner != 0))
	{
	  if ((spr[soundinfo[i].owner].sound == 0)
	       || (spr[soundinfo[i].owner].active == false) )
	    {
	      Mix_HaltChannel(i-1);
	      soundinfo[i].owner = 0;
	      soundinfo[i].repeat = 0;
	    }
	  else
	    {
	      /* TODO */
	      // soundbank[i]->SetPan(get_pan(soundinfo[i].owner));
	      // soundbank[i]->SetVolume(get_vol(soundinfo[i].owner));
	    }
	}
      
      if (Mix_Playing(i-1))
	{
	  if (soundinfo[i].owner != 0)
	    {
	      if (spr[soundinfo[i].owner].active == false)
		{
		  Mix_HaltChannel(i-1);
		}
	      else
		{
		  /* TODO */
		  // soundbank[i]->SetPan(get_pan(soundinfo[i].owner));
		  // soundbank[i]->SetVolume(get_vol(soundinfo[i].owner));
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
 * - sound3d: use get_pan(sound3d) and get_vol(sound3d);
 *   seems to be the sprite the sound is attached to.
 * - min: frequency (Hz)
 * - plus: max random frequency, to add to min
 * (Check playbank in the original source code)
 **/

int SoundPlayEffect(int sound, int min, int plus, int sound3d, bool repeat)
{
  int channel;
  int channel_index;

  /* TODO: adjust channel params (min, plus, sound3d) before to play */

  channel = Mix_PlayChannel(-1, registered_sounds[sound].sound, repeat ? -1 : 0);

  if (channel < 0)
    {
      fprintf(stderr, "Mix_PlayChannel: Error playing sound %d - %s\n",
	      sound, Mix_GetError());
      return 0;
    }

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

  if (sound >= max_sounds)
    {
      fprintf(stderr, "Attempting to get stop sound %d (> MAX_SOUNDS=%d)",
	      sound, max_sounds);
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
      Msg (("Couldn't open drive: %s\n", SDL_GetError()));
    
    if (CD_INDRIVE(SDL_CDStatus(cdrom))) {
      /* TODO: do some test about the presence of audio tracks in the
         CD - though fortunately SDL_Mixer does not read data track */
      cd_inserted = true;
    } 
    
    /* This newly opened CD-ROM becomes the default CD used when other
       CD functions are passed a NULL CD-ROM handle. */
  }
  
  // TODO: parameters values are random / not really justified
  /* sabetts: Mix_OpenAudio (MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024) */
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024) == -1)
    {
      Msg (("Mix_OpenAudio: %s", Mix_GetError ()));
      return 0;
    }

  /* Allocate channels to play effects to */
  Mix_AllocateChannels(num_soundbanks);

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
  for (idxKill = 0; idxKill < max_sounds; idxKill++)
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
