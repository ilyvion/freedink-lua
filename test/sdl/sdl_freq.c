/**
 * Play a sound at a different frequency / sample rate

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

#include <stdio.h>
#include <math.h>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

int hw_freq, hw_channels;

const char *format2string(Uint16 format) {
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

struct data
{
  Mix_Chunk *chunk;
  int pos; /* which sample (not frame) we're playing now, in 1/256th */
  int shift; /* position advance for 1 frame, in 1/256th */
  int pos_lastframe; /* index for the last frame */
  int pos_end; /* upper bound */
  SDL_AudioSpec orig_spec;
  SDL_AudioCVT cvt;
  int play_freq;
};

/* Frequency shift with linear interpolation */
/* Techniques from Allegro's mixer.c */
/* We'll assume 8bit is unsigned and 16bit is signed for simplicity
   (common case) */
/* We'll also assume that the converted buffer is in the system's
   endianness (there's no reason it shouldn't be) - still the code
   portably handles LSB and MSB endianness alike. */
void callback_effect(int chan, void *stream, int len, void *udata)
{
  struct data* data = (struct data*)udata;

  printf("%d - %d/%d\n", len, data->pos>>8, data->pos_end>>8);

  switch (data->cvt.dst_format)
    {
    case AUDIO_U8:
      if (hw_channels == 1)
	{
	  /* Unsigned 8bit mono */
	  Uint8 *buf = (Uint8*) data->cvt.buf;
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
	      if (data->pos < data->pos_lastframe)
		  v2 = *(pbuf++);
	      else
		  v2 = silence;
	      
	      /* Compute interpolation based on progress (subpos) between v1
		 and v2 (measured between 0 and (1<<8)-1) */
	      int subpos = data->pos & ((1<<8) - 1); /* or: data->pos % 256 */
	      Uint8 v = (v1 * ((1<<8) - subpos) + v2 * subpos) >> 8;
	      *(pstream++) = v;
	      
	      data->pos += data->shift;
	      if (data->pos >= data->pos_end)
		data->pos = 0;
	    }
 	}
      else if (hw_channels == 2)
	{
	  /* Unsigned 8bit stereo */
	  Uint8 *buf = (Uint8*) data->cvt.buf;
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
	      if (data->pos < data->pos_lastframe)
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
	      if (data->pos >= data->pos_end)
		data->pos = 0;
	    }
	}
      break;
    case AUDIO_S16SYS:
      if (hw_channels == 1)
	{
	  /* Signed 16bit mono */
	  Sint16 *buf = (Sint16*) data->cvt.buf;
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
	      if (data->pos < data->pos_lastframe)
		  v2 = *(pbuf++);
	      else
		  v2 = silence;
	      
	      /* Compute interpolation based on progress (subpos) between v1
		 and v2 (measured between 0 and (1<<8)-1) */
	      int subpos = data->pos & ((1<<8) - 1); /* or: data->pos % 256 */
	      Sint16 v = (v1 * ((1<<8) - subpos) + v2 * subpos) >> 8;
	      *(pstream++) = v;
	      
	      data->pos += data->shift;
	      if (data->pos >= data->pos_end)
		data->pos = 0;
	    }
	}
      else if (hw_channels == 2)
	{
	  /* Signed 16bit stereo */
	  Sint16 *buf = (Sint16*) data->cvt.buf;
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
	      if (data->pos < data->pos_lastframe)
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
	      if (data->pos >= data->pos_end)
		data->pos = 0;
	    }
	}
      break;
    }
}

