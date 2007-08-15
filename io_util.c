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

#include <stdio.h>
#include <string.h>
#include <dirent.h>

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
