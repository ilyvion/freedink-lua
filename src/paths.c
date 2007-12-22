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

#include "io_util.h"

#include "paths.h"

static char* pkgdatadir = NULL;
static char* fallbackdir = NULL; /* default = pkgdatadir + '/dink' */
static char* dmoddir = NULL; /* pkgdatadir + '/dink' */

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

  /** refdir **/
  char *refdir;
  {
    /** exedir (e.g. "C:/Program Files/Dink Smallwood") **/
    char *exedir;
    {
      char *myself = strdup(get_full_program_name());
      int len = strlen(myself);
      char *pc = myself + len;
      while (--pc >= myself && *pc != '/');
      if (*pc == '/')
	*pc = '\0';
      exedir = myself;
    }

    /** => refdir **/
    char* match = NULL;
    char* lookup[4];
    int i = 0;
    /* All entries may be free()'d later on,use strdup */
    if (refdir_opt == NULL)
      lookup[0] = NULL;
    else
      lookup[0] = strdup(refdir_opt);
    lookup[1] = strdup(".");
    lookup[2] = exedir;
    lookup[3] = strdup(pkgdatadir);

    for (; i < 4; i++)
      {
	char *dir_graphics_ci, *dir_tiles_ci;
	if (lookup[i] == NULL)
	  continue;
	dir_graphics_ci = br_strcat(lookup[i], "/dink/graphics");
	dir_tiles_ci = br_strcat(lookup[i], "/dink/tiles");
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

    for (i = 0; i < 4; i++)
      if (lookup[i] != NULL && lookup[i] != refdir)
	free(lookup[i]);
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

/*   char *tmp[2000]; */
/*   printf("pwd = %s\n", getcwd(tmp, 2000)); */
  printf("refdir = %s\n", refdir);
  printf("pkgdatadir = %s\n", pkgdatadir);
  printf("dmoddir = %s\n", dmoddir);
}

const char *paths_dmoddir(void)
{
  return dmoddir;
}

char *paths_dmodfile(char *file)
{
  char *path = malloc(strlen(dmoddir) + 1 + strlen(file) + 1);
  sprintf(path, "%s/%s", dmoddir, file);
  return path;
}

const char *paths_fallbackdir(void)
{
  return fallbackdir;
}

char *paths_fallbackfile(char *file)
{
  char *path = malloc(strlen(fallbackdir) + 1 + strlen(file) + 1);
  sprintf(path, "%s/%s", fallbackdir, file);
  return path;
}

const char *paths_pkgdatadir(void)
{
  return pkgdatadir;
}

char *paths_pkgdatafile(char *file)
{
  char *path = malloc(strlen(pkgdatadir) + 1 + strlen(file) + 1);
  sprintf(path, "%s/%s", pkgdatadir, file);
  return path;
}
