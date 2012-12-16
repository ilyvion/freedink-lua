/**
 * Compute and store the search paths

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
#include <stdlib.h>

#include "io_util.h"
#include "paths.h"
#include "msgbox.h"
#include "log.h"


#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0401
#include <windows.h>
#include <shlobj.h>
#endif

#include "relocatable.h"
#include "progname.h"
#include "binreloc.h"

#include <string.h> /* strdup */
#include "canonicalize.h" /* canonicalize_file_name */

/* basename */
#include <libgen.h>
#include "dirname.h"

/* mkdir */
#include <sys/stat.h>
#include <sys/types.h>

#include "str_util.h" /* asprintf_append */


static char* defaultpkgdatadir = NULL;
static char* pkgdatadir = NULL;
static char* exedir = NULL;
static char* fallbackdir = NULL;
static char* dmoddir = NULL;
static char* dmodname = NULL;
static char* userappdir = NULL;
static char* exefile = NULL;


void paths_init(char *argv0, char *refdir_opt, char *dmoddir_opt)
{
  char *datadir = NULL;
  char *refdir = NULL;

  /* relocatable_prog */
  set_program_name(argv0);

  /** default pkgdatadir (e.g. "/usr/share/freedink") **/
  {
    defaultpkgdatadir = br_build_path(DEFAULT_DATA_DIR, PACKAGE);
  }

  /** datadir (e.g. "/usr/share") **/
  {
    const char *datadir_relocatable;
    char *datadir_binreloc, *default_data_dir;
    
    /* First, ask relocable-prog */
    /* Put in a variable to avoid "comparison with string literal" */
    default_data_dir = DEFAULT_DATA_DIR;
    datadir_relocatable = relocate(default_data_dir);
    
    /* Then ask binreloc - it handles ../share even if CWD != "bin"
       (e.g. "src"). However it's not as precise and portable ($PATH
       lookup, etc.). */
    BrInitError error;
    if (br_init(&error) == 0 && error != BR_INIT_ERROR_DISABLED)
      {
	log_warn("BinReloc failed to initialize (error code %d)", error);
	datadir_binreloc = strdup(datadir_relocatable);
      }
    else
      {
	datadir_binreloc = br_find_data_dir(datadir_relocatable);
      }
    
    /* Free relocable-prog's path, if necessary */
    if (datadir_relocatable != default_data_dir)
      free((char *)datadir_relocatable);
    
    /* BinReloc always return a newly allocated string, with either the
       built directory, or a copy of its argument */
    datadir = datadir_binreloc;
  }
  
  /** pkgdatadir (e.g. ".local/share/freedink") **/
  {
    pkgdatadir = br_build_path(datadir, PACKAGE);
  }

  /** exedir (e.g. "C:/Program Files/Dink Smallwood") **/
  {
    char* fullprogname = get_full_program_name();
    if (fullprogname != NULL)
      exefile = strdup(fullprogname);
    else
      exefile = strdup(argv0);
    log_info("Hi, I'm '%s'", exefile);
    /* gnulib's dir_name always returns a newly xalloc'd string */
    exedir = dir_name(exefile);
  }

  /** refdir  (e.g. "/usr/share/dink") **/
  {
    /** => refdir **/
    char* match = NULL;
    int nb_dirs = 8;
    char** lookup = malloc(sizeof(char*) * nb_dirs);
    int i = 0;
    if (refdir_opt == NULL)
      lookup[0] = NULL;
    else
      lookup[0] = refdir_opt;
    lookup[1] = ".";
    lookup[2] = exedir;

    char *default3 = NULL, *default4 = NULL, *default5 = NULL,
      *default6 = NULL, *default7 = NULL;
    /* FHS mentions optional 'share/games' which some Debian packagers
       seem to be found of */
    /* Packagers: don't alter these paths. FreeDink must run in a
       _consistent_ way across platforms. If you need an alternate
       path, consider using ./configure --prefix=..., or contact
       bug-freedink@gnu.org to discuss it. */
    default3 = br_build_path(datadir, "dink");
    default4 = "/usr/local/share/games/dink";
    default5 = "/usr/local/share/dink";
    default6 = "/usr/share/games/dink";
    default7 = "/usr/share/dink";
    lookup[3] = default3;
    lookup[4] = default4;
    lookup[5] = default5;
    lookup[6] = default6;
    lookup[7] = default7;

    for (; i < nb_dirs; i++)
      {
	char *dir_graphics_ci = NULL, *dir_tiles_ci = NULL;
	if (lookup[i] == NULL)
	  continue;
	dir_graphics_ci = br_build_path(lookup[i], "dink/graphics");
	dir_tiles_ci = br_build_path(lookup[i], "dink/tiles");
	ciconvert(dir_graphics_ci);
	ciconvert(dir_tiles_ci);
	if (is_directory(dir_graphics_ci) && is_directory(dir_tiles_ci))
	  {
	    match = lookup[i];
	  }

	if (match == NULL && i == 0)
	  {
	    msgbox_init_error("Invalid --refdir option: %s and/or %s are not accessible.",
			      dir_graphics_ci, dir_tiles_ci);
	    exit(1);
	  }

	free(dir_graphics_ci);
	free(dir_tiles_ci);
	if (match != NULL)
	    break;
      }
    refdir = match;
    if (refdir != NULL)
      {
	refdir = strdup(refdir);
      }
    else
      {
	char *msg = NULL;
	asprintf_append(&msg, "Error: cannot find reference directory (--refdir). I looked in:\n");
	// lookup[0] already treated above
	asprintf_append(&msg, "- %s [current dir]\n", lookup[1]);
	asprintf_append(&msg, "- %s [exedir]\n", lookup[2]);
	asprintf_append(&msg, "- %s [detected prefix]\n", lookup[3]);
	asprintf_append(&msg, "- %s [/usr/local/share/games prefix]\n", lookup[4]);
	asprintf_append(&msg, "- %s [/usr/local/share prefix]\n", lookup[5]);
	asprintf_append(&msg, "- %s [/usr/share/games prefix]\n", lookup[6]);
	asprintf_append(&msg, "- %s [/usr/share prefix]\n", lookup[7]);
	asprintf_append(&msg, "The reference directory contains among others the "
		"'dink/graphics/' and 'dink/tiles/' directories (as well as "
		"D-Mods).");
	msgbox_init_error(msg);
	free(msg);
	exit(1);
      }

    free(default3); // br_build_path()
    free(lookup);
  }

  /** fallbackdir (e.g. "/usr/share/dink/dink") **/
  /* (directory used when a file cannot be found in a D-Mod) */
  {
    fallbackdir = br_strcat(refdir, "/dink");
  }

  /** dmoddir (e.g. "/usr/share/dink/island") **/
  {
    if (dmoddir_opt != NULL && is_directory(dmoddir_opt))
      {
	/* Use path given on the command line, either a full path or a
	   path relative to the current directory. */
	/* Note: don't search for "dink" in the default dir if no
	   '-game' option was given */
	dmoddir = strdup(dmoddir_opt);
      }
    else
      {
	/* Use path given on the command line, relative to $refdir */
	char *subdir = dmoddir_opt;
	if (subdir == NULL)
	  subdir = "dink";
	dmoddir = malloc(strlen(refdir) + 1 + strlen(subdir) + 1);
	strcpy(dmoddir, refdir);
	strcat(dmoddir, "/");
	strcat(dmoddir, subdir);
	if (!is_directory(dmoddir))
	  {
	    char *msg = NULL;

	    asprintf_append(&msg, "Error: D-Mod directory '%s' doesn't exist. I looked in:\n", subdir);
	    if (dmoddir_opt != NULL)
	      asprintf_append(&msg, "- ./%s\n", dmoddir_opt);
	    asprintf_append(&msg, "- %s (refdir is '%s')", dmoddir, refdir);

	    msgbox_init_error(msg);
	    free(msg);
	    exit(1);
	  }
      }
    /* Strip slashes */
    while (strlen(dmoddir) > 0 && dmoddir[strlen(dmoddir) - 1] == '/')
      dmoddir[strlen(dmoddir) - 1] = '\0';
  }

  /** dmodname (e.g. "island") **/
  /* Used to save games in ~/.dink/<dmod>/... */
  {
    dmodname = base_name(dmoddir);
    if (strcmp(dmodname, ".") == 0)
      {
	free(dmodname);
	char *canonical_dmoddir = canonicalize_file_name(dmoddir);
	dmodname = base_name(canonical_dmoddir);
	free(canonical_dmoddir);
      }
  }

  /** userappdir (e.g. "~/.dink") **/
  {
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
    userappdir = malloc(MAX_PATH);
    /* C:\Documents and Settings\name\Application Data */
    SHGetSpecialFolderPath(NULL, userappdir, CSIDL_APPDATA, 1);
#else
    char* envhome = getenv("HOME");
    if (envhome != NULL)
      userappdir = strdup(getenv("HOME"));
#endif
    if (userappdir != NULL)
      {
	userappdir = realloc(userappdir, strlen(userappdir) + 1 + 1 + strlen(PACKAGE) + 1);
	strcat(userappdir, "/");
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#else
	strcat(userappdir, ".");
#endif
	strcat(userappdir, "dink");
      }
    else
      {
	// No detected home directory - saving in the reference
	// directory
	userappdir = strdup(refdir);
      }
  }

  log_info("exedir = %s", exedir);
  log_info("datadir = %s", datadir);
  log_info("pkgdatadir = %s", pkgdatadir);
  log_info("defaultpkgdatadir = %s", defaultpkgdatadir);
  log_info("refdir = %s", refdir);
  log_info("dmoddir = %s", dmoddir);
  log_info("dmodname = %s", dmodname);
  log_info("userappdir = %s", userappdir);

  free(datadir);
  free(refdir);
}


