/**
 * Compute and store the search paths

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "relocatable.h"
#include "progname.h"
#include "binreloc.h"

/* canonicalize_file_name */
#include "canonicalize.h"

/* basename */
#include <libgen.h>
#include "dirname.h"

/* mkdir */
#include <sys/stat.h>
#include <sys/types.h>

#include "io_util.h"
#include "paths.h"

static char* pkgdatadir = NULL;
static char* exedir = NULL;
static char* fallbackdir = NULL;
static char* dmoddir = NULL;
static char* dmodname = NULL;
static char* userappdir = NULL;

void paths_init(char *refdir_opt, char *dmoddir_opt)
{
  /** pkgdatadir (e.g. "/usr/share/freedink") **/
  {
    /** datadir (e.g. "/usr/share") **/
    char *datadir;
    {
      const char *datadir_relocatable;
      char *datadir_binreloc, *default_data_dir;
      
      /* First, ask relocable-prog */
      /* copy to avoid "comparison with string literal" */
      default_data_dir = strdup(DEFAULT_DATA_DIR);
      datadir_relocatable = relocate(default_data_dir);
      
      /* Then ask binreloc - it handles ../share even if CWD != "bin"
	 (e.g. "src"). However it's not as precise and portable ($PATH
	 lookup, etc.). */
      datadir_binreloc = br_find_data_dir(datadir_relocatable);
      
      /* Free relocable-prog's path, if necessary */
      if (datadir_relocatable != default_data_dir)
	free((char *)datadir_relocatable);
      free(default_data_dir);
      
      /* Binreloc always return a newly allocated string, with either the
	 built directory, or a copy of its argument */
      datadir = datadir_binreloc;
    }
    
    /** => pkgdatadir **/
    char *retpath = (char*) malloc(strlen(datadir) + 1 + strlen(PACKAGE) + 1);
    sprintf(retpath, "%s/%s", datadir, PACKAGE);
    if (is_directory(retpath))
      {
	/* Relocated $datadir/freedink exists */
	pkgdatadir = retpath;
      }
    else
      {
	/* Fallback to compile-time datadir */
	retpath = (char*) realloc(retpath, strlen(DEFAULT_DATA_DIR) + 1 + strlen(PACKAGE) + 1);
	sprintf(retpath, "%s/%s", DEFAULT_DATA_DIR, PACKAGE);
	pkgdatadir = retpath;
      }
  }

  /** exedir (e.g. "C:/Program Files/Dink Smallwood") **/
  {
    exedir = dirname(get_full_program_name());
  }

  /** refdir **/
  char *refdir;
  {
    /** => refdir **/
    char* match = NULL;
    char* lookup[4];
    int i = 0;
    if (refdir_opt == NULL)
      lookup[0] = NULL;
    else
      lookup[0] = refdir_opt;
    lookup[1] = ".";
    lookup[2] = exedir;
    lookup[3] = pkgdatadir;

    for (; i < 4; i++)
      {
	char *dir_graphics_ci, *dir_tiles_ci;
	if (lookup[i] == NULL)
	  continue;
	dir_graphics_ci = br_strcat(lookup[i], "/dink/graphics");
	dir_tiles_ci = br_strcat(lookup[i], "/dink/tiles");
	ciconvert(dir_graphics_ci);
	ciconvert(dir_tiles_ci);
	if (is_directory(dir_graphics_ci) && is_directory(dir_tiles_ci))
	  {
	    match = lookup[i];
	  }

	if (match == NULL && i == 0)
	  {
	    fprintf(stderr, "Invalid refdir: %s and/or %s are not accessible.\n",
		    dir_graphics_ci, dir_tiles_ci);
	    exit(1);
	  }

	free(dir_graphics_ci);
	free(dir_tiles_ci);
	if (match != NULL)
	    break;
      }
    refdir = match;

    if (refdir == NULL)
      {
	fprintf(stderr, "Error: cannot find reference directory (--refdir). I looked in:\n");
	int i = 0;
	for (i = 0; i < 4; i++)
	  {
	    if (lookup[i] != NULL)
	      {
		char *dir_graphics_ci;
		dir_graphics_ci = lookup[i];
		fprintf(stderr, "- %s\n", dir_graphics_ci);
	      }
	  }
	fprintf(stderr, "It should contain 'dink/graphics/' and 'dink/tiles/' directories (as well as D-Mods).\n");
	exit(1);
      }
  }

  /** fallbackdir (e.g. "/usr/share/freedink/dink") **/
  {
    fallbackdir = br_strcat(refdir, "/dink");
  }

  /** dmoddir (e.g. "/usr/share/freedink/island") **/
  {
    if (dmoddir_opt == NULL)
      dmoddir_opt = "dink";
    if (is_directory(dmoddir_opt))
      {
	dmoddir = strdup(dmoddir_opt);
      }
    else
      {
	dmoddir = malloc(strlen(refdir) + 1 + strlen(dmoddir_opt) + 1);
	strcpy(dmoddir, refdir);
	strcat(dmoddir, "/");
	strcat(dmoddir, dmoddir_opt);
	if (!is_directory(dmoddir))
	  {
	    fprintf(stderr, "Error: D-Mod directory '%s' doesn't exist. I looked in:\n", dmoddir_opt);
	    fprintf(stderr, "- ./%s\n", dmoddir_opt);
	    fprintf(stderr, "- %s (refdir is '%s')\n", dmoddir, refdir);
	    exit(1);
	  }
      }
  }

  /** dmodname (e.g. "island") **/
  /* Used to save games in ~/.freedink/<dmod>/... */
  {
    dmodname = base_name(dmoddir);
    if (strcmp(dmodname, ".") == 0)
      {
	free(dmodname);
	char *canonical_dmoddir = canonicalize_file_name(dmoddir);
	dmodname = basename(canonical_dmoddir);
	free(canonical_dmoddir);
      }
  }

  /** userappdir (e.g. "~/.freedink") **/
  {
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0401
#include <windows.h>
#include <shlobj.h>
    userappdir = malloc(MAX_PATH);
    /* C:\Documents and Settings\name\Application Data */
    SHGetSpecialFolderPath(NULL, userappdir, CSIDL_APPDATA, 1);
#else
    userappdir = strdup(getenv("HOME"));
#endif
    userappdir = realloc(userappdir, strlen(userappdir) + 1 + 1 + strlen(PACKAGE) + 1);
    strcat(userappdir, "/");
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#else
    strcat(userappdir, ".");
#endif
    strcat(userappdir, PACKAGE);
  }

  printf("refdir = %s\n", refdir);
  printf("exedir = %s\n", exedir);
  printf("pkgdatadir = %s\n", pkgdatadir);
  printf("dmoddir = %s\n", dmoddir);
  printf("dmodname = %s\n", dmodname);
  printf("userappdir = %s\n", userappdir);
}


