/**
 * Sound effects (not music) management

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

#include "SDL.h"
#include "SDL_mixer.h"
#include "game_engine.h"
#include "io_util.h"
#include "paths.h"
#include "log.h"
#include "math.h"
#include "sfx.h"


/* Channel metadata */
#define NUM_CHANNELS 20
struct
{
  /*bool*/int repeat;
  int owner;
  int survive;
  int cur_sound; /* Sound currently played in that channel */
  Uint8 *fake_buf;
} channelinfo[NUM_CHANNELS];


/* Sound metadata */
#define MAX_SOUNDS 100
static struct
{
  SDL_AudioSpec orig_spec;
  Uint32 orig_len;
  int pos_lastframe; /* index for the last frame */
  int pos_end; /* upper bound (pre-calculated for efficiency&simplicity) */
  SDL_AudioCVT cvt;
} registered_sounds[MAX_SOUNDS];


/* Hardware soundcard information */
static int hw_freq, hw_channels;
static Uint16 hw_format;


static int SetVolume(int channel, int dx_volume);
static int SetPan(int channel, int dx_panning);
static void CleanupChannel(int channel);
static void FreeRegisteredSound(int index);


/**
 * Display a SDL audio-format in human-readable form
 **/
static const char *format2string(Uint16 format) {
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
  return format_str;
}



/**
 * Frequency shift with linear interpolation
 * 
 * Techniques from Allegro's mixer.c
 * 
 * We'll assume 8bit is unsigned and 16bit is signed for simplicity
 * (common case)
 *
 * We'll also assume that the converted buffer is in the system's
 * endianness (there's no reason it shouldn't be) - still the code
 * portably handles LSB and MSB endianness alike.
 */
