/**
 * Stream a sound to SDL_mixer by abusing Mix_RegisterEffect

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

#include "SDL.h"
#include "SDL_mixer.h"
#include <math.h>
#include <stdbool.h>

int global_halt = 0;

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
 * Hold the state of the 'effect'
 */
struct sine_state
{
  int x;
  int loop;
};

/**
 * Generate a A-440Hz sine, starting at x coordinate 'udata->x' and lasting
 * 'len'/2 frames
 */
void gen_sine(int chan, void *stream, int len, void *udata)
{
  printf("gen_sine(%d)\n", len);
  struct sine_state* data = (struct sine_state*)udata;
  Sint16* pstream = (Sint16*) stream;

  int max_vol = 1<<(2*8-1) - 1; // 127
  int vol = max_vol * .75;

  for (int i = 0; i < len/2/2; i++)
    {
      /* Fill 'stream' */
      Sint16 y = sin(data->x*6.28*440/44100) * vol;
      *pstream = y;
      pstream++;
      *pstream = y; /* stereo */
      pstream++;
      data->x++;
    }

  data->loop++;
  if (data->loop == 10)
    {
      //Mix_HaltChannel(1);
      global_halt = 1;
    }
}
void gen_sine_cleanup(int chan, void *udata)
{
  free(udata);
}

/**
 * Free channel resources if any
 */
void channel_cleanup(int channel)
{
}

/**
 * Work-around a streamed chunk support in SDL_mixer, by running an
 * effect over a small chunk ran in loop
 */
int main(void)
{
  /* Init */
  SDL_Init(SDL_INIT_AUDIO);
  Mix_OpenAudio(44100, AUDIO_S16LSB,
		MIX_DEFAULT_CHANNELS, 4096 /* 4096 */);
  Mix_AllocateChannels(1);

  /* Dump audio info */
  {
    int hw_freq, hw_channels;
    Uint16 hw_format;
    int numtimesopened;
    numtimesopened = Mix_QuerySpec(&hw_freq, &hw_format, &hw_channels);
    if (!numtimesopened)
      printf("Mix_QuerySpec: %s\n", Mix_GetError());
    else
      printf("Audio hardware info: frequency=%dHz\tformat=%s\tchannels=%d\topened=%d times\n",
	     hw_freq, format2string(hw_format), hw_channels, numtimesopened);
  }

  char buf[409600];
  Mix_Chunk *chunk = Mix_QuickLoad_RAW(buf, 409600);
  if (chunk == NULL)
    printf("Mix_QuickLoad_RAW: %s\n", Mix_GetError());

  
  /** Play a A-440hz pure sound **/
  Mix_Pause(0);
  int channel = Mix_PlayChannel(0, chunk, -1);
  printf("Using channel %d\n", channel);/* error */
  if (channel < 0)
    printf("Error %d\n", channel);/* error */

  /* Register sound generator */
  struct sine_state *data = calloc(1, sizeof(struct sine_state));
  data->x = 0;
  Mix_RegisterEffect(channel, gen_sine, gen_sine_cleanup, data);
  Mix_ChannelFinished(channel_cleanup);

  Mix_Resume(channel);

  /* Play chunk with looping (infinite play) */
  while (global_halt == 0)
    {
      SDL_Delay(100);
    }
}


/**
 * Local Variables:
 * compile-command: "gcc test.c -std=c99 `sdl-config --cflags --libs` -lSDL_mixer"
 * End:
 */
