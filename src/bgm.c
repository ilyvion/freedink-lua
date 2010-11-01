/**
 * Background music (currently .midi's and audio CDs)

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009  Sylvain Beucler

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

#include <stdlib.h>
#include <string.h>
#include <strings.h> /* strcasecmp */

#include "game_engine.h"
#include "bgm.h"
#include "io_util.h"
#include "str_util.h"
#include "paths.h"
#include "log.h"

/* CD-ROM handle */
#if SDL_VERSION_ATLEAST(1, 3, 0)
static void* cdrom = NULL;
#define SDL_CDStatus(cdrom) -1 /* CD_ERROR */
#define SDL_CDStop(cdrom) 0
#define CD_INDRIVE(i) 0
#define SDL_CDPlayTracks(cdrom, start_track, start_frame, ntracks, nframes) -1
#define SDL_CDClose(cdrom) 0
#else
static SDL_CD *cdrom = NULL;
#endif

/* Current background music (not cd) */
static Mix_Music *music_data = NULL;
static char* last_midi = NULL;
int last_cd_track = 0;
static int loop_midi = 0;

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
#if SDL_VERSION_ATLEAST(1, 3, 0)
  return 0;
#else
  return (SDL_CDStatus(cdrom) == CD_PLAYING);
#endif
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
  if (cdrom == NULL)
    return -1;

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
int PlayMidi(char *midi_filename)
{
  char relpath[256];
  char *fullpath = NULL;
  
  /* no midi stuff right now */
  if (sound_on == /*false*/0)
    return 1;
  
  /* Do nothing if the same midi is already playing */
  /* TODO: Does not differentiate midi and ./midi, qsf\\midi and
     qsf/midi... Ok, midi is supposed to be just a number, but..*/
  if (last_midi != NULL && compare(last_midi, midi_filename)
      && something_playing())
    {
      log_info("I think %s is already playing, I should skip it...", midi_filename);
      return 0;
    }

      
  // Attempt to play .ogg in addition to .mid, if playing a ".*\.mid$"
  char* oggv_filename = NULL;
  int pos = strlen(midi_filename) - strlen(".mid");
  if (strcasecmp(midi_filename + pos, ".mid") == 0)
    {
      oggv_filename = strdup(midi_filename);
      strcpy(oggv_filename + pos, ".ogg");
    }

  /* Try to load the ogg vorbis or midi in the DMod or the main game */
  int exists = 0;
  fullpath = malloc(1);
  if (!exists && oggv_filename != NULL)
    {
      free(fullpath);
      sprintf(relpath, "sound/%s", oggv_filename);
      fullpath = paths_dmodfile(relpath);
      exists = exist(fullpath);
    }
  if (!exists)
    {
      free(fullpath);
      sprintf(relpath, "sound/%s", midi_filename);
      fullpath = paths_dmodfile(relpath);
      exists = exist(fullpath);
    }
  if (!exists && oggv_filename != NULL)
    {
      free(fullpath);
      sprintf(relpath, "sound/%s", oggv_filename);
      fullpath = paths_fallbackfile(relpath);
      exists = exist(fullpath);
    }
  if (!exist(fullpath))
    {
      free(fullpath);
      sprintf(relpath, "sound/%s", midi_filename);
      fullpath = paths_fallbackfile(relpath);
      exists = exist(fullpath);
    }
  free(oggv_filename);

  if (!exist(fullpath))
    {
      free(fullpath);
      log_warn("Error playing midi %s, doesn't exist in any dir.", midi_filename);
      return 0;
    }


  /* Save the midi currently playing */
  if (last_midi != NULL)
    free(last_midi);
  last_midi = strdup(midi_filename);

  /* Stop CD track */
  log_info("Killing cd...");
  killcd();


  /* Stop whatever is playing before we play something else. */
  Mix_HaltMusic ();
  
  /* Load the file */
  if ((music_data = Mix_LoadMUS(fullpath)) == NULL)
    {
      log_warn("Unable to play '%s': %s", fullpath, Mix_GetError());
      free(fullpath);
      return 0;
    }

  /* Play it */
  Mix_HookMusicFinished(callback_HookMusicFinished);
  Mix_PlayMusic (music_data, (loop_midi == 1) ? -1 : 1);

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
      log_info("Stopped cd");
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
      /* If couldn't play the CD track, fallback to midi */
      sprintf(midi_filename, "%d.mid", map.music[*pmap] - 1000);
      PlayMidi(midi_filename);
    }
  else
    {
      /* Just play the specified MIDI */
      sprintf(midi_filename, "%d.mid", map.music[*pmap]);
      PlayMidi(midi_filename);
    }
}

/**
 * Initialize BackGround Music (currently, only CD-ROM is concerned,
 * MIDI init is done with SDL_INIT_AUDIO in sfx.c).
 */
void bgm_init(void)
{
#if SDL_VERSION_ATLEAST(1, 3, 0)
  log_info("No CDROM support (SDL 1.3 dropped it)");
#else
  if (SDL_Init(SDL_INIT_CDROM) == -1)
    {
      log_error("SDL_Init: %s", SDL_GetError());
      return;
    }

  /* Check for CD drives */
  if (!SDL_CDNumDrives()){
    /* None found */
    log_info("No CDROM devices available");
    return;
  }
  
  /* Open the default drive */
  cdrom = SDL_CDOpen(0);
  
  /* Did if open? Check if cdrom is NULL */
  if (cdrom == NULL)
    {
      log_error("Couldn't open drive: %s", SDL_GetError());
      return;
    }
  
  if (CD_INDRIVE(SDL_CDStatus(cdrom)))
    if (cdrom->numtracks == 19)
      /* only enable CD for the original game CD; and well, it makes
	 sense not to try to play the CD anytime the user left a CD in
	 the drive */
      cd_inserted = 1;
  
  /* This newly opened CD-ROM becomes the default CD used when other
     CD functions are passed a NULL CD-ROM handle. */
#endif  
}

void bgm_quit(void)
{
  Mix_HaltMusic();
  log_info("Shutting down CD stuff.");
  killcd();
  if (last_midi != NULL)
    free(last_midi);
  last_midi = NULL;
  if (cdrom != NULL)
    SDL_CDClose(cdrom);
  cdrom = NULL;
#if SDL_VERSION_ATLEAST(1, 3, 0)
#else
  SDL_QuitSubSystem(SDL_INIT_CDROM);
#endif
}



/** DinkC procedures **/
void loopmidi(int arg_loop_midi)
{
  if (arg_loop_midi > 0)
    loop_midi = 1;
  else
    loop_midi = 0;
}