const char *paths_getdefaultpkgdatadir(void)
{
  return defaultpkgdatadir;
}


const char *paths_getpkgdatadir(void)
{
  return pkgdatadir;
}
const char *paths_getdmoddir(void)
{
  return dmoddir;
}
const char *paths_getdmodname(void)
{
  return dmodname;
}

const char *paths_getfallbackdir(void)
{
  return fallbackdir;
}

const char *paths_getexedir(void)
{
  return exedir;
}

const char *paths_getexefile(void)
{
  return exefile;
}


char* paths_dmodfile(char *file)
{
  char *fullpath = br_build_path(dmoddir, file);
  ciconvert(fullpath);
  return fullpath;
}

FILE* paths_dmodfile_fopen(char *file, char *mode)
{
  char *fullpath = paths_dmodfile(file);
  FILE *result = fopen(fullpath, mode);
  free(fullpath);
  return result;
}

char* paths_fallbackfile(char *file)
{
  char *fullpath = br_build_path(fallbackdir, file);
  ciconvert(fullpath);
  return fullpath;
}

FILE* paths_fallbackfile_fopen(char *file, char *mode)
{
  char *fullpath = paths_fallbackfile(file);
  FILE *result = fopen(fullpath, mode);
  free(fullpath);
  return result;
}

