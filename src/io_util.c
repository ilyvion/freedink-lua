/**
 * Filesystem helpers

 * Copyright (C) 2005, 2007  Sylvain Beucler

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
#include <string.h>
#include <dirent.h>
#include <errno.h>

/* stat(2) */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "binreloc.h"
#include "progname.h"
#include "relocatable.h"
#include "SDL.h"
#include "SDL_rwops_zzip.h"

/* TODO: use Gnulib */
#ifndef PATH_MAX
#  define PATH_MAX 255
#endif

static char* pkgdatadir = NULL;
static char* datafallbackdir = NULL; /* default = pkgdatadir + '/dink' */
static char* dmoddir = NULL; /* pkgdatadir + '/dink' */

/* Returns a pointer to the end of the current path element (file or
   directory) */
static char*
end_of_elt(char *str)
{
  char *p = str;
  while ((*p != '/') && (*p != '\\') && (*p != '\0'))
    p++;
  return p;
}

/* Look for filename case-insensitively, to mimic MS Woe's
   case-insensitive file system; modifies the 'filename' buffer and
   returns case-sensitive path. Also converts '\' to '/'. */
/* I repeat: the filename buffer is modified, so it cannot be "a
   constant string" */
char*
ciconvert (char *filename)
{
#ifndef _WIN32
  /* Parse all the directories that composes filename */
  char cur_dir[PATH_MAX];
  char *pcur_elt, *pend_of_elt, *pend_of_cur_dir;
  int error = 0;
  
  /* First simply try to open the file, constant complexity. Permits
     decent speed for careful authors who write exact filenames. */
  FILE *f;
  if ((f = fopen(filename, "r")) != NULL)
    {
      fclose(f);
      return filename;
    }

  /* Else, check each path element of the filename */
  if (*filename == '/')
    {
      strcpy(cur_dir, "/");
      pend_of_cur_dir = cur_dir + 1;
    }
  else
    {
      strcpy(cur_dir, "./");
      pend_of_cur_dir = cur_dir + 2;
    }
  pcur_elt = filename;
  do
    {
      char end_of_elt_backup;

      /* skip leading or multiple slashes */
      while (*pcur_elt == '/' || *pcur_elt == '\\')
	{
	  *pcur_elt = '/';
	  pcur_elt++;
	}

      pend_of_elt = end_of_elt(pcur_elt);
      end_of_elt_backup = *pend_of_elt;
      *pend_of_elt = '\0';
      /* pcur_elt is now a \0-terminated string containing the current
	 path element. */
      
      /* Now check if there's a matching entry in the directory */
      DIR *list;
      struct dirent *entry;
      int found = 0;
      
      list = opendir (cur_dir);
      if (list != NULL)
	{
	  /* note: we may want to sort list first */
	  while ((entry = readdir (list)) != NULL && !found)
	    {
	      if (strcasecmp(pcur_elt, entry->d_name) == 0)
		{
		  /* Good case-insensitive match: replace the
		     user-provided filename with it */
		  strcpy(pcur_elt, entry->d_name);
		  found = 1;
		}
	    }
	  closedir (list);
	}
      if (!found)
	error = 1;

      if (end_of_elt_backup != '\0')
	*pend_of_elt = '/'; /* restore */


      /* Prepare next directory */
      {
	int cur_elt_len = pend_of_elt - pcur_elt;
	strncpy(pend_of_cur_dir, pcur_elt, cur_elt_len + 1);
	pend_of_cur_dir += cur_elt_len + 1;
	*pend_of_cur_dir = '\0';
      }

      /* go to the next path element */
      pcur_elt = pend_of_elt + 1;
    }
  while(*pend_of_elt != '\0' && !error);

  /* If there was an error, we return a half-converted path (maybe the
     file didn't exist yet, but leading directories still needed to be
     converted); otherwise, filename contains the fully-converted
     path, ready to be opened on a case-sensitive filesystem. */
#endif /* !_WIN32 */
  return filename;
}

/* Variant to make it easier to use constant strings: */
/* char tmp_filename[PATH_MAX]; */
/* ... */
/* if (exists(ciconvert("/usr/share/freedink/myfile1", tmp_filename))) */
/* ... */
/* if (exists(ciconvert("/usr/share/freedink/myfile2", tmp_filename))) */
char*
ciconvertbuf (const char *filename, char* buf)
{
  strcpy(buf, filename);
  return ciconvert(buf);
}

/* Does this file exist and can be opened? */
int exist(char name[255])
{
  char tmp_filename[PATH_MAX];

  FILE *fp;
  fp = fopen(ciconvertbuf(name, tmp_filename), "rb");
  if (!fp)
    return 0;
  
  fclose(fp);
  return 1;
}

/* Is it a directory that exists? */
int is_directory(char *name)
{
  char tmp_filename[PATH_MAX];

  struct stat buf;
  stat(ciconvertbuf(name, tmp_filename), &buf);
  return S_ISDIR(buf.st_mode);
}


SDL_RWops *find_resource_as_rwops(char *name)
{
  /* Look in appended ZIP archive */
  SDL_RWops* rwops = NULL;
  /* get_full_program_name() checks /proc (Linux), then argv[0] +
     PATH. Under Woe it uses GetModuleFileName(). The only way to make
     it fail is to execl("./freedink", "idontexist", 0); */
  char *myself = get_full_program_name();
  char *zippath = malloc(strlen(myself) + 1 + strlen(name) + 1);
  strcpy(zippath, myself);
  strcat(zippath, "/");
  strcat(zippath, name);
  /* sample zippath: "/usr/bin/freedink/LiberationSans-Regular.ttf" */
  rwops = SDL_RWFromZZIP(zippath, "rb");
  return rwops;
  /* Retrieve error (if any) with :
     printf("%s\n", strerror(errno)); */
}