struct callback_data
{
  int pos; /* which sample (not frame) we're playing now, in 1/256th */
  int shift; /* position advance for 1 frame, in 1/256th */
  int sound; /* sound index */
};
static void callback_samplerate_cleanup(int chan, void *udata)
{
  free(udata);
}
static void callback_samplerate(int chan, void *stream, int len, void *udata)
{
  struct callback_data* data = (struct callback_data*)udata;

  /* printf("%d - %d/%d\n", len, data->pos>>8, registered_sounds[data->sound].pos_end>>8); */
  int pos_end = registered_sounds[data->sound].pos_end;
  int pos_lastframe = registered_sounds[data->sound].pos_lastframe;

  switch (registered_sounds[data->sound].cvt.dst_format)
    {
    case AUDIO_U8:
      if (hw_channels == 1)
	{
	  /* Unsigned 8bit mono */
	  Uint8 *buf = (Uint8*) registered_sounds[data->sound].cvt.buf;
	  Uint8 *pstream = (Uint8*)stream;
	  short silence = 128;
	  int bytesPerFrame = 1;
	  int i;
	  for (i = 0; i < len; i += bytesPerFrame)
	    {
	      int cur_index = data->pos >> 8;
	      Uint8 *pbuf = &buf[cur_index];
	      /* This frame (left and right channels)... */
	      Uint8 v1 = *(pbuf++);
	      /* and the next frame */
	      Uint8 v2;
	      if (data->pos < pos_lastframe)
		  v2 = *(pbuf++);
	      else
		  v2 = silence;
	      
	      /* Compute interpolation based on progress (subpos) between v1
		 and v2 (measured between 0 and (1<<8)-1) */
	      int subpos = data->pos & ((1<<8) - 1); /* or: data->pos % 256 */
	      Uint8 v = (v1 * ((1<<8) - subpos) + v2 * subpos) >> 8;
	      *(pstream++) = v;
	      
	      data->pos += data->shift;
	      if (data->pos >= pos_end)
		data->pos = 0;
	    }
 	}
      else if (hw_channels == 2)
	{
	  /* Unsigned 8bit stereo */
	  Uint8 *buf = (Uint8*) registered_sounds[data->sound].cvt.buf;
	  Uint8 *pstream = (Uint8*)stream;
	  short silence = 128;
	  int bytesPerFrame = 2;
	  int i;
	  for (i = 0; i < len; i += bytesPerFrame)
	    {
	      int cur_index = data->pos >> 8 << 1; /* x2 because stereo */
	      Uint8 *pbuf = &buf[cur_index];
	      /* This frame (left and right channels)... */
	      Uint8 v1l = *(pbuf++);
	      Uint8 v1r = *(pbuf++);
	      /* and the next frame */
	      Uint8 v2l, v2r;
	      if (data->pos < pos_lastframe)
		{
		  v2l = *(pbuf++);
		  v2r = *(pbuf++);
		}
	      else
		{
		  v2l = v2r = silence;
		}
	      
	      /* Compute interpolation based on progress (subpos) between v1
		 and v2 (measured between 0 and (1<<8)-1) */
	      int subpos = data->pos & ((1<<8) - 1); /* or: data->pos % 256 */
	      Uint8 vl = (v1l * ((1<<8) - subpos) + v2l * subpos) >> 8;
	      Uint8 vr = (v1r * ((1<<8) - subpos) + v2r * subpos) >> 8;
	      *(pstream++) = vl;
	      *(pstream++) = vr;
	      
	      data->pos += data->shift;
	      if (data->pos >= pos_end)
		data->pos = 0;
	    }
	}
      break;
    case AUDIO_S16SYS:
      if (hw_channels == 1)
	{
	  /* Signed 16bit mono */
	  Sint16 *buf = (Sint16*) registered_sounds[data->sound].cvt.buf;
	  Sint16 *pstream = (Sint16*)stream;
	  short silence = 0;
	  int bytesPerFrame = 2;
	  int i;
	  for (i = 0; i < len; i += bytesPerFrame)
	    {
	      int cur_index = data->pos >> 8;
	      Sint16 *pbuf = &buf[cur_index];
	      /* This frame (left and right channels)... */
	      Sint16 v1 = *(pbuf++);
	      /* and the next frame */
	      Sint16 v2;
	      if (data->pos < pos_lastframe)
		  v2 = *(pbuf++);
	      else
		  v2 = silence;
	      
	      /* Compute interpolation based on progress (subpos) between v1
		 and v2 (measured between 0 and (1<<8)-1) */
	      int subpos = data->pos & ((1<<8) - 1); /* or: data->pos % 256 */
	      Sint16 v = (v1 * ((1<<8) - subpos) + v2 * subpos) >> 8;
	      *(pstream++) = v;
	      
	      data->pos += data->shift;
	      if (data->pos >= pos_end)
		data->pos = 0;
	    }
	}
      else if (hw_channels == 2)
	{
	  /* Signed 16bit stereo */
	  Sint16 *buf = (Sint16*) registered_sounds[data->sound].cvt.buf;
	  Sint16 *pstream = (Sint16*)stream;
	  short silence = 0;
	  int bytesPerFrame = 4;
	  int i;
	  for (i = 0; i < len; i += bytesPerFrame)
	    {
	      int cur_index = (data->pos >> 8) << 1; /* x2 because stereo */
	      Sint16 *pbuf = &buf[cur_index];
	      /* This frame (left and right channels)... */
	      Sint16 v1l = *(pbuf++);
	      Sint16 v1r = *(pbuf++);
	      /* and the next frame */
	      Sint16 v2l, v2r;
	      if (data->pos < pos_lastframe)
		{
		  v2l = *(pbuf++);
		  v2r = *(pbuf++);
		}
	      else
		{
		  v2l = v2r = silence;
		}
	      
	      /* Compute interpolation based on progress (subpos) between v1
		 and v2 (measured between 0 and (1<<8)-1) */
	      int subpos = data->pos & ((1<<8) - 1); /* or: data->pos % 256 */
	      Sint16 vl = (v1l * ((1<<8) - subpos) + v2l * subpos) >> 8;
	      Sint16 vr = (v1r * ((1<<8) - subpos) + v2r * subpos) >> 8;
	      *(pstream++) = vl;
	      *(pstream++) = vr;
	      
	      data->pos += data->shift;
	      if (data->pos >= pos_end)
		data->pos = 0;
	    }
	}
      break;
    }
}




/**
 * Load sounds from the standard paths. Sound is converted to the
 * current hardware (soundcard) format so that sound rate can be
 * altered with minimal overhead when playing the sound.
 */
