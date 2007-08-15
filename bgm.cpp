/**
 * Background music (currently .midi's and audio CDs)

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
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

/* CD functions */
#include "SDL.h"
/* MIDI functions */
#include "SDL_mixer.h"

#include "dinkvar.h"
#include "bgm.h"
#include "io_util.h"

/* Current background music (not cd) */
static Mix_Music *music_data = NULL;
static char last_midi[20];
int last_cd_track = 0;

/*
 * Audio CD Functions
 */

/**
 * Return whether an audio track currently playing
 */
// TODO: test
int
cdplaying(void)
{
  return (SDL_CDStatus(NULL) == CD_PLAYING);
}

int
killcd()
{
  last_cd_track = 0;	
  return SDL_CDStop(NULL);
} 

/**
 * Play a CD track
 */
int
PlayCD(int cd_track)
{
  last_cd_track = cd_track;
  
  /* TODO: stopmidi()? */
  playMIDIFile("");

  /* Play track #cd_track */
  if(CD_INDRIVE(SDL_CDStatus(NULL)))
    return SDL_CDPlayTracks(NULL, cd_track - 1, 0, 1, 0);

  return -1;
} 



/*
 * MIDI functions
 */

/**
 * Returns whether the background music is currently playing
 */
int something_playing(void)
{
  return Mix_PlayingMusic();
}

/**
 * Thing to play the midi
 */
/* TODO: midi_filename can be empty, check that */
int
playMIDIFile(char *midi_filename)
{ 
  char tmp_filename[PATH_MAX];
  /* Stop whatever is playing before we play something else. */
  Mix_HaltMusic ();
  if (music_data != NULL)
    Mix_FreeMusic (music_data);
  
  /* Load the file */
  music_data = Mix_LoadMUS(ciconvertbuf(midi_filename, tmp_filename));
  if (music_data == NULL)
    {
      Msg (("Unable to play '%s': %s", midi_filename, Mix_GetError()));
      return 0;
    }

  /* Play it */
  Mix_PlayMusic (music_data, 1);
  return 0;
} 

int PlayMidi(char *sFileName)
{
  char midi_filename[256];

  /* no midi stuff right now */
  if (sound_on == false)
    return 1;
  
  /* Do nothing if the same midi is already playing */
  /* TODO: Does not differentiate midi and ./midi, qsf\\midi and
     qsf/midi... Ok, midi is supposed to be just a number, but..*/
  if (compare(last_midi, sFileName)) 
    {
      if (something_playing())
	{
	  Msg("I think %s is already playing, I should skip it it...",
	      sFileName);
	  return 0;
	}  
    }

  /* Try to load the midi in the DMod or the main game */
  sprintf(midi_filename, "sound/%s", sFileName);
  if (!exist(midi_filename))
    {
      sprintf(midi_filename, "../dink/sound/%s",sFileName);
      if (!exist(midi_filename))
	{
	  Msg("Error playing midi %s, doesn't exist in any dir.", sFileName);
	  return 0;
	}  
    }

  /* Save the midi currently playing */
  strcpy(last_midi, sFileName);

  /* Stop CD track */
  Msg("Killing cd...");
  killcd();

  playMIDIFile(midi_filename);
  return 1;
}



/**
 * Pause midi file if we're not already paused
 */
// TODO: test when this is used, and whether is works
int PauseMidi()
{
  Mix_PauseMusic();
  return 1;
}
	
/**
 * Resumes playing of a midi file
 */
// TODO: test when this is used, and whether is works
int
ResumeMidi()
{
  Mix_ResumeMusic();
  return 1;
}


/**
 * Stops a midi file playing	
 */
// TODO: rename *Midi to *BGM (BackGround Music) - we may support
// background .ogg's in the future
int StopMidi()
{
  Mix_HaltMusic();
  return 1;
}

/**
 * Replays a midi file (as of 2005-09-24, only works MOD, OGG, MP3 and
 * native MIDI). Restart from the beginning of the file without
 * stopping.
 */
// UNUSED
int ReplayMidi()
{
  Mix_RewindMusic();
  return 1;
}

/**
 * When entering a screen, play the appropriate MIDI
 */
void check_midi(void)
{
  // TODO: use a better constant (like max_file_path)
  char midi_filename[20];
	
  if ((!midi_active) || (map.music[*pmap] == 0))
    return;

  /* There is music information associated with this screen */
  if (map.music[*pmap] == -1)
    /* Kill cd music */
    {
      Msg("Stopped cd");
      killcd();
    }
  else if (map.music[*pmap] > 1000)
    /* Try to play a CD track */
    {
      int cd_track = map.music[*pmap] - 1000;
      if (cd_inserted)
	/* Play track */
	{
	  /* Do nothing if already playing the right track */
	  if (cd_track == last_cd_track
	      && cdplaying())
	    return;
	 
	  if (PlayCD(cd_track) >= 0)
	    return; /* Playing fine */
	}
    }

  /* Just play the specified MIDI */
  sprintf(midi_filename, "%d.mid", map.music[*pmap]);
  PlayMidi(midi_filename);
}