void init_paths(char *distdir_opt, char *dmoddir_opt)
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
    char *retpath = (char*) realloc(retpath, strlen(datadir) + 1 + strlen(PACKAGE) + 1);
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

  /** distdir **/
  char *distdir;
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

    /** => distdir **/
    char* match;
    char* lookup[4];
    // TODO:
    lookup[0] = distdir_opt;
    lookup[1] = ".";
    lookup[2] = exedir;
    lookup[3] = pkgdatadir;
    if (lookup[0] == NULL)
      lookup[0] = lookup[1];
    for (int i = 0; i < 4; i++)
      {
	char *dir_story_ci, *
dir_tiles_ci;
	dir_story_ci = br_strcat(lookup[i], "/dink/graphics");
	dir_tiles_ci = br_strcat(lookup[i], "/dink/tiles");
	if (is_directory(dir_story_ci) && is_directory(dir_tiles_ci))
	  {
	    match = lookup[i];
	  }
	free(dir_story_ci);
	free(dir_tiles_ci);
	if (match != NULL)
	  break;
      }
    distdir = match;

    if (distdir == NULL)
      {
	fprintf(stderr, "Error: distdir doesn't exist (looked in...)\n");
	exit(1);
      }
  }

  /** datafallbackdir (e.g. "/usr/share/freedink/dink") **/
  {
    datafallbackdir = br_strcat(distdir, "/dink");
  }

  /** dmoddir (e.g. "/usr/share/freedink/island") **/
  {
    if (dmoddir_opt == NULL)
      dmoddir_opt = "dink";
    if (is_directory(dmoddir_opt))
      {
	dmoddir = dmoddir_opt;
      }
    else
      {
	dmoddir = malloc(strlen(distdir) + 1 + strlen(dmoddir_opt) + 1);
	strcpy(dmoddir, distdir);
	strcat(dmoddir, "/");
	strcat(dmoddir, dmoddir_opt);
	if (!is_directory(dmoddir_opt))
	  {
	    fprintf(stderr, "Error: dmoddir doesn't exist (looked in...)\n");
	    exit(1);
	  }
      }
  }
}

/* Try different options to get a data file, such as the default font,
   the application icon, etc. */
/* datadir + "/" + PACKAGE + "/" + filename */
char *
find_data_file(const char *filename)
{
  char *dir = NULL;
  char *retpath = malloc(1);

  /* Try dirname(executable)/../share or dirname(executable)/share */
#ifdef _WIN32
  {
    /* Get executable's directory */
    char myself[MAX_PATH];
    int success = GetModuleFileName(NULL, myself, MAX_PATH);
    if (success)
      {
	int len = strlen(myself);
	char *pc = myself + len;
	while (--pc >= myself && *pc != '/');
	if (*pc == '/')
	  {
	    *pc = '\0';
	    dir = myself;
	  }
      }
  }
#else
  dir = br_find_data_dir(DEFAULT_DATA_DIR);
#endif
  if (dir != NULL)
    {
      retpath = (char*) realloc(retpath, strlen(dir) + 1 + strlen(PACKAGE) + 1 + strlen(filename) + 1);
      sprintf(retpath, "%s/%s/%s", dir, PACKAGE, filename);
      if (exist(retpath))
	{
	  free(dir);
	  return retpath;
	}
    }

  /* Try compile-time datadir */
  retpath = (char*) realloc(retpath, strlen(DEFAULT_DATA_DIR) + 1 + strlen(PACKAGE) + 1 + strlen(filename) + 1);
  sprintf(retpath, "%s/%s/%s", DEFAULT_DATA_DIR, PACKAGE, filename);
  if (exist(retpath))
    return retpath;

  /* Try . */
  retpath = (char*) realloc(retpath, strlen(".") + 1 + strlen(filename) + 1);
  sprintf(retpath, "./%s", filename);
  if (exist(retpath))
    return retpath;
  
  free(retpath);
  return NULL;
}

#ifdef TEST
/* find ../.. -print0 | tr a-z A-Z | xargs -0 ./a.out */
int
main (int argc, char *argv[])
{
  if (argc == 1)
    fprintf (stderr, "Usage: %s file...\n", argv[0]);
  while (*++argv)
    {
      char before[PATH_MAX];
      char after[PATH_MAX];
      FILE *f = NULL;
      strcpy(before, *argv);
      ciconvert(after);
      strcpy(after, *argv);
      printf("%s => ", before);
      printf("%s\n", after);
      if ((strcasecmp(before, after) != 0)
	  || ((f = fopen(after, "r")) == NULL))
	fprintf(stderr, "ERROR with %s => %s\n", before, after);
      if (f != NULL) fclose(f);
    }
  return 0;
  /* other tests: */
  /* - with multiple slashes: .//file */
  /* - from the root: find /tmp/ ... */
  /* - files containing '\' should fail */
  /* - with a file and a directory with the same name, in the same
       directory: currently we don't support that, as it's not
       portable anyway */
}
#endif
