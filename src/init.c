/**
 * System initialization, common to FreeDink and FreeDinkEdit

 * Copyright (C) 2007, 2008, 2009  Sylvain Beucler

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <unistd.h> /* chdir */
#include <xalloc.h>

#include <locale.h>
#include "gettext.h"
#define _(String) gettext (String)

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_framerate.h"
#include "binreloc.h"
#include "progname.h"
#include "game_engine.h"
#include "dinkini.h"
#include "dinkvar.h"
#include "gfx.h"
#include "gfx_fonts.h"
#include "gfx_palette.h"
#include "gfx_sprites.h"
#include "gfx_tiles.h"
#include "fastfile.h"
#include "sfx.h"
#include "bgm.h"
#include "input.h"
#include "io_util.h"
#include "paths.h"
#include "log.h"
#include "init.h"
#include "msgbox.h"

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _PSP
/* The following is necessary if you're running in PRX format, whose
   default heap size is only 64kB... */
#include <pspmoduleinfo.h>
PSP_HEAP_SIZE_MAX();
#endif

static int g_b_no_write_ini = 0; // -noini passed to command line?
static char* init_error_msg = NULL;

void init_set_error_msg(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vasprintf(&init_error_msg, fmt, ap);
  va_end(ap);
}

/**
 * Prints the version on the standard ouput. Based on the homonymous
 * function from ratpoison
 */
void
print_version ()
{
  printf("%s %s\n", PACKAGE_NAME, VERSION);
  printf("FreeDink is free software, and you are welcome to redistribute it\n");
  printf("under certain conditions; see the GNU GPL for details.\n");
  printf("http://gnu.org/licenses/gpl.html\n");
  printf("There is NO WARRANTY, to the extent permitted by law.\n");
  exit(EXIT_SUCCESS);
}


/**
 * Prints the version on the standard ouput. Based on the homonymous
 * function from ratpoison
 */
void
print_help (int argc, char *argv[])
{
  printf(_("Usage: %s [OPTIONS]...\n"), argv[0]);
  if (!dinkedit)
    printf(_("Starts the Dink Smallwood game or one of its D-Mods."));
  else
    printf(_("Edit the Dink Smallwood game or one of its D-Mods."));
  printf("\n");
  /* TODO: Display the default configuration here */
  printf(_("  -h, --help            Display this help screen\n"));
  printf(_("  -v, --version         Display the version\n"));
  printf("\n");
  printf(_("  -g, --game <dir>      Specify a DMod directory\n"));
  printf(_("  -r, --refdir <dir>    Specify base directory for dink/graphics, D-Mods, etc.\n"));
  printf("\n");
  printf(_("  -d, --debug           Explain what is being done\n"));
  printf(_("  -i, --noini           Do not attempt to write dinksmallwood.ini\n"));
  printf(_("  -j, --nojoy           Do not attempt to use joystick\n"));
  printf(_("  -s, --nosound         Do not play sound\n"));
  printf(_("  -t, --truecolor       Allow more colours (for recent D-Mod graphics)\n"));
  printf(_("  -w, --window          Use windowed mode instead of screen mode\n"));
  printf(_("  -7, --v1.07           Enable v1.07 compatibility mode\n"));
  printf("\n");

  /* printf ("Type 'info freedink' for more information\n"); */

  /* TRANSLATORS: The placeholder indicates the bug-reporting address
     for this package.  Please add _another line_ saying "Report
     translation bugs to <...>\n" with the address for translation
     bugs (typically your translation team's web or email
     address).  */
  printf(_("Report bugs to %s.\n"), PACKAGE_BUGREPORT);

  exit(EXIT_SUCCESS);
}



/*
* finiObjects
*
* finished with all objects we use; release them
*/
void finiObjects()
{
  if (g_b_kill_app != 0)
    return;
  g_b_kill_app = 1;
  
  if (last_saved_game > 0)
    {
      log_info("Modifying saved game.");
      
      if (!add_time_to_saved_game(last_saved_game))
	log_error("Error modifying saved game.");
      last_saved_game = 0;
    }
	
  log_path(/*playing=*/0);
  
  if (sound_on)
    {
      //lets kill the cdaudio too
      bgm_quit();
      QuitSound();
    }
	
  kill_all_scripts_for_real();
  FastFileFini();

  dinkc_quit();
  dinkini_quit();

  game_quit();
  
  input_quit();
  
  gfx_quit();
  
  //SDL_QuitSubSystem(SDL_INIT_EVENTTHREAD);	
  SDL_QuitSubSystem(SDL_INIT_TIMER);
  
  SDL_Quit();

  if (init_error_msg != NULL)
    free(init_error_msg);
  
  paths_quit();
}