int CreateBufferFromWaveFile(char* filename, int index)
{
  /* Open the wave file */
  char path[150];
  FILE* in = NULL;

  sprintf(path, "sound/%s", filename);
  in = paths_dmodfile_fopen(path, "rb");
  if (in == NULL)
    in = paths_fallbackfile_fopen(path, "rb");
  if (in == NULL)
    {
      perror("CreateBufferFromWaveFile");
      return 0;
    }

  SDL_RWops* rwops = SDL_RWFromFP(in, /*autoclose=*/1);
  return CreateBufferFromWaveFile_RW(rwops, 1, index);
}
int CreateBufferFromWaveFile_RW(SDL_RWops* rwops, int rwfreesrc, int index)
{
  SDL_AudioSpec wav_spec;
  Uint8 *wav_buf;
  Uint32 wav_len;

  // Safety check
  if (index >= MAX_SOUNDS)
    {
      fprintf(stderr, "SCRIPTING ERROR: sound index %d is too big.\n", index);
      return 0;
    }

  // Free previous sound if necessary
  FreeRegisteredSound(index);


  if (SDL_LoadWAV_RW(rwops, rwfreesrc, &wav_spec, &wav_buf, &wav_len) == NULL)
    {
      fprintf(stderr, "Could not open sound file: %s\n", SDL_GetError());
      return 0;
    }
  printf("info: frequency=%dHz\tformat=%s\tchannels=%d\tlength=%d bytes\n",
	 wav_spec.freq, format2string(wav_spec.format),
	 wav_spec.channels, wav_len);


  /* Converting some WAV data to hardware format - except for sample
     rate, which we will do better after that. */
  /* TODO: testing with a sine.wav, I saw that stereo->mono (2->1
     channels) conversion works quite bad - maybe we should it
     ourselves too. AFAICS this case is not handled by
     SDL_audiocvt.c. */
  SDL_AudioCVT cvt;
  int ret;
  
  /* Build AudioCVT */
  ret = SDL_BuildAudioCVT(&cvt,
			  wav_spec.format, wav_spec.channels, wav_spec.freq,
			  hw_format, hw_channels, wav_spec.freq);
  
  /* Check that the convert was built */
  if (ret == -1) {
    fprintf(stderr, "Couldn't build converter: %s\n", SDL_GetError());
    SDL_FreeWAV(wav_buf);
  }

  /* Setup for conversion */
  cvt.buf = (Uint8 *)malloc(wav_len*cvt.len_mult);
  cvt.len = wav_len;
  memcpy(cvt.buf, wav_buf, wav_len);
  
  /* We can delete to original WAV data now */
  SDL_FreeWAV(wav_buf);

  
  /* And now we're ready to convert */
  ret = SDL_ConvertAudio(&cvt);
  if (ret == -1) {
    fprintf(stderr, "Couldn't convert audiox: %s\n", SDL_GetError());
    SDL_FreeWAV(wav_buf);
  }
  
  /* Work-around: if no conversion is needed, format is not specified: */
  if (cvt.needed == 0)
    cvt.dst_format = wav_spec.format;

  /* Converted audio is now in cvt.buf */
  /*printf("CVT\t\tinfo: frequency=?Hz\tformat=%s\tchannels=??\tlength=%d bytes\n",
      format2string(cvt.dst_format), cvt.len_cvt);*/

  /* Precompute the sound bounds */
  /* Last byte in hw_format is the number of bits per sample: */
  int wav_bytesPerSample = (wav_spec.format & 0xFF) / 8;
  int pos_end = ((wav_len / wav_bytesPerSample) / wav_spec.channels) << 8; /* number of frames */
  int pos_lastframe = (((wav_len / wav_bytesPerSample) / wav_spec.channels) - 1) << 8;

  registered_sounds[index].orig_spec = wav_spec;
  registered_sounds[index].orig_len = wav_len;
  registered_sounds[index].cvt = cvt;
  registered_sounds[index].pos_end = pos_end;
  registered_sounds[index].pos_lastframe = pos_lastframe;
  
  return 1;
}

/**
 * Return the channel that plays the specified sound, or -1 if not
 * found
 */
int get_channel(int sound) {
  int i;
  /* Check all channels to see if it is playing the sound */
  for (i = 0; i < NUM_CHANNELS; i++)
    {
      if (channelinfo[i].cur_sound == sound)
        return i;
    }
  return -1;
}