const char *paths_pkgdatadir(void)
{
  return pkgdatadir;
}

const char *paths_dmoddir(void)
{
  return dmoddir;
}

const char *paths_fallbackdir(void)
{
  return fallbackdir;
}

const char *paths_exedir(void)
{
  return exedir;
}


char *paths_pkgdatafile(char *file)
{
  char *path = malloc(strlen(pkgdatadir) + 1 + strlen(file) + 1);
  sprintf(path, "%s/%s", pkgdatadir, file);
  return path;
}

char *paths_dmodfile(char *file)
{
  char *path = malloc(strlen(dmoddir) + 1 + strlen(file) + 1);
  sprintf(path, "%s/%s", dmoddir, file);
  return path;
}

char *paths_fallbackfile(char *file)
{
  char *path = malloc(strlen(fallbackdir) + 1 + strlen(file) + 1);
  sprintf(path, "%s/%s", fallbackdir, file);
  return path;
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
  fullpath_in_userappdir = malloc(strlen(savedir) + 1 + strlen(file) + 1);
  sprintf(fullpath_in_userappdir, "%s/%s", savedir, file);
  free(savedir);


  /** fullpath_in_dmoddir **/
  fullpath_in_dmoddir = paths_dmodfile(file);
  ciconvert(fullpath_in_dmoddir);
  

  /* Try ~/.freedink (if present) when reading - but don't try that
     first when writing */
  if (strchr(mode, 'r') != NULL)
    fp = fopen(fullpath_in_userappdir, mode);

  /* Try in the D-Mod dir */
  if (fp == NULL)
    fp = fopen(fullpath_in_dmoddir, mode);

  /* Then try in ~/.freedink */
  if (fp == NULL)
    fp = fopen(fullpath_in_userappdir, mode);

  free(fullpath_in_dmoddir);
  free(fullpath_in_userappdir);

  return fp;
}