/**
 * This function is called if the initialization function fails
 */
int initFail(char *message)
{
  msgbox_init_error(message);
  return -1; /* used when "return initFail(...);" */
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
  int debug_p = 0;

  /* Options '-debug', '-game', '-noini', '-nojoy', '-nosound' and
     '-window' (with one dash '-' only) are required to maintain
     backward compatibility with the original game */
  struct option long_options[] = 
    {
      {"debug",     no_argument,       NULL, 'd'},
      {"refdir",    required_argument, NULL, 'r'},
      {"game",      required_argument, NULL, 'g'},
      {"help",      no_argument,       NULL, 'h'},
      {"noini",     no_argument,       NULL, 'i'},
      {"nojoy",     no_argument,       NULL, 'j'},
      {"nosound",   no_argument,       NULL, 's'},
      {"version",   no_argument,       NULL, 'v'},
      {"window",    no_argument,       NULL, 'w'},
      {"v1.07",     no_argument,       NULL, '7'},
      {"truecolor", no_argument,       NULL, 't'},
      {"nomovie"  , no_argument,       NULL, ','},
      {0, 0, 0, 0}
    };
  
  char short_options[] = "dr:g:hijsvw7t";

  /* Loop through each argument */
  while ((c = getopt_long_only (argc, argv, short_options, long_options, NULL)) != EOF)
    {
      switch (c) {
      case 'd':
	debug_p = 1;
        /* Enable early debugging, before we can locate DEBUG.txt */
        log_set_priority(LOG_PRIORITY_DEBUG);
	break;
      case 'r':
	refdir_opt = strdup(optarg);
	break;
      case 'g':
	dmoddir_opt = strdup(optarg);
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
	break;
      case '7':
	dversion = 107;
	break;
      case 't':
	truecolor = 1;
	break;
      case ',':
        printf(_("Note: -nomovie is accepted for compatiblity, but has no effect.\n"));
	break;
      default:
	exit(EXIT_FAILURE);
      }
    }
  
  if (optind < argc) {
    fprintf(stderr, "Invalid additional argument: ");
    while (optind < argc)
      fprintf(stderr, "%s ", argv[optind++]);
    printf(" (did you forget '--game'?)\n");
    exit(EXIT_FAILURE);
  }

  
  paths_init(argv[0], refdir_opt, dmoddir_opt);

  free(refdir_opt);
  free(dmoddir_opt);

  if (debug_p == 1)
    {
      /* Remove DEBUG.TXT when starting Dink (but not when toggling debug) */
      char* fullpath = paths_dmodfile("DEBUG.TXT");
      remove(fullpath);
      free(fullpath);

      log_debug_on();
    }

  log_info("Game directory is '%s'.", paths_getdmoddir());
  return 1;
}


/**
 * What to do if running out of memory
 */
void xalloc_die (void) {
  fprintf(stderr, "Memory exhausted!");
  finiObjects();
  abort();
}


/* The goal is to replace freedink and freedinkedit's doInit() by a
   common init procedure. This procedure will also initialize each
   subsystem as needed (eg InitSound) */