/**
 * Is the specified sound currently playing?
 * 
 * Only used in pig_brain(), could be removed maybe.
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

  for (i = 0; i < NUM_CHANNELS; i++)
    {
      // Msg("Bank #%d: repeat=%d, owner=%d, survive=%d", i,
      //   soundinfo[i].repeat, soundinfo[i].owner, soundinfo[i].survive);
      if (channelinfo[i].repeat && (channelinfo[i].owner == 0)
          && (channelinfo[i].survive == 0))
        {
          Mix_HaltChannel(i);
	  Msg("Killed repeating sound %d", i);
          channelinfo[i].repeat = 0;
        }
    }
}

/**
 * Kill all repeating sounds, even the ones that survive (used from
 * DinkC's restart_game() and load_game())
 */
void kill_repeat_sounds_all(void)
{
  int i;
  if (!sound_on)
    return;
  
  for (i = 0; i < NUM_CHANNELS; i++)
    {
      if (channelinfo[i].repeat && (channelinfo[i].owner == 0))
        {
          Mix_HaltChannel(i);
          channelinfo[i].repeat = 0;
        }
    }
}

/**
 * Kill one sound
 */
void kill_this_sound(int channel)
{
  Mix_HaltChannel(channel);
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
  int i;

  if (!sound_on)
    return;
  
  for (i = 0; i < NUM_CHANNELS; i++)
    {
      if (channelinfo[i].repeat && (channelinfo[i].owner != 0))
	{
	  if ((spr[channelinfo[i].owner].sound == 0)
	      || (spr[channelinfo[i].owner].active == /*false*/0) )
	    {
	      Mix_HaltChannel(i);
	      channelinfo[i].owner = 0;
	      channelinfo[i].repeat = 0;
	    }
	  else
	    {
	      SetPan(i, get_pan(channelinfo[i].owner));
	      SetVolume(i, get_vol(channelinfo[i].owner));
	    }
	}
      
      if (Mix_Playing(i))
	{
	  if (channelinfo[i].owner != 0)
	    {
	      if (spr[channelinfo[i].owner].active == /*false*/0)
		{
		  Mix_HaltChannel(i);
		}
	      else
		{
		  SetPan(i, get_pan(channelinfo[i].owner));
		  SetVolume(i, get_vol(channelinfo[i].owner));
		}
	    }
	}
    }
}


static int SoundPlayEffectChannel(int sound, int min, int plus, int sound3d, /*bool*/int repeat, int explicit_channel);

/**
 * Just play a sound, do not try to update sprites info or apply
 * effects
 */
void EditorSoundPlayEffect(int sound)
{
  /* Don't print warning if the sound isn't present - as sounds are
     played continuously when arrow keys are pressed */
  if (registered_sounds[sound].cvt.buf != NULL)
    SoundPlayEffectChannel(sound, registered_sounds[sound].orig_spec.freq, 0, 0, 0, 0);
}

/**
 * Play a sound previously loaded to memory (in registered_sounds)
 * - sound: sound index
 * - min: frequency (Hz)
 * - plus: max random frequency, to add to min
 * - sound3d: if != 0, sprite number whose location will be used for
 *   pseudo-3d effects (volume, panning)
 * - repeat: is sound looping?
 **/
int SoundPlayEffect(int sound, int min, int plus, int sound3d, /*bool*/int repeat)
{
  return SoundPlayEffectChannel(sound, min, plus, sound3d, repeat, -1);
}
/**
 * SoundPlayEffect_Channel_ allows to specify an explicit audio
 * channel, which in turns allows the editor to only use one channel
 * for everything (when you move the mouse with the keyboard, you'll
 * hear a series of close 'ticks', but they won't overlap each
 * others). The rest of the time, the game will just pass '-1' for the
 * channel, so the first available channel (among NUM_CHANNELS useable
 * simultaneously) will be selected.
 */