char* paths_defaultpkgdatafile(char *file)
{
  char *fullpath = br_build_path(defaultpkgdatadir, file);
  ciconvert(fullpath);
  return fullpath;
}

FILE* paths_defaultpkgdatafile_fopen(char *file, char *mode)
{
  char *fullpath = paths_defaultpkgdatafile(file);
  FILE *result = fopen(fullpath, mode);
  free(fullpath);
  return result;
}

char* paths_pkgdatafile(char *file)
{
  char *fullpath = br_build_path(pkgdatadir, file);
  ciconvert(fullpath);
  return fullpath;
}

FILE* paths_pkgdatafile_fopen(char *file, char *mode)
{
  char *fullpath = paths_pkgdatafile(file);
  FILE *result = fopen(fullpath, mode);
  free(fullpath);
  return result;
}

char* paths_exedirfile(char *file)
{
  char *fullpath = br_build_path(exedir, file);
  ciconvert(fullpath);
  return fullpath;
}

FILE* paths_exedirfile_fopen(char *file, char *mode)
{
  char *fullpath = paths_exedirfile(file);
  FILE *result = fopen(fullpath, mode);
  free(fullpath);
  return result;
}

FILE *paths_savegame_fopen(int num, char *mode)
{
  char *fullpath_in_dmoddir = NULL;
  char *fullpath_in_userappdir = NULL;
  FILE *fp = NULL;

  /* 20 decimal digits max for 64bit integer - should be enough :) */
  char file[4 + 20 + 4 + 1];
  sprintf(file, "save%d.dat", num);


  /** fullpath_in_userappdir **/
  char *savedir = strdup(userappdir);
  savedir = realloc(savedir, strlen(userappdir) + 1 + strlen(dmodname) + 1);
  strcat(savedir, "/");
  strcat(savedir, dmodname);
  /* Create directories if needed */
  if (strchr(mode, 'w') != NULL || strchr(mode, 'a') != NULL)
      /* Note: 0777 & umask => 0755 in common case */
      if ((!is_directory(userappdir) && (mkdir(userappdir, 0777) < 0))
	  || (!is_directory(savedir) && (mkdir(savedir, 0777) < 0)))
	{
	  free(savedir);
	  return NULL;
	}
  fullpath_in_userappdir = br_build_path(savedir, file);
  ciconvert(fullpath_in_userappdir);
  free(savedir);


  /** fullpath_in_dmoddir **/
  fullpath_in_dmoddir = paths_dmodfile(file);
  ciconvert(fullpath_in_dmoddir);
  

  /* Try ~/.dink (if present) when reading - but don't try that
     first when writing */
  if (strchr(mode, 'r') != NULL)
    fp = fopen(fullpath_in_userappdir, mode);

  /* Try in the D-Mod dir */
  if (fp == NULL)
    fp = fopen(fullpath_in_dmoddir, mode);

  /* Then try in ~/.dink */
  if (fp == NULL)
    fp = fopen(fullpath_in_userappdir, mode);

  free(fullpath_in_dmoddir);
  free(fullpath_in_userappdir);

  return fp;
}

void paths_quit(void)
{
  free(defaultpkgdatadir);
  free(pkgdatadir);
  free(exedir);
  free(fallbackdir);
  free(dmoddir);
  free(dmodname);
  free(userappdir);
  free(exefile);

  defaultpkgdatadir = NULL;
  pkgdatadir        = NULL;
  exedir            = NULL;
  fallbackdir       = NULL;
  dmoddir           = NULL;
  dmodname          = NULL;
  userappdir        = NULL;
  exefile           = NULL;
}