int main(int argc, char *argv[])
{
  if (argc != 2)
    {
      printf("Usage: %s file.wav\n", argv[0]);
      exit(1);
    }
  
  if (SDL_Init(SDL_INIT_AUDIO) == -1)
    {
      printf("SDL_Init: %s\n", SDL_GetError());
      return 0;
    }
  
  /* MIX_DEFAULT_FREQUENCY is ~22kHz are considered a good default,
     44kHz is considered too CPU-intensive on older computers */
  /* MIX_DEFAULT_FORMAT is 16bit adapted to current architecture
     (little/big endian) */
  /* MIX_DEFAULT_CHANNELS is 2 => stereo, allowing panning effects */
  /* 1024 (chunk on which effects are applied) seems a good default,
     4096 is considered too big for SFX */
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16SYS/* MIX_DEFAULT_FORMAT */, 2, 1024) == -1)
    {
      printf("Mix_OpenAudio: %s", Mix_GetError());
      return 0;
    }

  /* Allocate channels to play effects to */
  Mix_AllocateChannels(MIX_CHANNELS);

  /* Done with initialization */
  /* Avoid calling SDL_PauseAudio when using SDL_Mixer */
  /* SDL_PauseAudio(0); */
  
  
  /* Dump audio info */
  int numtimesopened; /* hw_channels, hw_frequency -> top-level */ 
  Uint16 hw_format;
  numtimesopened = Mix_QuerySpec(&hw_freq, &hw_format, &hw_channels);
  if (!numtimesopened)
    {
      printf("Mix_QuerySpec: %s\n", Mix_GetError());
      exit(1);
    }
  else
    {
      printf("Audio info: frequency=%dHz\tformat=%s\tchannels=%d\topened=%d times\n",
	     hw_freq, format2string(hw_format), hw_channels, numtimesopened);
    }




  SDL_AudioSpec wav_spec;
  Uint8 *wav_buf;
  Uint32 wav_len;
  if (SDL_LoadWAV(argv[1], &wav_spec, &wav_buf, &wav_len) == NULL)
    {
      fprintf(stderr, "Could not open test.wav: %s\n", SDL_GetError());
      /* free(...) */
      exit(1);
    }
  printf("WAV   info: frequency=%dHz\tformat=%s\tchannels=%d\tlength=%d bytes\n",
	 wav_spec.freq, format2string(wav_spec.format), wav_spec.channels, wav_len);


  /* Converting some WAV data to hardware format - except for sample
     rate, which we will do better after that. */
  /* Testing with a sine.wav, I saw that stereo->mono (2->1 channels)
     conversion works quite bad - maybe we should it ourselves
     too. AFAICS this case is not handled by SDL_audiocvt.c. */
  SDL_AudioCVT  wav_cvt;
  int ret;
  
  /* Build AudioCVT */
  ret = SDL_BuildAudioCVT(&wav_cvt,
			  wav_spec.format, wav_spec.channels, wav_spec.freq,
			  hw_format, hw_channels, wav_spec.freq);
  
  /* Check that the convert was built */
  if (ret == -1) {
    fprintf(stderr, "Couldn't build converter: %s\n", SDL_GetError());
    Mix_CloseAudio();
    SDL_FreeWAV(wav_buf);
    exit(1);
  }

  /* Setup for conversion */
  wav_cvt.buf = (Uint8 *)malloc(wav_len*wav_cvt.len_mult);
  wav_cvt.len = wav_len;
  memcpy(wav_cvt.buf, wav_buf, wav_len);
  
  /* We can delete to original WAV data now */
  SDL_FreeWAV(wav_buf);

  
  /* And now we're ready to convert */
  ret = SDL_ConvertAudio(&wav_cvt);
  if (ret == -1) {
    fprintf(stderr, "Couldn't convert audiox: %s\n", SDL_GetError());
    Mix_CloseAudio();
    SDL_FreeWAV(wav_buf);
    exit(1);
  }
  
  /* Work-around: if no conversion is needed, format is not specified: */
  if (wav_cvt.needed == 0)
    wav_cvt.dst_format = wav_spec.format;

  /* Converted audio is now in wav_cvt.buf */
  printf("CVT   info: frequency=?Hz\tformat=%s\tchannels=??\tlength=%d bytes\n",
	 format2string(wav_cvt.dst_format), wav_cvt.len_cvt);

  /** Shift:
      hw_adjust = wave_freq / hw_freq;
      sample_rate_adjust = play_freq / wave_freq
      shift = hw_adjust * sample_rate_adjust = wave_freq/hw_freq * play_freq/wave_freq
      <<8 because we're using 1/256th units (poor man's double)
  */
  int play_freq = 22050;
  int shift = ((int)round((double)play_freq / hw_freq * (1<<8)));
  printf("shift=%d (%d*64)\n", shift, shift>>8);

  /* Last byte in hw_format is the number of bits per sample: */
  int wav_bytesPerSample = (wav_spec.format & 0xFF) / 8;
  int pos_end = ((wav_len / wav_bytesPerSample) / wav_spec.channels) << 8; /* number of frames */
  int pos_lastframe = (((wav_len / wav_bytesPerSample) / wav_spec.channels) - 1) << 8;

  /** Create a junk wav of the right size **/
  int fake_len = wav_cvt.len_cvt * ((double)hw_freq / play_freq);

  /* Give a bigger buffer to Mix_PlayChannel if needed */
  char* fake_buf = malloc(fake_len > wav_len ? fake_len : wav_len);
  Mix_Chunk *chunk = Mix_QuickLoad_RAW(fake_buf, fake_len);
  printf("fake_len=%d bytes, pos_end=%d\n", fake_len, pos_end);
  
  int channel = Mix_PlayChannel(-1, chunk, -1);
  struct data data = {chunk,
		      0,
		      shift,
		      pos_lastframe,
		      pos_end,
		      wav_spec,
		      wav_cvt,
		      play_freq
  };
  Mix_RegisterEffect(channel, callback_effect, NULL, &data);

  sleep(2);

  free(wav_cvt.buf);
  Mix_CloseAudio();
  SDL_Quit();
  return 0;
}

/**
 * Local Variables:
 * compile-command: "gcc sdl_freq.c `sdl-config --cflags --libs` -lSDL_mixer"
 * End:
 */