static int SoundPlayEffectChannel(int sound, int min, int plus, int sound3d, /*bool*/int repeat, int explicit_channel)
{
  int channel;

  // Safety check
  if (registered_sounds[sound].cvt.buf == NULL)
    {
      fprintf(stderr, "Error: attempting to play empty sound %d.\n", sound);
      return 0;
    }

  /* Sample rate / frequency */
  {
    /** Shift:
	hw_adjust = wave_freq / hw_freq;
	sample_rate_adjust = play_freq / wave_freq
	shift = hw_adjust * sample_rate_adjust = wave_freq/hw_freq * play_freq/wave_freq
	<<8 because we're using 1/256th units (poor man's double)
  */
    int play_freq;
    if (plus == 0)
      play_freq = min;
    else
      play_freq = (rand () % plus) + min;
    
    /* Compute how much we should advance in the original sound when we
       play one frame with hw_freq */
    int shift = ((int)round((double)play_freq / hw_freq * (1<<8)));
    /* printf("shift=%d (%d*64)\n", shift, shift>>8); */
    
    /** Create a junk wav of the right size **/
    int resized_len = registered_sounds[sound].cvt.len_cvt * ((double)hw_freq / play_freq);

    /* Fake buffer: we give an empty buffer to SDL_mixer and a size
       information that matches the sound sample with the target
       sample rate, so the mixer will play and stop the sound
       appropriately. We won't actually play from that buffer though,
       as the audio buffer will be generated in
       callback_samplerate(). */
    /* We used to share the buffer among channels, but it made the
       engine crash if the buffer was realloc()'d to store a bigger
       sound _and_ moved to a different memory address in the process
       (while other existing sounds would still refer to the old,
       freed memory location). */
    Uint8* fake_buf = NULL;
    fake_buf = malloc(resized_len);
    Mix_Chunk *chunk = Mix_QuickLoad_RAW(fake_buf, resized_len);
    /* printf("resized_len=%d bytes, pos_end=%d\n", resized_len, pos_end); */
    
    channel = Mix_PlayChannel(explicit_channel, chunk, repeat ? -1 : 0);
    if (channel < 0)
      {
	fprintf(stderr, "Mix_PlayChannel: Error playing sound %d - %s\n",
		sound, Mix_GetError());
	return 0;
      }
    channelinfo[channel].fake_buf = fake_buf;

    struct callback_data *data = calloc(1, sizeof(struct callback_data));
    data->pos = 0;
    data->shift = shift;
    data->sound = sound;

    Mix_RegisterEffect(channel, callback_samplerate, callback_samplerate_cleanup, data);
    Mix_ChannelFinished(CleanupChannel);
  }


  if (sound3d > 0)
    {
      SetPan(channel, get_pan(sound3d));
      SetVolume(channel, get_vol(sound3d));
    }


  channelinfo[channel].owner = sound3d;
  channelinfo[channel].repeat = repeat;
  channelinfo[channel].survive = 0;
  channelinfo[channel].cur_sound = sound;

  /* Return a non-zero channel */
  return channel+1;
}
        
/**
 * SoundStopEffect
 *
 * Stops the sound effect specified.
 * Returns TRUE if succeeded.
 */
int SoundStopEffect(int sound)
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


/**
 * InitSound
 *
 * Sets up the DirectSound object and loads all sounds into secondary
 * DirectSound buffers.  Returns -1 on error, or 0 if successful
 */
int InitSound()
{
  Msg("initting sound");

  if (SDL_Init(SDL_INIT_AUDIO) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return -1;
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
      return -1;
    }

  /* Allocate channels (not mono/stereo, but the simultaneous sounds
     to be mixed, possibly with effects) */
  Mix_AllocateChannels(NUM_CHANNELS);

  /* Done with initialization */
  /* Avoid calling SDL_PauseAudio when using SDL_Mixer */
  /* SDL_PauseAudio(0); */
  
  /* Dump audio info */
  {
    int numtimesopened;
    numtimesopened = Mix_QuerySpec(&hw_freq, &hw_format, &hw_channels);
    if (!numtimesopened)
      printf("Mix_QuerySpec: %s\n", Mix_GetError());
    else
      printf("Audio hardware info: frequency=%dHz\tformat=%s\tchannels=%d\topened=%d times\n",
	     hw_freq, format2string(hw_format), hw_channels, numtimesopened);
  }

  /* No sound playing yet - initialize the lookup table: */
  memset(channelinfo, 0, sizeof(channelinfo));
  int i;
  for (i = 0; i < NUM_CHANNELS; i++)
    channelinfo[i].cur_sound = -1;

  /* No sound loaded yet - initialise the registered sounds: */
  memset(registered_sounds, 0, sizeof(registered_sounds));
  /* Make sure they won't be used: */
  for (i = 0; i < MAX_SOUNDS; i++)
    registered_sounds[i].cvt.buf = NULL;
  
  return 0;
}