int init(int argc, char *argv[], char* splash_path)
{
  /** i18n **/
  /* Only using LC_MESSAGES because LC_CTYPE (part of LC_ALL) may
     bring locale-specific behavior in the DinkC parsers. If that's a
     problem we may use some gnulib modules
     (cf. (gettext.info.gz)Triggering) */
  /* Ex. with scanf("%f"...):
     LANG=C            1.1 -> 1.100000
     LANG=C            1,1 -> 1.000000
     LANG=fr_FR.UTF-8  1.1 -> 1,000000
     LANG=fr_FR.UTF-8  1,1 -> 1,100000 */
  /* setlocale (LC_ALL, ""); */
  setlocale(LC_MESSAGES, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bindtextdomain(PACKAGE "-gnulib", LOCALEDIR);
  textdomain(PACKAGE);
  
  /* SDL can display messages in either ASCII or UTF-8. Thus we need
     gettext to output translations in UTF-8. */
  /* That's a problem for console messages in locales that are not
     UTF-8-encoded. If this is really a problem, we'll have to perform
     some character conversion directly. We can't create a separate
     message catalog for those, because several console messages are
     also displayed on the SDL screen and need a separate conversion
     anyway. */
  bind_textdomain_codeset(PACKAGE, "UTF-8");


  if (!check_arg(argc, argv))
    return -1;

  /* Same for this D-Mod's .mo (after options are parsed) */
  char* dmod_localedir = paths_dmodfile("l10n");
  bindtextdomain(paths_getdmodname(), dmod_localedir);
  bind_textdomain_codeset(paths_getdmodname(), "UTF-8");
  free(dmod_localedir);


  /* SDL */
  /* Init timer subsystem */
  if (SDL_Init(SDL_INIT_TIMER) == -1) {
    init_set_error_msg("Timer initialization error: %s\n", SDL_GetError());
    return initFail(init_error_msg);
  }

  /* Quits in case we couldn't do it properly first (i.e. attempt to
     avoid stucking the user in 640x480 when crashing) */
  atexit(finiObjects);

  /* GFX */
  if (gfx_init(windowed ? GFX_WINDOWED : GFX_FULLSCREEN,
	       splash_path) < 0)
    return initFail(init_error_msg);

  /* Joystick */
  input_init();

  /* Engine */
  /* Start with this initialization as it resets structures that are
     filled in other subsystems initialization */
  game_init();

  dinkini_init();
  dinkc_init();

  /* SFX & BGM */
  if (sound_on) 
    {
      log_info("Initting sound");
      if (InitSound() < 0)
	sound_on = 0;
      else
	sound_on = 1;

      if (sound_on)
	bgm_init();
    }
  if (cd_inserted)
    PlayCD(7);


  SDL_initFramerate(&framerate_manager);
  /* The official v1.08 .exe runs 50-60 FPS in practice, despite the
     documented intent of running 83 FPS (or 12ms delay). */
  /* SDL_setFramerate(manager, 83); */
  SDL_setFramerate(&framerate_manager, FPS);

  //dinks normal walk
  log_info("Loading batch...");
  load_batch();
  log_info(" done!");
  
  log_info("Loading hard...");
  load_hard();
  log_info(" done!");

  /* We'll handle those events manually */
  SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
  SDL_EventState(SDL_VIDEOEXPOSE, SDL_IGNORE);
  SDL_EventState(SDL_VIDEORESIZE, SDL_IGNORE);
  SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
  SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
  SDL_EventState(SDL_KEYUP, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
  SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
  /* We still process through a SDL_PollEvent() loop: */
  /* - SDL_QUIT: quit on window close and Ctrl+C */
  /* - SDL_MOUSEBUTTONDOWN: don't miss quick clicks */
  /* - Joystick: apparently we need to keep them, otherwise joystick
       doesn't work at all */
  /* - SDL_KEYDOWN: we want the keydown events for text input
       (show_console and editor input dialog) */

  return 0;
}



/**
 * Save where Dink is installed in a .ini file, read by third-party
 * applications like the DinkFrontEnd. Also notify whether Dink is
 * running or not.
 */
void log_path(/*bool*/int playing)
{
  if (g_b_no_write_ini)
    return; //fix problem with NT security if -noini is set
  /* TODO: saves it in the user home instead. Think about where to
     cleanly store additional DMods. */

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
  char windir[100];
  char inifile[256];
  GetWindowsDirectory(windir, 256);
  sprintf(inifile, "%s\\dinksmallwood.ini", windir);

  unlink(inifile);

  add_text("[Dink Smallwood Directory Information for the CD to read]\r\n", inifile);
  add_text((char *)paths_getexedir(), inifile);
  add_text("\r\n", inifile);
  if (playing)
    add_text("TRUE\r\n", inifile);
  else
    add_text("FALSE\r\n", inifile);
#endif
}
