/**
 * System initialization, common to FreeDink and FreeDinkEdit

 * Copyright (C) 2007, 2008  Sylvain Beucler

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <unistd.h> /* chdir */
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_framerate.h"
#include "binreloc.h"
#include "progname.h"
/* Msg */
#include "dinkvar.h"
#include "gfx.h"
#include "gfx_fonts.h"
#include "gfx_tiles.h"
#include "gfx_sprites.h"
#include "fastfile.h"
#include "sfx.h"
#include "bgm.h"
#include "input.h"
#include "io_util.h"
#include "paths.h"
#include "log.h"
#include "init.h"
#include "msgbox.h"

static char* init_error_msg = NULL;

void init_set_error_msg(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vasprintf(&init_error_msg, fmt, ap);
}

/**
 * Prints the version on the standard ouput. Based on the homonymous
 * function from ratpoison
 */
void
print_version ()
{
  printf ("%s %s\n", PACKAGE_NAME, VERSION);
  printf ("Copyright (C) 2007 by contributors\n");
  printf ("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
  printf ("This is free software: you are free to change and redistribute it.\n");
  printf ("There is NO WARRANTY, to the extent permitted by law.\n");
  exit (EXIT_SUCCESS);
}


/**
 * Prints the version on the standard ouput. Based on the homonymous
 * function from ratpoison
 */
void
print_help (int argc, char *argv[])
{
  printf("Usage: %s [OPTIONS]...\n", argv[0]);
  printf("\n");
  printf("TODO                  Display the default configuration here\n");
  printf("-h, --help            Display this help screen\n");
  printf("-v, --version         Display the version\n");
  printf("\n");
  printf("-g, --game <dir>      Specify a DMod directory\n");
  printf("-r, --refdir <dir>    Specify base directory for dink/graphics, D-Mods, etc.\n");
  printf("\n");
  printf("-d, --debug           Explain what is being done\n");
  printf("-i, --noini           Do not attempt to write dinksmallwood.ini\n");
  printf("-j, --nojoy           Do not attempt to use joystick\n");
  printf("-s, --nosound         Do not play sound\n");
  printf("-w, --window          Use windowed mode instead of screen mode\n");
  printf("\n");

  /* printf ("Type 'info freedink' for more information\n"); */
  printf("Report bugs to %s.\n", PACKAGE_BUGREPORT);

  exit(EXIT_SUCCESS);
}



/*
* finiObjects
*
* finished with all objects we use; release them
*/
void finiObjects()
{
	//wDeviceID = mciGetDeviceID("MCI_ALL_DEVICE_ID"); 
	
	if (last_saved_game > 0)
	{
		Msg("Modifying saved game.");
		
		if (!add_time_to_saved_game(last_saved_game))
			Msg("Error modifying saved game.");
	}
	
	log_path(/*false*/0);

	gfx_quit();

	if (sound_on)
	{
	//lets kill the cdaudio too
/* 	if (mciSendString("close all", NULL, 0, NULL) != 0) */
/* 	{ */
/* 		Msg("Couldn't close all MCI events.."); */
/* 		//	return(FALSE); */
/* 	} */
	  bgm_quit();
	  QuitSound();
	}
	
	kill_all_scripts_for_real();
	FastFileFini();

	g_b_kill_app = 1;
/* 	ShowWindow(hWndMain, SW_HIDE); */
/* 	SendMessage(hWndMain, WM_IMDONE, 0,0); */
	//PostQuitMessage(0);

	input_quit();

	SDL_QuitSubSystem(SDL_INIT_EVENTTHREAD);	
	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	SDL_QuitSubSystem(SDL_INIT_TIMER);

	paths_quit();
	
	if (init_error_msg != NULL)
	  free(init_error_msg);
}

/**
 * This function is called if the initialization function fails
 */
int initFail(char *message)
{
  msgbox_init_error(message);
  finiObjects();
  return 0; /* used when "return initFail(...);" */
}


/**
 * Check the command line arguments and initialize the required global
 * variables
 */
static int check_arg(int argc, char *argv[])
{
  int c;
  char *refdir_opt = NULL;
  char *dmoddir_opt = NULL;

  /* Options '-debug', '-game', '-noini', '-nojoy', '-nosound' and
     '-window' (with one dash '-' only) are required to maintain
     backward compatibility with the original game */
  struct option long_options[] = 
    {
      {"debug",   no_argument,       NULL, 'd'},
      {"refdir",  required_argument, NULL, 'r'},
      {"game",    required_argument, NULL, 'g'},
      {"help",    no_argument,       NULL, 'h'},
      {"noini",   no_argument,       NULL, 'i'},
      {"nojoy",   no_argument,       NULL, 'j'},
      {"nosound", no_argument,       NULL, 's'},
      {"version", no_argument,       NULL, 'v'},
      {"window",  no_argument,       NULL, 'w'},
      {0, 0, 0, 0}
    };
  
  char short_options[] = "dr:g:hijsvw";

  /* Loop through each argument */
  while ((c = getopt_long_only (argc, argv, short_options, long_options, NULL)) != EOF)
    {
      switch (c) {
      case 'd':
	  debug_mode = 1;
	  /* TODO: use global path */
	  remove("dink/debug.txt");
	  break;
      case 'r':
	refdir_opt = strdup(optarg);
	break;
      case 'g':
	{
	  dmoddir_opt = strdup(optarg);
	  Msg("Working directory %s requested.", dmoddir_opt);
	}
	break;
      case 'h':
	print_help(argc, argv);
	break;
      case 'j':
	joystick = 0;
	break;
      case 'i':
	g_b_no_write_ini = 1;
	break;
      case 's':
	sound_on = 0;
	break;
      case 'v':
	print_version();
	break;
      case 'w':
	  windowed = 1;
	  // Beuc: enabling transition is more fun :)
	  //no_transition = true;
	  break;	
      default:
	exit(EXIT_FAILURE);
      }
    }
  
  paths_init(argv[0], refdir_opt, dmoddir_opt);

  free(refdir_opt);
  free(dmoddir_opt);

  Msg("Game directory is '%s'.", paths_dmoddir());
  return 1;
}



/* The goal is to replace freedink and freedinkedit's doInit() by a
   common init procedure. This procedure will also initialize each
   subsystem as needed (eg InitSound) */
int init(int argc, char *argv[])
{
  if (!check_arg(argc, argv))
    return 0;


  if (sound_on) 
    {
      Msg("Initting sound");
      if (InitSound() < 0)
	sound_on = 0;
      else
	sound_on = 1;

      if (sound_on)
	bgm_init();
    }


  /* SDL */
  /* Init timer subsystem */
  if (SDL_Init(SDL_INIT_TIMER) == -1)
    {
      Msg("Timer initialization error: %s\n", SDL_GetError());
      return 0;
    }


  SDL_initFramerate(&framerate_manager);
  /* The official v1.08 .exe runs 50-60 FPS in practice, despite the
     documented intent of running 83 FPS (or 12ms delay). */
  /* SDL_setFramerate(manager, 83); */
  SDL_setFramerate(&framerate_manager, 60);


  int gfx_init_success = 0;
  if (windowed)
    gfx_init_success = gfx_init(GFX_WINDOWED);
  else
    gfx_init_success = gfx_init(GFX_FULLSCREEN);
  if (gfx_init_success < 0)
    return initFail(init_error_msg);

  /* Mouse */
  /* Center mouse and reset relative positionning */
  SDL_WarpMouse(320, 240);
  SDL_PumpEvents();
  SDL_GetRelativeMouseState(NULL, NULL);


  /* We'll handle those events manually */
  SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
  SDL_EventState(SDL_VIDEORESIZE, SDL_IGNORE);
  SDL_EventState(SDL_VIDEOEXPOSE, SDL_IGNORE);
  SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
  SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
  SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
  SDL_EventState(SDL_KEYUP, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
  /* We still process through a SDL_PollEvent() loop: */
  /* - SDL_QUIT: quit on window close and Ctrl+C */
  /* - SDL_MOUSEBUTTONDOWN: don't miss quick clicks */
  /* - Joystick: apparently we need to keep them, otherwise joystick
       doesn't work at all */


  /* SDL_MouseMotionEvent: If the cursor is hidden (SDL_ShowCursor(0))
     and the input is grabbed (SDL_WM_GrabInput(SDL_GRAB_ON)), then
     the mouse will give relative motion events even when the cursor
     reaches the edge of the screen. This is currently only
     implemented on Windows and Linux/Unix-alikes. */
  /* So it's not portable and it blocks Alt+Tab, so let's try
     something else - maybe enable it as a command line option. */
  /* SDL_WM_GrabInput(SDL_GRAB_ON); */


  /* Joystick */
  input_init();


  /* Maybe use SDL_QuitSubSystem instead */
  atexit(SDL_Quit);


  /* Engine */
  /* Clean the game state structure - done by C++ but not
     automatically done by C, and this causes errors. TODO: fix the
     errors properly instead of using this dirty trick. */
  memset(&play, 0, sizeof(play));

  memset(&hmap, 0, sizeof(hmap));
  memset(&pam, 0, sizeof(pam));

  return 1;
}