/**
 * Undoes everything that was done in a InitSound call
 */
void QuitSound(void)
{
  int idxKill;
  
  /* Stops all SFX channels */
  Mix_HaltChannel(-1);
  
  /**
   * Frees up resources associated with a sound effect
   */
  for (idxKill = 0; idxKill < MAX_SOUNDS; idxKill++)
    FreeRegisteredSound(idxKill);

  Mix_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

/**
 * Free memory used by sound #'sound'
 */
static void FreeRegisteredSound(int sound)
{
  if (registered_sounds[sound].cvt.buf != NULL)
    /* cvt.buf was malloc'd by us before. It's used both as source and
       destination by 'SDL_ConvertAudio', so it's not realloc'd in the
       process, and we use 'free' (and not a SDL-specific func): */
    free(registered_sounds[sound].cvt.buf);

  memset(&registered_sounds[sound], 0, sizeof (registered_sounds[sound]));
  /* Make sure it won't be reused: */
  registered_sounds[sound].cvt.buf = NULL;
}

/**
 * Free chunk once it's played (through Mix_ChannelFinished()
 * callback)
 */
static void CleanupChannel(int channel)
{
  /* SDL_mixer won't try to free the actual audio buffer (fake_buf) if
     the Chunk was loaded via Mix_QuickLoad_RAW() - which is a good
     thing since fake_buf is shared by all Chunks. We still need to
     free the Chunk when it's done playing. */
  Mix_Chunk *chunk = Mix_GetChunk(channel);
  if (chunk == NULL)
    {
      printf("Internal error: cannot free channel %d's chunk (where did it disappear?)\n", channel);
      exit(1);
    }
  Mix_FreeChunk(chunk);
  /* WAV buffer is not freed when Chunk is created using
     Mix_QuickLoad_RAW (cf. chunk->allocated), do it manually. */
  free(channelinfo[channel].fake_buf);
  channelinfo[channel].cur_sound = -1;

  /* Revert SetVolume and SetPan effects */
  Mix_UnregisterAllEffects(channel);
  Mix_Volume(channel, MIX_MAX_VOLUME);
}


/**
 * Set volume; dx_volume is [-10000;10000] in hundredth of dB
 */
static int SetVolume(int channel, int dx_volume)
{
  // SFX
  /* See doc/sound.txt for details */
  return Mix_Volume(channel, MIX_MAX_VOLUME * pow(10, ((double)dx_volume / 100) / 20));
}


/**
 * Set left/right balance; dx_panning is [-10000;10000] in hundredth
 * of dB, -ive is right channel attenuation, +ive is left channel
 * attenuation
 */
static int SetPan(int channel, int dx_panning)
{
  // SFX
  /* See doc/sound.txt for details */
  if (dx_panning > 0)
    return Mix_SetPanning(channel, 255 * pow(10, ((double)-dx_panning / 100) / 20), 255);
  else
    return Mix_SetPanning(channel, 255, 255 * pow(10, ((double)dx_panning / 100) / 20));
}


/** DinkC procedures **/
/* BIG FAT WARNING: in DinkC, soundbank is channel+1
   (a.k.a. non-zero), and 0 means failure. */
int playsound(int sound, int min, int plus, int sound3d, int repeat)
{
  int channel = SoundPlayEffect(sound, min, plus, sound3d, repeat);
  int soundbank = channel + 1;
  return soundbank;
}
void sound_set_kill(int soundbank)
{
  int channel = soundbank - 1;
  Mix_HaltChannel(channel);
}
void sound_set_survive(int soundbank, int survive)
{
  int channel = soundbank - 1;
  channelinfo[channel].survive = survive;
}
void sound_set_vol(int soundbank, int volume)
{
  int channel = soundbank - 1;
  SetVolume(channel, volume);
}
