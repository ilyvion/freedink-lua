/**
 * Background music (currently .midi's and audio CDs)

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005  Sylvain Beucler

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* CD functions */
#include "SDL.h"
/* MIDI functions */
#include "SDL_mixer.h"

#include <string.h>

#include "dinkvar.h"
#include "bgm.h"
#include "io_util.h"
#include "paths.h"
#include "log.h"

/* CD-ROM handle */
static SDL_CD *cdrom = NULL;


/* Current background music (not cd) */
static Mix_Music *music_data = NULL;
static char* last_midi = NULL;
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
  return (SDL_CDStatus(cdrom) == CD_PLAYING);
}

int
killcd()
{
  last_cd_track = 0;	
  return SDL_CDStop(cdrom);
} 

/**
 * Play a CD track
 */
int
PlayCD(int cd_track)
{
  Mix_HaltMusic();

  last_cd_track = cd_track;

  /* Play track #cd_track */
  if (CD_INDRIVE(SDL_CDStatus(cdrom)))
    return SDL_CDPlayTracks(cdrom, cd_track - 1, 0, 1, 0);
  else
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
 * Clean-up music when it's finished or manually halted
 */
static void callback_HookMusicFinished()
{
  if (music_data != NULL)
    Mix_FreeMusic (music_data);
}

/**
 * Thing to play the midi
 */
int PlayMidi(char *sFileName)
{
  char midi_filename[256];
  char *fullpath = NULL;
  
  /* no midi stuff right now */
  if (sound_on == /*false*/0)
    return 1;
  
  /* Do nothing if the same midi is already playing */
  /* TODO: Does not differentiate midi and ./midi, qsf\\midi and
     qsf/midi... Ok, midi is supposed to be just a number, but..*/
  if (last_midi != NULL && compare(last_midi, sFileName)
      && something_playing())
    {
      Msg("I think %s is already playing, I should skip it it...",
	  sFileName);
      return 0;
    }

  /* Try to load the midi in the DMod or the main game */
  sprintf(midi_filename, "sound/%s", sFileName);
  fullpath = paths_dmodfile(midi_filename);
  if (!exist(fullpath))
    {
      free(fullpath);
      fullpath = paths_fallbackfile(midi_filename);
      if (!exist(fullpath))
	{
	  free(fullpath);
  	  Msg("Error playing midi %s, doesn't exist in any dir.", sFileName);
	  return 0;
	}  
    }

  /* Save the midi currently playing */
  if (last_midi != NULL)
    free(last_midi);
  last_midi = strdup(sFileName);

  /* Stop CD track */
  Msg("Killing cd...");
  killcd();


  /* Stop whatever is playing before we play something else. */
  Mix_HaltMusic ();
  
  /* Load the file */
  if ((music_data = Mix_LoadMUS(fullpath)) == NULL)
    {
      Msg("Unable to play '%s': %s", fullpath, Mix_GetError());
      free(fullpath);
      return 0;
    }

  /* Play it */
  Mix_HookMusicFinished(callback_HookMusicFinished);
  Mix_PlayMusic (music_data, 1);

  free(fullpath);
  return 1;
}



/**
 * Pause midi file if we're not already paused
 */
/* TODO: should be used when player hits 'n' or alt+'n' - but I never
   got it to work in the original game */
int PauseMidi()
{
  Mix_PauseMusic();
  return 1;
}
	
/**
 * Resumes playing of a midi file
 */
/* TODO: should be used when player hits 'b' or alt+'b' - but I never
   got it to work in the original game */
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
// DinkC binding: stopmidi()
int StopMidi()
{
  Mix_HaltMusic(); // return always 0
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

/**
 * Initialize BackGround Music (currently, only CD-ROM is concerned,
 * MIDI init is done with SDL_INIT_AUDIO in sfx.c).
 */
void bgm_init(void)
{
  if (SDL_Init(SDL_INIT_CDROM) == -1)
    {
      Msg("SDL_Init: %s\n", SDL_GetError());
      return;
    }
  
  /* Check for CD drives */
  if(!SDL_CDNumDrives()){
    /* None found */
    Msg (("No CDROM devices available\n"));
  }
  
  /* Open the default drive */
  cdrom = SDL_CDOpen(0);
  
  /* Did if open? Check if cdrom is NULL */
  if (cdrom == NULL)
    {
      Msg("Couldn't open drive: %s\n", SDL_GetError());
      return;
    }
  
  if (CD_INDRIVE(SDL_CDStatus(cdrom))) {
    /* TODO: do some test about the presence of audio tracks in the
       CD - though fortunately SDL_Mixer does not read data track */
    cd_inserted = /*true*/1;
  } 
  
  /* This newly opened CD-ROM becomes the default CD used when other
     CD functions are passed a NULL CD-ROM handle. */
}

void bgm_quit(void)
{
  Mix_HaltMusic();
  Msg("Shutting down CD stuff.");
  killcd();
  if (last_midi != NULL)
    free(last_midi);
  if (cdrom != NULL)
    SDL_CDClose(cdrom);
  SDL_QuitSubSystem(SDL_INIT_CDROM);
}
